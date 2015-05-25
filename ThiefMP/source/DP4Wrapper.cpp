/*************************************************************
* File: DP4Wrapper.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements DirectPlay4 -> DirectPlay 8 wrapper class.
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "DP4Wrapper.h"
#include "Callbacks.h"
#include "Client.h"
#include "DarkPeer.h"

#if (GAME == GAME_THIEF)
#define RECEIVE_BUFF_ADDR 0x0077CDC0
#define DPLAY4_ADDR 0x00720AD8
#elif (GAME == GAME_SHOCK)
#define RECEIVE_BUFF_ADDR 0x007E14E0
#define DPLAY4_ADDR 0x007887A0
#elif (GAME == GAME_DROMED)
#define RECEIVE_BUFF_ADDR 0x010030C0
#define DPLAY4_ADDR 0x00AE9118
#endif

#define UNIMPLEMENTED_METHOD Log.Print("%s: method unimplemented.", __FUNCTION__); return E_FAIL;

extern CCriticalSection csReceive;

CDPWrapper* g_pDPWrapper = NULL;

ULONG __stdcall CDPWrapper::AddRef()
{
	// Should *not* be called normally
	assert(false && "unexpected call to AddRef()");

	m_refCount++;
	return m_refCount; //return 0;
}

ULONG __stdcall CDPWrapper::Release()
{
	if (--m_refCount == 0)
	{
		delete this;
		g_pDPWrapper = NULL;

		return 0;
	}

	return m_refCount;
}

HRESULT __stdcall CDPWrapper::QueryInterface(REFIID riid, void** ppvObj)
{
	// Should *not* be called normally
	assert(false && "unexpected call to QueryInterface()");
	return 0;
	//*ppvObj = NULL;

	//HRESULT hRes = m_pDP4->QueryInterface(riid, ppvObj); 

	//if (hRes == S_OK)
	//	*ppvObj = this;
	//
	//return hRes;
}

HRESULT __stdcall CDPWrapper::AddGroupToGroup(DPID idParentGroup,  DPID idGroup) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::AddPlayerToGroup(DPID idGroup,  DPID idPlayer) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::CancelMessage(DWORD dwMsgID, DWORD dwFlags) 
{
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::CancelPriority(DWORD dwMinPriority, DWORD dwMaxPriority, DWORD dwFlags) 
{ 
	UNIMPLEMENTED_METHOD
}

HRESULT __stdcall CDPWrapper::Close() 
{
	if (g_pDarkNet)
	{
		g_pDarkNet->Shutdown();
		delete g_pDarkNet;
	}

	return S_OK;
}

HRESULT __stdcall CDPWrapper::CreateGroup(LPDPID lpidGroup,  LPDPNAME lpGroupName,  LPVOID lpData,  DWORD dwDataSize,  DWORD dwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::CreateGroupInGroup(DPID idParentGroup, LPDPID lpidGroup, LPDPNAME lpGroupName, LPVOID lpData, DWORD dwDataSize, DWORD dwFlags) 
{
	UNIMPLEMENTED_METHOD;
}

// Called from 5EA584

HRESULT __stdcall CDPWrapper::CreatePlayer(LPDPID lpidPlayer, LPDPNAME lpPlayerName, HANDLE hEvent, LPVOID lpData, DWORD dwDataSize, DWORD dwFlags) 
{ 
	if (g_Net == STATE_Host)
	{
		if (Debug.IsFlagSet(DEBUG_NET))
			ConPrintF("Setting local host DPNID to %x.",  g_pDarkNet->GetLocalID());
		*lpidPlayer = g_pDarkNet->GetLocalID();
	}
	else
	{
		// Wait until we're connected, since the local DPNID is not known until then
		if (WaitForSingleObject(g_pDarkNet->GetConnectEventHandle(), 6000) == WAIT_TIMEOUT)
			return E_FAIL;

		*lpidPlayer =  g_pDarkNet->GetLocalID();

		if (Debug.IsFlagSet(DEBUG_NET))
			ConPrintF("Setting local client DPNID to %x.", *lpidPlayer);
	}

	return S_OK;
}

HRESULT __stdcall CDPWrapper::DeleteGroupFromGroup(DPID idParentGroup, DPID idGroup) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::DeletePlayerFromGroup(DPID idGroup,  DPID idPlayer) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::DestroyGroup(DPID idGroup) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::DestroyPlayer(DPID idPlayer) 
{
	return S_OK;
} 

HRESULT __stdcall CDPWrapper::EnumConnections(LPCGUID lpguidApplication, LPDPENUMCONNECTIONSCALLBACK lpEnumCallback, LPVOID lpContext, DWORD dwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::EnumGroupPlayers(DPID idGroup, LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2, LPVOID lpContext, DWORD dwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::EnumGroups(LPGUID lpguidInstance,  LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2,  LPVOID lpContext,  DWORD dwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::EnumGroupsInGroup(DPID idGroup, LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumCallback, LPVOID lpContext, DWORD dwFlags) 
{
	UNIMPLEMENTED_METHOD;
}

// Two callbacks - 5EA780 calls cNet::InitExistingPlayers, seems to be used for host
// 539140 calls cNetManager::EnumPlayersCallback, seems to be used for clients
HRESULT __stdcall CDPWrapper::EnumPlayers(LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2, LPVOID lpContext, DWORD dwFlags) 
{ 
	if (g_Net)
	{
		DPNID enumeratedPlayers[8];
		DWORD maxDpnids = 8;
		int contextID = 0;

		ZeroMemory(&enumeratedPlayers, sizeof(DPNID) * 8);

		HRESULT hRes = g_pDarkNet->GetPeer()->EnumPlayersAndGroups(enumeratedPlayers, &maxDpnids, DPNENUM_PLAYERS);

		if (lpEnumPlayersCallback2 == &HookNetManager::_DestroyNetPlayerCallback)
			contextID =0;
		else if (lpEnumPlayersCallback2 == &HookNetManager::EnumPlayersCallback)
			contextID = 1;
		else if (lpEnumPlayersCallback2 == &HookNetManager::InitExistingPlayer)
			contextID = 2;

		if (hRes == DPNERR_BUFFERTOOSMALL)
			Log.Print("ERROR: Failed to enumerate players: buffer too small!");
		else
		{
			for (int i = 0;  i < (int)maxDpnids; i++)
			{
				if (enumeratedPlayers[i])
				{
					switch (contextID)
					{
					case 0:
						if (Debug.IsFlagSet(DEBUG_NET))
							ConPrintF("Calling destroy net player callback for ID %x.", enumeratedPlayers[i]);
						cNetManager::_DestroyNetPlayerCallback(enumeratedPlayers[i], DPPLAYERTYPE_PLAYER, NULL, NULL, (void*)*cNetManager::gm_TheNetManager); break;
					case 1:
						if (Debug.IsFlagSet(DEBUG_NET))
							ConPrintF("Calling enum players callback for ID %x.", enumeratedPlayers[i]);
						cNetManager::EnumPlayersCallback(enumeratedPlayers[i], DPPLAYERTYPE_PLAYER, NULL, NULL, (void*)*cNetManager::gm_TheNetManager); break;
					case 2:
						if (Debug.IsFlagSet(DEBUG_NET))
							ConPrintF("Calling init existing player for ID %x.", enumeratedPlayers[i]);
						cNetManager::InitExistingPlayer(enumeratedPlayers[i], DPPLAYERTYPE_PLAYER, NULL, NULL, (void*)*cNetManager::gm_Net); break;
					}
				}
			}
		}
	}

	return S_OK;
}

HRESULT __stdcall CDPWrapper::EnumSessions(LPDPSESSIONDESC2 lpsd, DWORD dwTimeout, LPDPENUMSESSIONSCALLBACK2 lpEnumSessionsCallback2, LPVOID lpContext, DWORD dwFlags) 
{
	// Unnecessary under DirectPlay8
	return S_OK;
}

HRESULT __stdcall CDPWrapper::GetCaps(LPDPCAPS lpDPCaps, DWORD dwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetGroupConnectionSettings(DWORD dwFlags,  DPID idGroup,  LPVOID lpData,  LPDWORD lpdwDataSize) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetGroupData(DPID idGroup, LPVOID lpData, LPDWORD lpdwDataSize, DWORD dwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetGroupFlags(DPID idGroup,  LPDWORD lpdwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetGroupName(DPID idGroup, LPVOID lpData, LPDWORD lpdwDataSize) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetGroupOwner(DPID idGroup, LPDPID lpidOwner) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetGroupParent(DPID idGroup, LPDPID lpidParent) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetMessageCount(DPID idPlayer, LPDWORD lpdwCount) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetMessageQueue(DPID idFrom, DPID idTo, DWORD dwFlags, LPDWORD lpdwNumMsgs, LPDWORD lpdwNumBytes) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetPlayerAccount(DPID idPlayer, DWORD dwFlags, LPVOID lpData, LPDWORD lpdwDataSize) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetPlayerAddress(DPID idPlayer, LPVOID lpData, LPDWORD lpdwDataSize) 
{ 
	return S_OK;
}

HRESULT __stdcall CDPWrapper::GetPlayerCaps(DPID idPlayer, LPDPCAPS lpPlayerCaps,  DWORD dwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetPlayerData(DPID idPlayer, LPVOID lpData, LPDWORD lpdwDataSize, DWORD dwFlags) 
{ 
	if (dwFlags == DPGET_LOCAL)
	{
		OldPlayerData& playerData = g_pDarkNet->GetOldPlayerData();

		if (playerData[idPlayer])
		{
			int playerObject = playerData[idPlayer]->playerObject;
			DWORD P1 = playerData[idPlayer]->P1;

			//ConPrintF("Data exists. Player object: %d.", playerObject);
			__asm
			{
				mov eax, [lpData]
				mov ecx, [P1]
				mov [eax], ecx
				mov ecx, [playerObject]
				mov [eax+4], ecx
			}
		}
		else
		{
			if (idPlayer < 0)
				Log.Print("ERROR: Attempted to get player data for invalid DPNID.");

			__asm
			{
				xor eax, eax
				mov ecx, [lpData]
				mov [ecx], eax
				mov [ecx+4], eax
			}	
		}
	}
	else
		Log.Print("Attempted to get remote player data!");

	return S_OK;
}

HRESULT __stdcall CDPWrapper::GetPlayerFlags(DPID idPlayer, LPDWORD lpdwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetPlayerName(DPID idPlayer, LPVOID lpData, LPDWORD lpdwDataSize) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::GetSessionDesc(LPVOID lpData, LPDWORD lpdwDataSize) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::Initialize(LPGUID lpGUID) 
{ 
	UNIMPLEMENTED_METHOD;
}

// Called from 005EA178
HRESULT __stdcall CDPWrapper::InitializeConnection(LPVOID lpConnection, DWORD dwFlags) 
{ 
	return S_OK;
}

HRESULT __stdcall CDPWrapper::Open(LPDPSESSIONDESC2 lpsd, DWORD dwFlags) 
{ 
	if (dwFlags == DPOPEN_CREATE)
	{
		if (g_pDarkNet)
		{
			dbgassert(false && "multiple initializations in Open()");
			return E_FAIL;
		}

		try
		{
			g_pDarkNet = new CDarkPeer(STATE_Host);
		}

		catch (DPlayException error)
		{
			Log.Print("Host failed: %s (%x).", error.GetMessage(), error.GetCode());
			return E_FAIL;
		}
	}

	return S_OK;
}

// Called from 005EA713
HRESULT __stdcall CDPWrapper::Receive(LPDPID lpidFrom, LPDPID lpidTo, DWORD dwFlags, LPVOID lpData, LPDWORD lpdwDataSize) 
{ 
	csReceive.Lock();

#ifdef _THIEFBUILD
	Client.m_PacketHandler.OnPollNetwork(); // Handle new packets first
#endif

	if (!g_pDarkNet)
	{
		csReceive.Unlock();
		return DPERR_NOMESSAGES;
	}

	// Emulate DirectPlay 4's message queue
	NetMessage* pMsg = g_pDarkNet->PopQueuedMessage();
	if (pMsg)
	{
		if (!g_pDarkNet->ReceivedPlayers() && pMsg->m_dpnidSender != DPID_SYSMSG)
		{
			g_pDarkNet->SetReceivedPlayers(true);
			if (Debug.IsFlagSet(DEBUG_NET))
				ConPrintF("Finished receiving existing players.");
		}
		// Set the sender and receiver DPNIDs, and the size of the data
		*lpidFrom = pMsg->m_dpnidSender;
		*lpidTo = *cNetManager::gm_PlayerDPID;
		*lpdwDataSize = pMsg->m_size;

		// Copy the data into the game's receive buffer
		memcpy((void*)RECEIVE_BUFF_ADDR, pMsg->m_pMsgData, pMsg->m_size);	

		BYTE* pRcv = (BYTE*)RECEIVE_BUFF_ADDR;

		// If we have a buffer handle from DPlay, we need to return ownership of the data so it can be freed
		if (pMsg->m_bufferHandle != -1)
		{
			IDirectPlay8Peer* peer = g_pDarkNet->GetPeer();
			if (peer)
				peer->ReturnBuffer(pMsg->m_bufferHandle, 0);
		}
		else
			pMsg->FreeData();

		// Check to see if the session has been terminated
		LPDPMSG_GENERIC pRecvData = (LPDPMSG_GENERIC)RECEIVE_BUFF_ADDR;
		if (pRecvData->dwType == DPSYS_SESSIONLOST)
		{
			//g_pDarkNet->Shutdown();
			g_pNetMan->Leave(); // 7/25/10 - changed to Leave, g_pDarkNet was not being freed
		}

		delete pMsg;

		csReceive.Unlock();
		return S_OK;
	}
	else		// no messages
	{
		csReceive.Unlock();
		return DPERR_NOMESSAGES;
	}
}

HRESULT __stdcall CDPWrapper::SecureOpen(LPCDPSESSIONDESC2 lpsd, DWORD dwFlags, LPCDPSECURITYDESC lpSecurity, LPCDPCREDENTIALS lpCredentials) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::Send(DPID idFrom, DPID idTo, DWORD dwFlags, LPVOID lpData, DWORD dwDataSize)
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::SendChatMessage(DPID idFrom, DPID idTo, DWORD dwFlags, LPDPCHAT lpChatMessage) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::SendEx(DPID idFrom, DPID idTo, DWORD dwFlags, LPVOID lpData, DWORD dwDataSize, DWORD dwPriority, DWORD dwTimeout, LPVOID lpContext, LPDWORD lpdwMsgID) 
{
	if (g_pDarkNet)
		g_pDarkNet->SendOP(idTo, lpData, dwDataSize, dwTimeout, dwFlags);

	return S_OK;
}

HRESULT __stdcall CDPWrapper::SetGroupConnectionSettings(DWORD dwFlags, DPID idGroup, LPDPLCONNECTION lpConnection) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::SetGroupData(DPID idGroup, LPVOID lpData, DWORD dwDataSize, DWORD dwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::SetGroupName(DPID idGroup, LPDPNAME lpGroupName, DWORD dwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::SetGroupOwner(DPID idGroup, DPID idOwner) 
{ 
	UNIMPLEMENTED_METHOD;
}

// Flags are DPSET_LOCAL
HRESULT __stdcall CDPWrapper::SetPlayerData(DPID idPlayer, LPVOID lpData, DWORD dwDataSize, DWORD dwFlags) 
{
	DP4Data* oldData = (DP4Data*)lpData;
	DP4Data* newData = new DP4Data;

	newData->playerObject = oldData->playerObject;
	newData->P1 = oldData->P1;
	g_pDarkNet->GetOldPlayerData()[idPlayer] = newData;

	if (Debug.IsFlagSet(DEBUG_NET))
		ConPrintF("DirectPlay: setting player data for DPID %x. Player obj: %d. P1: %x.", idPlayer, g_pDarkNet->GetOldPlayerData()[idPlayer]->playerObject, g_pDarkNet->GetOldPlayerData()[idPlayer]->P1);

	return S_OK;
}

HRESULT __stdcall CDPWrapper::SetPlayerName(DPID idPlayer, LPDPNAME lpPlayerName, DWORD dwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::SetSessionDesc(LPDPSESSIONDESC2 lpSessDesc, DWORD dwFlags) 
{ 
	UNIMPLEMENTED_METHOD;
}

HRESULT __stdcall CDPWrapper::StartSession(DWORD dwFlags, DPID idGroup) 
{ 
	UNIMPLEMENTED_METHOD;
}

//======================================================================================
// Purpose: Intercepts calls to CoCreateInstance. Returns a CDPWrapper object (instead of an IDirectPlay4 object)
// that forwards old DirectPlay4 calls to DirectPlay8.
//======================================================================================
HRESULT __stdcall FakeCoCreateInstance(REFCLSID rclsid,LPUNKNOWN pUnkOuter,DWORD dwClsContext,
								   REFIID riid, void** ppv)
{
	HRESULT hr = S_OK;

	if (ppv == (void**)DPLAY4_ADDR) // cNet::gDP
	{
		g_pDPWrapper = new CDPWrapper();
		*ppv = (IDirectPlay4A*)g_pDPWrapper;  
	}
	else
	{
		// 1/9/10: no longer instantiating DirectPlay4 at all
		hr =  CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
	}

	if (hr != S_OK)
		Log.Print("Error in CoCreateInstance.");

	return hr;
}

void WrapperTerm()
{
	if (g_pDPWrapper)
	{
		// Note: freeing the DirectPlay 4 wrapper will also free the Dark peer class.
		g_pDPWrapper->Close();
		//SAFE_RELEASE(g_pDPWrapper);
	}
}