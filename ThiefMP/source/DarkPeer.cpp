/*************************************************************
* File: DarkPeer.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements DirectPlay8Peer networking Dark engine games.
*************************************************************/

// Note: the decision to use peer-to-peer networking was made due to the design of the original
// DirectPlay4 network code in Dark. Ideally, this would be client/server

#include "stdafx.h"

#include "Main.h"
#include "GlobalServer.h"
#include "DarkPeer.h"

#ifdef _THIEFBUILD
#include "Engine\inc\Crc32.h"

#include "Client.h"
#include "PlayerDB.h"
#include "Sound.h"
#include "DarkOverlay.h"
#include "Patcher.h"
#endif

void NetDisconnectListenerInit();
void WrapperTerm();

IDarkNet* g_pDarkNet = NULL;

// Global variable that tracks the network state
int g_Net = STATE_None;
bool g_bNeedSnapshot = false;

const int kHeartbeatInterval = 15000;

CCriticalSection csReceive("receive");
SimpleQueue<NetMessage> NewReceiveQueue; // Handles new packets
ConnectAttempt g_ConnectAttempt;

bool CDarkPeer::m_bLoadedNetCommands = false;
bool CDarkPeer::m_bLoadedBinds = false;

CDarkPeer::CDarkPeer(NetState state) : m_csEnumResponse("peer enum response")
{
	m_pDP = NULL;
	m_pDeviceAddress = NULL;
	m_pHostAddress = NULL;
	m_gsc = NULL;
	m_pSessionMgr = NULL;
	m_queuePeak = 0;
	m_dpnidHost = kInvalidHostID;

	m_DarkQueue.Clear();
	NewReceiveQueue.Clear();

	if (state == STATE_Client)
	{
		m_bReceivedExistingPlayers = false;

		InitClient();
		m_ConnectedEvent = CreateEvent(NULL, true, false, NULL);
	}
	else if (state == STATE_Host)
	{
		InitServer();

		// Start up global connection, if set to advertise server
		if (Cfg.GetBool("ListOnGlobal"))
		{
			if (!InitGlobalLink())
				Log.Print("Failed to list server to global.");
		}
	}

	EventManager::Broadcast(ET_NetStart, NULL);
	EventManager::RegisterListener(ET_GameMode, this);
	EventManager::RegisterListener(ET_MissionLoaded, this);

#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
	if (!m_bLoadedBinds)
	{
		g_pInputBinder->LoadBndFile("user_mp.bnd", 0x21, NULL);
		m_bLoadedBinds = true;
	}
	if (!m_bLoadedNetCommands)
	{
		InitNetCommands();
		m_bLoadedNetCommands = true;
	}

	NetDisconnectListenerInit();

#if (GAME == GAME_THIEF)
	MemPatcher::WritePtr(g_MultiplayerPatches);
	MemPatcher::WriteRaw(g_MultiplayerRawPatches, NULL, NULL);

	PatchCommands(true);
#endif
	Debug.SetExceptionFilter(); // enable crash logger
#endif
}

CDarkPeer::~CDarkPeer()
{
	g_pDarkNet = NULL;

	if (!EventManager::UnregisterListener(ET_GameMode, this))
		Log.Print("Failed to unregister listener.");
	if (!EventManager::UnregisterListener(ET_MissionLoaded, this))
		Log.Print("Failed to unregister listener.");

	for (std::map<DPNID, DP4Data*>::iterator iter = m_DP4PlayerData.begin(); iter != m_DP4PlayerData.end(); iter++)
	{
		delete iter->second;

		if (Debug.IsFlagSet(DEBUG_NET))
			ConPrintF("Deleting player data for DPNID %x.", iter->first);
	}

	//if (g_pNetMan)
	//	g_pNetMan->Unlisten(m_hDisconnectListener);

	GS_DestroyClient(m_gsc);

#if (GAME == GAME_THIEF)
	MemPatcher::RestorePtr(g_MultiplayerPatches);
	MemPatcher::RestoreRaw(g_MultiplayerRawPatches);

	PatchCommands(false);
#endif

	Debug.RestoreExceptionFilter();

	EventManager::Broadcast(ET_NetClose, NULL);
}

//======================================================================================
// Purpose: Cancels the currently active host enumeration.
//======================================================================================
void CDarkPeer::CancelEnumeration()
{
	if (m_enumSessionAsyncOp)
	{
		if (m_pDP)
			m_pDP->CancelAsyncOperation(m_enumSessionAsyncOp, 0);
	}
}

//======================================================================================
// Purpose: Cancels all host enumerations.
//======================================================================================
void CDarkPeer::CancelAllEnumerations()
{
	if (m_pDP)
		m_pDP->CancelAsyncOperation(NULL, DPNCANCEL_ENUM);
}

//======================================================================================
// Purpose: Cancels all asynchronous operations and closes the DirectPlay session.
//======================================================================================
void CDarkPeer::Close()
{
	if (m_pDP)
	{
		m_pDP->CancelAsyncOperation(NULL, DPNCANCEL_ALL_OPERATIONS);
		m_pDP->Close(0);
	}
}

//======================================================================================
// Purpose: Closes connections in the event of a crash.
//======================================================================================
void CDarkPeer::CrashCleanup()
{
	dbgassert(g_pDarkNet);

	if (m_gsc)
	{
		//m_gsc->SendQuitting(IGSClient::ExitReason_AppCrash);
		GS_DestroyClient(m_gsc);
	}

	Close();

	// Shut down the DirectPlay 4 wrapper. Will also cause "this" to be deleted.
	WrapperTerm();
}

//==============================================================================
// ConnectToServer()
//
// Attempts to connect to a server running on the given IP address.
//==============================================================================
bool CDarkPeer::ConnectToServer(const char* serverIP, const char* password)
{
	EnumHosts(serverIP, password);

	if (WaitForSingleObject(m_ConnectedEvent, 5000) == WAIT_TIMEOUT)
	{
		ConPrintF("Connection timed out.");
		return false;
	}

#if 0
	StagingDisplay();	
#endif

	return true;
}

//==============================================================================
// DestroyPeer()
//
// Removes peer from session.
//==============================================================================
bool CDarkPeer::DestroyPeer(DPNID dpnidPeer, const void* pDestroyData, DWORD dataSize)
{
	if (FAILED(m_pDP->DestroyPeer(dpnidPeer, pDestroyData, dataSize, NULL)))
		return false;
	else
		return true;
}

//==============================================================================
// EnumHosts()
//
// Enumerates sessions running on the given IP address.
//==============================================================================
void CDarkPeer::EnumHosts(const char* serverIP, const char* password)
{
#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
	if (!g_ConnectAttempt.IsActive())
		return dbgassert(false && "no connect attempt?");
#endif

	dbgassert(m_pHostAddress);

	HRESULT hRes;
	DPN_APPLICATION_DESC dpnAppDesc;
	wchar_t wcBuff[150];

	AnsiToWide(wcBuff, serverIP, sizeof(wcBuff));

	ZeroMemory(&dpnAppDesc, sizeof(DPN_APPLICATION_DESC));
	dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
	dpnAppDesc.guidApplication = GAME_GUID;

	hRes = m_pHostAddress->AddComponent(DPNA_KEY_HOSTNAME, wcBuff, (DWORD)(wcslen(wcBuff)+1)*sizeof(WCHAR), DPNA_DATATYPE_STRING);

	int port = g_ConnectAttempt.GetPort();
	hRes = m_pHostAddress->AddComponent(DPNA_KEY_PORT, &port, sizeof(port), DPNA_DATATYPE_DWORD);

	hRes = m_pDP->EnumHosts(&dpnAppDesc, m_pHostAddress, m_pDeviceAddress, NULL, 0, INFINITE, 0, 
		INFINITE, NULL, &m_enumSessionAsyncOp, NULL);
	if (FAILED(hRes))
		ConPrintF("Failed to enumerate hosts (%x).", hRes);
}

//==============================================================================
// OnEnumerationResponse()
//
// Handles host enumeration responses, and connects to the returned session.
//==============================================================================
void CDarkPeer::OnEnumerationResponse(PDPNMSG_ENUM_HOSTS_RESPONSE pEnumHostsResponseMsg)
{
#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
	if (!g_ConnectAttempt.IsActive())
	{
		return;
	}
#endif

	HRESULT hRes;
	const DPN_APPLICATION_DESC* pAppDesc = pEnumHostsResponseMsg->pApplicationDescription;

#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
	m_pSessionMgr->SetSessionInfo(pEnumHostsResponseMsg->pApplicationDescription);
	const SessionInfo& info = GetSessionInfo();

	if (info.build != TMP_BUILD)
	{
		ConPrintF("Server version (%d) does not match client version. Connection failed.", info.build);
		CancelEnumeration();
		return;
	}

	NString sessName = pEnumHostsResponseMsg->pApplicationDescription->pwszSessionName;
	ConPrintF("Game found: %s  Players: %d/%d  Build: %d.", sessName.Str(), pAppDesc->dwCurrentPlayers, pAppDesc->dwMaxPlayers, info.build);
#endif

	hRes = SetPlayerInfo();
	if (FAILED(hRes))
		return Log.Print("Failed to set peer info.");

	// Set the password, if we have one
	if (g_ConnectAttempt.GetPassword())
	{
		(WCHAR*)pAppDesc->pwszPassword = (WCHAR*)g_ConnectAttempt.GetPassword();
	}

	hRes = m_pDP->Connect(pAppDesc, pEnumHostsResponseMsg->pAddressSender, pEnumHostsResponseMsg->pAddressDevice, 
		NULL, NULL, NULL, 0, this, NULL, NULL, DPNCONNECT_SYNC);

	g_ConnectAttempt.Clear();

	switch (hRes)
	{
	case DPN_OK:
#ifdef ALLOW_LATE_JOINS
		if (info.gameStarted)
		{
			g_bNeedSnapshot = true;
			CNetMsg_RequestSnapshot msg;

			DbgPrint("Requesting snapshot");

			g_pNetMan->m_numStartedSynch = g_pNetMan->NumPlayers(); // 8/8/10 //g_pDarkNet->GetSessionManager()->GetApplicationDesc()->dwCurrentPlayers - 1;
			DbgPrint("Other players: %d", g_pNetMan->m_numStartedSynch);

			cNetManagerFns::_LoopFunc((void*)0x006C9420, kMsgBeginFrame, NULL);

			SendToHost((void*)&msg, sizeof(CNetMsg_RequestSnapshot), DPNSEND_GUARANTEED);
		}
#endif
		break; // Connection successful
	case DPNERR_INVALIDPASSWORD:
		ConPrintF("Incorrect password.");
		CancelEnumeration();
		break;
	case DPNERR_SESSIONFULL:
		ConPrintF("The server is full.");
		CancelEnumeration();
		break;
	case DPNERR_HOSTREJECTEDCONNECTION:
		ConPrintF("Cannot join server: the host has already started the mission.");
		CancelEnumeration();
		break;
	default:
		ConPrintF("Failed to connect to server. (%x)", DPlayErrorToString(hRes)); // Unknown error
		CancelEnumeration();
	}
}

bool CDarkPeer::GetSendQueueStats(QueueStats& stats)
{
	stats.peak = m_queuePeak;
	stats.queued = m_DarkQueue.GetSize();

	return true;
}

void CDarkPeer::GetSendQueues()
{
	DPNID enumeratedPlayers[8];
	DWORD maxDpnids = 8;
	DWORD numBytes, numMsgs;

	ZeroMemory(&enumeratedPlayers, sizeof(DPNID) * 8);

	HRESULT hRes = m_pDP->EnumPlayersAndGroups(enumeratedPlayers, &maxDpnids, DPNENUM_PLAYERS);
	if (SUCCEEDED(hRes))
	{
		for (int i = 0; i < (int)maxDpnids; i++)
		{
			if (enumeratedPlayers[i] != *cNetManager::gm_PlayerDPID)
			{
				hRes = m_pDP->GetSendQueueInfo(enumeratedPlayers[i], &numMsgs, &numBytes, NULL);
				if (FAILED(hRes))
					Log.Print("Failed to get send queue info for %x!", enumeratedPlayers[i]);
				else
				{
					Log.Print("Queue for %x: %d msgs, %d bytes.", enumeratedPlayers[i], numMsgs, numBytes);
				}
			}
		}
	}
}

//==============================================================================
// GetHostConnectionInfo()
//
// Returns DPN_CONNECTION_INFO for the host of the game.
//==============================================================================
HRESULT CDarkPeer::GetHostConnectionInfo(DPN_CONNECTION_INFO* pConnectInfo)
{
	return GetConnectionInfo(m_dpnidHost, pConnectInfo);
}

//==============================================================================
// GetConnectionInfo()
//
// Returns the DPN_CONNECTION_INFO for a specific DPNID.
//==============================================================================
HRESULT CDarkPeer::GetConnectionInfo(DPNID dpnidPeer, DPN_CONNECTION_INFO* pConnectInfo)
{
	pConnectInfo->dwSize = sizeof(DPN_CONNECTION_INFO);
	return m_pDP->GetConnectionInfo(dpnidPeer, pConnectInfo, NULL);
}

//==============================================================================
// GetPeer()
//
// Returns internal DirectPlay8 object. Should really get rid of this...
//==============================================================================
IDirectPlay8Peer* CDarkPeer::GetPeer()
{
	return m_pDP;
}

//==============================================================================
// GetLocalID()
//
// Returns local DPNID.
//==============================================================================
DPNID CDarkPeer::GetLocalID()
{
	return m_dpnidLocal;
}

//==============================================================================
// InitClient()
//
// Initializes the DarkPeer instance as a client.
//==============================================================================
void CDarkPeer::InitClient()
{
	try
	{
		HRESULT hRes;
		DPN_SP_CAPS caps;

		g_Net = STATE_Client;

		// Create DirectPlay client
		hRes = CoCreateInstance(CLSID_DirectPlay8Peer, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Peer, 
			(LPVOID*) &m_pDP);
		// Initialize address objects
		hRes = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_ALL, IID_IDirectPlay8Address, 
			(LPVOID*) &m_pDeviceAddress);
		hRes = CoCreateInstance(CLSID_DirectPlay8Address, NULL, CLSCTX_ALL, IID_IDirectPlay8Address, 
			(LPVOID*) &m_pHostAddress);

		if (FAILED(hRes)) 
			throw DPlayException("COM initialization failed", hRes);

		m_pSessionMgr = new CDarkSessionManager(m_pDP);

		g_pNetMan->PreFlush();
		
		// Initialize DirectPlay peer
		hRes = m_pDP->Initialize(this, StaticMessageHandler, 0);
		if (FAILED(hRes)) throw DPlayException("Could not initialize DirectPlay", hRes);

		// Set the service providers
		hRes = m_pHostAddress->SetSP(&CLSID_DP8SP_TCPIP);
		hRes = m_pDeviceAddress->SetSP(&CLSID_DP8SP_TCPIP);
		if (FAILED(hRes)) throw DPlayException("SetSP() failed", hRes);

		caps.dwSize = sizeof(DPN_SP_CAPS);
		hRes = m_pDP->GetSPCaps(&CLSID_DP8SP_TCPIP, &caps, NULL);
		if (FAILED(hRes)) throw DPlayException("GetSP() failed", hRes);

		caps.dwSystemBufferSize = Cfg.GetInt("BufferSize");
		hRes = m_pDP->SetSPCaps(&CLSID_DP8SP_TCPIP, &caps, NULL);
		if (FAILED(hRes)) throw DPlayException("SetSPCaps() failed", hRes);

	}

	catch (DPlayException error)
	{
		// Release DirectPlay objects

		SAFE_RELEASE(m_pDeviceAddress);
		SAFE_RELEASE(m_pHostAddress);
		SAFE_RELEASE(m_pDP);

		throw error;
	}
}

//==============================================================================
// InitGlobalLink()
//
// Initializes connection to global server.
//==============================================================================
bool CDarkPeer::InitGlobalLink()
{
	try
	{
		m_gsc = GS_CreateDP8Client();
		if (m_gsc)
		{
			IGSClient::InitInfo info;
			info.pfnReceive = GS_OnReceive;
			info.pfnConnect = GS_OnConnectComplete;
			info.pfnConnLost = GS_OnConnectionLost;
			info.pfnHeartbeat = GS_SendHeartbeat;
			info.heartbeatInterval = kHeartbeatInterval;
			info.autoReconnect = true;
			info.appGUID = GAME_GUID;

			m_gsc->Init(info);
		}
		else
			ConPrintF("Failed to initialize global server connection");
	}

	catch (DPlayException error)
	{
		Log.Print("Failed to initialize global server connection: %s (%x).", error.GetMessage(), error.GetCode());
		return 0;
	}

	bool success = m_gsc->Connect(Cfg.GetString("HostName"), Cfg.GetInt("GlobalPort"), NULL);
	if (!success)
		ConPrintF("Failed to connect to global server. (%s: %d)", __FILE__, __LINE__);

	return success;
}

//==============================================================================
// InitServer()
//
//  Initializes the DarkPeer instance as a server.
//==============================================================================
void CDarkPeer::InitServer()
{
	try
	{
		HRESULT hRes;	
		DPN_APPLICATION_DESC dpnAppDesc;
		DPN_SP_CAPS caps;

		g_Net = STATE_Host;

		hRes = (CoCreateInstance(CLSID_DirectPlay8Peer, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Peer, 
			(LPVOID*) &m_pDP));
		hRes = CoCreateInstance(CLSID_DirectPlay8Address, NULL, 
										CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address, (LPVOID*) &m_pDeviceAddress);
		
		if (FAILED(hRes)) throw DPlayException("COM initialization failed", hRes);

		m_pSessionMgr = new CDarkSessionManager(m_pDP);

		g_pNetMan->PreFlush();

		ZeroMemory(&dpnAppDesc, sizeof(DPN_APPLICATION_DESC));
		dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
		dpnAppDesc.dwMaxPlayers = Cfg.GetInt("MaximumPlayers");
		dpnAppDesc.dwFlags = DPNSESSION_NODPNSVR;
		dpnAppDesc.guidApplication = GAME_GUID;

		wchar_t sessionName[kMaxGameName * 2];
		AnsiToWide(sessionName, Cfg.GetString("ServerName"), kMaxGameName);

		dpnAppDesc.pwszSessionName = sessionName;

		if (strlen(Cfg.GetString("Password")))
		{
			wchar_t buff[512];
			AnsiToWide(buff, Cfg.GetString("Password"), sizeof(buff));

			dpnAppDesc.pwszPassword = buff;
			dpnAppDesc.dwFlags |= DPNSESSION_REQUIREPASSWORD;
		}

		// Initialize DirectPlay
		hRes = m_pDP->Initialize(this, StaticMessageHandler, 0);
		if (FAILED(hRes)) throw DPlayException("Could not initialize DirectPlay", hRes);

		// Set service provider and port
		hRes = m_pDeviceAddress->SetSP(&CLSID_DP8SP_TCPIP);
		if (FAILED(hRes)) throw DPlayException("SetSP() failed", hRes);

		caps.dwSize = sizeof(DPN_SP_CAPS);
		hRes = m_pDP->GetSPCaps(&CLSID_DP8SP_TCPIP, &caps, NULL);
		if (FAILED(hRes)) throw DPlayException("GetSPCaps() failed", hRes);

		caps.dwSystemBufferSize = Cfg.GetInt("BufferSize");
		hRes = m_pDP->SetSPCaps(&CLSID_DP8SP_TCPIP, &caps, NULL);
		if (FAILED(hRes)) throw DPlayException("SetSPCaps() failed", hRes);

		int port = Cfg.GetInt("ServerPort");
		hRes = m_pDeviceAddress->AddComponent(DPNA_KEY_PORT, &port, sizeof(port), DPNA_DATATYPE_DWORD);
		if (FAILED(hRes)) throw DPlayException("AddComponent() failed", hRes);

		hRes = SetPlayerInfo();
		if (FAILED(hRes)) throw DPlayException("SetPeerInfo() failed", hRes);

		hRes = m_pDP->Host(&dpnAppDesc, &m_pDeviceAddress, 1, NULL, NULL, this, NULL);
		if (FAILED(hRes)) throw DPlayException("Host() failed", hRes);

		m_pSessionMgr->SetApplicationDesc(&dpnAppDesc);
		m_pSessionMgr->UpdateSessionInfo();
	}

	catch (DPlayException error)
	{
		// Release DirectPlay objects

		SAFE_RELEASE(m_pDeviceAddress);
		SAFE_RELEASE(m_pDP);

		throw error;
	}
}

HANDLE CDarkPeer::GetConnectEventHandle()
{
	return m_ConnectedEvent;
}

//==============================================================================
// GetPeerInfo()
//
// Retrieves peer info. Caller must free returned pointer.
//==============================================================================
DPN_PLAYER_INFO* CDarkPeer::GetPeerInfo(DPNID dpnidPlayer)
{
	DWORD dwSize = 0;
	DPN_PLAYER_INFO* pPeerInfo = NULL;
	HRESULT hRes = DPNERR_CONNECTING;

	while (hRes == DPNERR_CONNECTING) 
		hRes = m_pDP->GetPeerInfo(dpnidPlayer, pPeerInfo, &dwSize, 0);

	if (hRes == DPNERR_BUFFERTOOSMALL)
	{
		pPeerInfo = (DPN_PLAYER_INFO*) new BYTE[dwSize];
		ZeroMemory(pPeerInfo, dwSize);
		pPeerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
	
		hRes = m_pDP->GetPeerInfo(dpnidPlayer, pPeerInfo, &dwSize, 0);
		if (FAILED(hRes))
			return NULL;

		return pPeerInfo;
	}

	return NULL;
}

//======================================================================================
// HandleCreatePlayerMsg()
//
// Handles create player system message from DPlay.
//======================================================================================
void CDarkPeer::HandleCreatePlayerMsg(DPNMSG_CREATE_PLAYER* pCreatePlayer)
{
	csReceive.Lock(); // Block packet receives while we're handling this

	// Translate DPlay8's create player message to DPlay4
	char playerName[kMaxPlayerName] = "";
	DPMSG_CREATEPLAYERORGROUP* pDP4CreatePlayer = new DPMSG_CREATEPLAYERORGROUP;

	// Retrieve the peer info and set the host and local DPNIDs
	DPN_PLAYER_INFO* pdpPlayerInfo = GetPeerInfo(pCreatePlayer->dpnidPlayer);

	//Log.Print("Connection from DPNID: %08x", pCreatePlayer->dpnidPlayer);

	if (pdpPlayerInfo)
	{
		if (pdpPlayerInfo->dwPlayerFlags == DPNPLAYER_HOST)
		{
			m_dpnidHost = pCreatePlayer->dpnidPlayer;

			if (Debug.IsFlagSet(DEBUG_NET))
				ConPrintF("Set host DPNID to %x.", m_dpnidHost);
		}
		else if (pdpPlayerInfo->dwPlayerFlags == DPNPLAYER_LOCAL)
			m_dpnidLocal = pCreatePlayer->dpnidPlayer;

		char playerName[kMaxPlayerName];
		WideToAnsi(playerName, pdpPlayerInfo->pwszName, kMaxPlayerName);

		// Display the connection message if this isn't the local player
		if (!(pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_LOCAL))
		{
			ConPrintF("%s connected.", playerName);
			MessageMgr::Get()->AddLineFormat(false, "%s connected.", playerName);
		}

		SAFE_DELETE_ARRAY(pdpPlayerInfo);
	}
	else
		ConPrintF("Failed to get peer info for DPNID %x.", pCreatePlayer->dpnidPlayer);

	if (g_pNetMan->AmDefaultHost() && !IsStagingMode())
	{
		if (Client.MissionStarted())
		{
			IPtr<IQuestData> pQuest = _AppGetAggregated(IID_IQuestData);

			CNetMsg_StartMission msg;
			msg.difficulty = pQuest->Get("Difficulty");
			msg.missionID = _GetMissionData()->missNumber;

			Send(pCreatePlayer->dpnidPlayer, (void*)&msg, sizeof(CNetMsg_StartMission), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
		}
	}

	if (g_Net == STATE_Client && m_bReceivedExistingPlayers || g_Net == STATE_Host)
	{
		if (pCreatePlayer->pvPlayerContext == this)
		{
			if (Debug.IsFlagSet(DEBUG_NET))
				ConPrintF("Creating local host. (%x)", pCreatePlayer->dpnidPlayer);
			m_dpnidLocal = (pCreatePlayer->dpnidPlayer);

			csReceive.Unlock();
			return;// DPN_OK;
		}

		// send Thief the new player notification in the form it expects from DPlay 4
		ZeroMemory(pDP4CreatePlayer, sizeof(DPMSG_CREATEPLAYERORGROUP));

		pDP4CreatePlayer->dwType = DPSYS_CREATEPLAYERORGROUP;
		pDP4CreatePlayer->dwPlayerType = DPPLAYERTYPE_PLAYER;
		pDP4CreatePlayer->dpId = pCreatePlayer->dpnidPlayer;

		NetMessage* pMsg = new NetMessage;
		pMsg->m_pMsgData = (BYTE*)pDP4CreatePlayer;
		pMsg->m_size = sizeof(DPMSG_CREATEPLAYERORGROUP);
		pMsg->m_dpnidSender = DPID_SYSMSG;

		m_DarkQueue.Push(pMsg);

#ifdef _THIEFBUILD
		_SFX_Play_Raw(1, 0, "pickloot.wav");
#endif

	}			

	csReceive.Unlock();
}

//======================================================================================
// StaticMessageHandler() 
//
// Passes call to MessageHandler()
//======================================================================================
HRESULT WINAPI CDarkPeer::StaticMessageHandler(void* pContext, DWORD messageType, void* pMessage)
{
   CDarkPeer* pThisObject = (CDarkPeer*)pContext;

   return pThisObject->MessageHandler(pContext, messageType, pMessage);
}

//======================================================================================
// MessageHandler()
//
// Handles messages from DirectPlay.
//======================================================================================
HRESULT CDarkPeer::MessageHandler(void* pContext, DWORD messageType, void* pMessage)
{
	switch (messageType)
	{
	case DPN_MSGID_CREATE_PLAYER:
		{
			HandleCreatePlayerMsg((DPNMSG_CREATE_PLAYER*)pMessage);
			break;
		}

      case DPN_MSGID_DESTROY_PLAYER:
		{
			// Translate DPlay8's destroy player message to DPlay4
			PDPNMSG_DESTROY_PLAYER pDestroyPlayer = (PDPNMSG_DESTROY_PLAYER) pMessage;
			DPNID dpnidPlayer = pDestroyPlayer->dpnidPlayer;
			DPMSG_DESTROYPLAYERORGROUP* pDP4DestroyPlayer = new DPMSG_DESTROYPLAYERORGROUP;

			if (dpnidPlayer != m_dpnidLocal && g_pNetMan)
			{
#if (GAME == GAME_THIEF)
#if 1
				DPN_PLAYER_INFO* pPeerInfo = GetPeerInfo(pDestroyPlayer->dpnidPlayer);
				if (pPeerInfo)
				{
					char playerName[kMaxPlayerName];
					WideToAnsi(playerName, pPeerInfo->pwszName, sizeof(playerName));
					const char* str = "%s has left the game.";
					MessageMgr::Get()->AddLineFormat(true, str, playerName);
					ConPrintF(str, playerName);

					SAFE_DELETE_ARRAY(pPeerInfo);
				}
#else
				const char* playerName = Players.NameFromDPNID(pDestroyPlayer->dpnidPlayer);
				const char* msg = "%s has left the game.";
				MessageMgr::Get()->AddLineFormat(true, msg, playerName);
				ConPrintF(msg, playerName);
#endif
#endif
				ZeroMemory(pDP4DestroyPlayer, sizeof(DPMSG_DESTROYPLAYERORGROUP));
				pDP4DestroyPlayer->lpLocalData = m_DP4PlayerData[dpnidPlayer];
				pDP4DestroyPlayer->dpId = dpnidPlayer;
				pDP4DestroyPlayer->dwType = DPSYS_DESTROYPLAYERORGROUP;
				pDP4DestroyPlayer->dwPlayerType = DPPLAYERTYPE_PLAYER;

				// send Thief the destroy player notification in the form it expects from DPlay 4
				NetMessage* pMsg = new NetMessage;
				pMsg->m_pMsgData = (BYTE*)pDP4DestroyPlayer;
				pMsg->m_size = sizeof(DPMSG_DESTROYPLAYERORGROUP);
				pMsg->m_dpnidSender = DPID_SYSMSG;

				m_DarkQueue.Push(pMsg);
				// find the player's index and destroy the player data
			}
			break;
		}

		case DPN_MSGID_INDICATE_CONNECT:
			{
				if (g_Net == STATE_Host)
				{
#ifndef _RELEASE
					if (Client.MissionStarted() && !Cfg.GetBool("AllowJoinsInProgress"))
#else
					if (Client.MissionStarted())
#endif
						return DPNERR_HOSTREJECTEDCONNECTION;
				}
				break;
			}

		case DPN_MSGID_CONNECT_COMPLETE:
			{
				PDPNMSG_CONNECT_COMPLETE pConnComplete = (PDPNMSG_CONNECT_COMPLETE)pMessage;

				if (pConnComplete->hResultCode == DPN_OK)
				{
#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
					m_dpnidLocal = (pConnComplete->dpnidLocal);

					ConPrintF("Connected to session.");
					_SFX_Play_Raw(1, 0, "pickloot.wav");
#endif

					SetEvent(m_ConnectedEvent);
				}
				break;
			}

		case DPN_MSGID_ENUM_HOSTS_RESPONSE:
			{
				m_csEnumResponse.Lock();

				DPNMSG_ENUM_HOSTS_RESPONSE* pResponse = (DPNMSG_ENUM_HOSTS_RESPONSE*)pMessage;

				OnEnumerationResponse(pResponse);

				m_csEnumResponse.Unlock();
				break;
			}

      case DPN_MSGID_RECEIVE:
			{
				PDPNMSG_RECEIVE pRecvData = (PDPNMSG_RECEIVE)pMessage;
				Packet* packet = (Packet*)pRecvData->pReceiveData;

				csReceive.Lock();

				// this may not be a good idea... some original packets are very close to these numbers
				if (packet->type > NewPacketStart && packet->type < NewPacketEnd)
				{
					HandleNewPacket(pRecvData->pReceiveData, pRecvData->dwReceiveDataSize, pRecvData->dpnidSender);
					csReceive.Unlock();
					return DPN_OK;
				}

				NetMessage* pMsg = new NetMessage;

				pMsg->m_pMsgData = pRecvData->pReceiveData;
				pMsg->m_size = pRecvData->dwReceiveDataSize;
				pMsg->m_bufferHandle = pRecvData->hBufferHandle; // Save buffer handle so we can return it to DirectPlay with ReturnBuffer() later
				pMsg->m_dpnidSender = pRecvData->dpnidSender;

				m_DarkQueue.Push(pMsg);

				csReceive.Unlock();

				return DPNSUCCESS_PENDING;
			}

		case DPN_MSGID_TERMINATE_SESSION:
			{
				// Translate DPlay8's terminate session message to DPlay4
				PDPNMSG_TERMINATE_SESSION pTerminateSession = (PDPNMSG_TERMINATE_SESSION)pMessage;
				DPMSG_SESSIONLOST* pDP4SessLost = new DPMSG_SESSIONLOST;
				NetMessage* pMsg = new NetMessage;

				ZeroMemory(pDP4SessLost, sizeof(DPMSG_SESSIONLOST));
				pDP4SessLost->dwType = DPSYS_SESSIONLOST;

				pMsg->m_pMsgData = (BYTE*)pDP4SessLost;
				pMsg->m_size = sizeof(DPMSG_SESSIONLOST);
				pMsg->m_dpnidSender = DPID_SYSMSG;

				m_DarkQueue.Push(pMsg);

#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
				if (pTerminateSession->pvTerminateData)
				{
					SDestroyReason* pMsg = (SDestroyReason*)pTerminateSession->pvTerminateData;

					switch (pMsg->reason)
					{
						case DReason_CrcFailed:
							Client.StartEndGameTimer(EndGameTime, ER_KickedCrc); break;
						default:
						case DReason_Kicked:
							Client.StartEndGameTimer(EndGameTime, ER_Kicked); break;
					}
				}
				else
					Client.StartEndGameTimer(EndGameTime, ER_HostLeft);
#endif
				break;
			}
	}

	return DPN_OK;
}

//==============================================================================
// HandleNewPacket()
//
// Sends new packets to the client packet handlers.
//==============================================================================
void CDarkPeer::HandleNewPacket(BYTE* pData, DWORD size, DPNID dpnidFrom)
{
#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
	NetMessage* pMessage = new NetMessage;

	BYTE* pData2 = new BYTE[size];
	memcpy(pData2, pData, size);

	pMessage->m_dpnidSender = dpnidFrom;
	pMessage->m_pMsgData = pData2;
	pMessage->m_size = size;

	NewReceiveQueue.Push(pMessage);
#endif
}

//==============================================================================
// SendToHost()
//
// Sends a packet to the host of the game.
//==============================================================================
void CDarkPeer::SendToHost(void* data, DWORD dataSize, DWORD flags)
{
	if (!g_pNetMan->AmDefaultHost())
		Send(m_dpnidHost, data, dataSize, NULL, flags);
	else
		Send(*cNetManager::gm_PlayerDPID, data, dataSize, NULL, flags);
}

//==============================================================================
// SendToAll()
//
// Sends a packet to all players.
//==============================================================================
void CDarkPeer::SendToAll(void* pData, DWORD dwDataSize, DWORD dwTimeout, DWORD dwFlags)
{
	Send(ALL_PLAYERS, pData, dwDataSize, dwTimeout, dwFlags);
}

//==============================================================================
// Send()
//
// Sends a packet to any player.
//==============================================================================
void CDarkPeer::Send(DPNID idTo, void* pData, DWORD dwDataSize, DWORD dwTimeout, DWORD dwFlags)
{
	if (g_pNetMan->IsNetworkGame())
	{
		HRESULT hRes;
		DPN_BUFFER_DESC bufferDesc;
		DPNHANDLE hAsync;
		Packet* packet = (Packet*)pData;

		bufferDesc.dwBufferSize = dwDataSize;
		bufferDesc.pBufferData = (BYTE*)pData;

		if (idTo == kInvalidHostID)
			Log.Print("ERROR: Attempted to send packet type %d to an invalid DPNID.", packet->type);

		if (dwFlags & DPNSEND_SYNC)
		{
			hRes = m_pDP->SendTo(idTo, &bufferDesc, 1, dwTimeout, NULL, NULL, dwFlags);
			if (hRes != DPN_OK)
				Log.Print("%s(%d) DPlay error: %s. Attempted a synchronous send of packet type %d to DPNID %x.", __FILE__, __LINE__, DPlayErrorToString(hRes), packet->type, idTo);
		}
		else
		{
			hRes = m_pDP->SendTo(idTo, &bufferDesc, 1, dwTimeout, NULL, &hAsync, dwFlags);
			if (hRes != DPNSUCCESS_PENDING)
				Log.Print("%s(%d) DPlay error: %s. Attempted async send of packet type %d to ID %x.", __FILE__, __LINE__, DPlayErrorToString(hRes), packet->type, idTo);
		}
	}
}

//==============================================================================
// SendOP()
//
// Translates old DirectPlay 4 packet flags to DirectPlay 8 flags and sends the packet.
//==============================================================================
void CDarkPeer::SendOP(DPNID idTo, void* lpData, DWORD dwDataSize, DWORD dwTimeout, DWORD dwFlags)
{
	DPN_BUFFER_DESC bufferDesc;
	DPNHANDLE hAsync;
	HRESULT hRes;

	bufferDesc.dwBufferSize = dwDataSize;
	bufferDesc.pBufferData = (BYTE*)lpData;

	 // convert DirectPlay4's flags to DirectPlay8's
	switch (dwFlags)
	{
		case DPSEND_GUARANTEED:
			dwFlags = DPNSEND_GUARANTEED;
		case DPSEND_ASYNC | DPSEND_GUARANTEED:
			dwFlags = DPNSEND_GUARANTEED; 
			break;
		case NULL:
		case DPSEND_ASYNC:
			dwFlags = NULL;
			break;
		default:
			Log.Print("Unknown flags on old packet (%x)", dwFlags); break;
	}

	if (dwFlags != DPNSEND_GUARANTEED)
		dwFlags |= DPNSEND_NOCOMPLETE;

	dwFlags |= DPNSEND_NOLOOPBACK;
	dwFlags |= DPNSEND_NONSEQUENTIAL;
	dwFlags |= DPNSEND_COALESCE;

	//GetSendQueues();

	hRes = m_pDP->SendTo(idTo, &bufferDesc, 1, dwTimeout, 0, &hAsync, dwFlags);
	if (hRes != S_OK && hRes != DPNSUCCESS_PENDING
		&& hRes != DPNERR_INVALIDPLAYER) // 7/16/10 - don't need to treat this as an error, player could have left
	{
		if (hRes == DPNERR_NOCONNECTION)
			Shutdown();
		else
			ConPrintF("%s(%d) DPlay error: %s. Failed to send old packet to %x.", __FILE__, __LINE__, DPlayErrorToString(hRes), idTo);
	}
}

//==============================================================================
// SendChat()
//
// Sends a chat message to all players.
//==============================================================================
void CDarkPeer::SendChat(bool system, const char* chatMessage)
{
	CNetMsg_Chat msg;
	msg.type = PKT_Chat;

	if (!system)
		msg.idSpeaker = (BYTE)g_pNetMan->MyPlayerNum();
	else
		msg.idSpeaker = 0; // system message

	if (strlen(chatMessage) > kMaxMessageLength)
		return;

	strcpy(msg.chatText, chatMessage);
	DWORD size = (DWORD)sizeof(msg.type) + sizeof(msg.idSpeaker) + (strlen(msg.chatText) + 1) * sizeof(char);

	if (g_Net == STATE_Client)
		Send(m_dpnidHost, (void*)&msg, size, 0, DPNSEND_GUARANTEED);
	else if (g_Net == STATE_Host)
		Send(ALL_PLAYERS, (void*)&msg, size, 0, DPNSEND_GUARANTEED);
}

//==============================================================================
// HandleGameModeEntry()
//
// Called upon game mode entry. Notifies player of any global server connection issues, and forces
// a heartbeat update.
//==============================================================================
void CDarkPeer::HandleGameModeEntry()
{
#if (GAME == GAME_THIEF)
	if (Cfg.GetBool("ListOnGlobal") && m_gsc)
	{
		if (m_gsc->GetState() == IGSClient::CS_Connected)
		{
			// Send a heartbeat now to update the game state flags
			GS_SendHeartbeat(m_gsc);
		}
		//else
		//{
		//	MessageMgr::Get()->AddLine(true, "Failed to contact global server. Your server has not been listed.");
		//	GS_DestroyClient(m_gsc);
		//}
	}

	if (g_pNetMan->AmDefaultHost())
	{
		CNetMsg_CrcRequest msg;
		if (g_gamesysName[0])
		{
			msg.scanFileID = ScanFile_Gamesys;
			Send(ALL_PLAYERS, &msg, sizeof(CNetMsg_CrcRequest), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
		}

		msg.scanFileID = ScanFile_Mission;
		Send(ALL_PLAYERS, &msg, sizeof(CNetMsg_CrcRequest), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);

		msg.scanFileID = ScanFile_MotionDB;
		Send(ALL_PLAYERS, &msg, sizeof(CNetMsg_CrcRequest), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
	}
#endif
}

//==============================================================================
// HandleMissionLoaded()
//
// 
//==============================================================================
void CDarkPeer::HandleMissionLoaded()
{
	//m_bMissionStarted = true;

	g_pDarkNet->GetSessionManager()->UpdateSessionInfo();
}

//==============================================================================
// Update()
//
// Sends server information heartbeats to the global server at a rate defined by kHeartbeatInterval.
//==============================================================================
void CDarkPeer::Update()
{
	if (m_gsc && g_Net == STATE_Host)
		m_gsc->Update();
}

//==============================================================================
// EventHandler()
//
// 
//==============================================================================
void CDarkPeer::EventHandler(int eventType, void* eventData)
{
	switch (eventType)
	{
	case ET_GameMode:
		return HandleGameModeEntry();
	case ET_MissionLoaded:
		return HandleMissionLoaded();
	}
}

//==============================================================================
// SetPlayerInfo()
//
// 
//==============================================================================
HRESULT CDarkPeer::SetPlayerInfo()
{
	HRESULT hRes;

#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
	wchar_t playerNameW[kMaxPlayerName*2];
	AnsiToWide(playerNameW, Cfg.GetString("PlayerName"), kMaxPlayerName);

   DPN_PLAYER_INFO dpPlayerInfo;
   ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
   dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
   dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
	dpPlayerInfo.pwszName = playerNameW;

	hRes = m_pDP->SetPeerInfo(&dpPlayerInfo, NULL, NULL, DPNOP_SYNC);
	if (FAILED(hRes)) return hRes;
#else if (GAME == GAME_SHOCK)
	wchar_t playerNameW[kMaxPlayerName*2];
	AnsiToWide(playerNameW, g_pNetMan->m_playerName, kMaxPlayerName);

   DPN_PLAYER_INFO dpPlayerInfo;
   ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
   dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
   dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
	dpPlayerInfo.pwszName = playerNameW;

	hRes = m_pDP->SetPeerInfo(&dpPlayerInfo, NULL, NULL, DPNOP_SYNC);

	return hRes;
#endif

	return DPN_OK;
}

//==============================================================================
// Shutdown()
//
// Frees objects and closes DPlay session.
//==============================================================================
void CDarkPeer::Shutdown()
{
	// release and delete objects
	Close();

	if (g_Net == STATE_Host)
	{
		SAFE_RELEASE(m_pDeviceAddress);
		SAFE_RELEASE(m_pDP);

		if (m_gsc)
		{
			//m_gsc->SendQuitting(IGSClient::ExitReason_Quit);
			//m_gsc->SendShutdownNotification();
			GS_DestroyClient(m_gsc);
		}
	}

	else if (g_Net == STATE_Client)
	{
		SAFE_RELEASE(m_pDeviceAddress);
		SAFE_RELEASE(m_pHostAddress);
		SAFE_RELEASE(m_pDP);

		CloseHandle(m_ConnectedEvent);
	}

	//SAFE_DELETE(g_pDarkNet);

	g_Net = STATE_None;
}

CDarkSessionManager* CDarkPeer::GetSessionManager()
{
	return m_pSessionMgr;
}

const SessionInfo & CDarkPeer::GetSessionInfo() const
{
	return m_pSessionMgr->GetSessionInfo();
}

OldPlayerData& CDarkPeer::GetOldPlayerData()
{
	return m_DP4PlayerData;
}

bool CDarkPeer::ReceivedPlayers() const
{
	return m_bReceivedExistingPlayers;
}

void CDarkPeer::SetReceivedPlayers(bool bReceived)
{
	m_bReceivedExistingPlayers = bReceived;
}

NetMessage* CDarkPeer::PopQueuedMessage()
{
	uint size = m_DarkQueue.GetSize();

	if (size > 0)
	{
		if (size > m_queuePeak)
			m_queuePeak = size;

		return m_DarkQueue.Pop();
	}
	else
		return NULL;
}

void CDarkPeer::SendHeartbeat()
{
	GS_SendHeartbeat(m_gsc);
}

void ConnectAttempt::Start(uint port, const char* ip, const wchar_t* password)
{
	if (!ip)
		return;

	m_port = port;
	//m_ip = "192.168.1.104";
	m_ip = ip;
	if (password) m_password = password;

	m_active = true;
	g_pNetMan->Join(NULL, NULL, m_ip.c_str());
}

void ConnectAttempt::Clear()
{
	m_active = false;
	m_password.clear();
	m_ip.clear();
}

const wchar_t* ConnectAttempt::GetPassword() const
{
	if (!m_password.empty())
		return m_password.c_str();
	else
		return NULL;
}

uint ConnectAttempt::GetPort() const
{
	return m_port;
}

//==============================================================================
// GetPlayers()
//
// Get player object IDs.
//==============================================================================
void GetPlayers(int players[kMaxPlayers])
{
	int playerObj;
	sPropertyObjIter iter;

	players[0] = *_gPlayerObj;

	g_pNetMan->NetPlayerIterStart(&iter);
	for (int i = 1; i < kMaxPlayers; i++)
	{
		if (g_pNetMan->NetPlayerIterNext(&iter, &playerObj))
			players[i] = playerObj;
		else
			players[i] = 0;
	}
}