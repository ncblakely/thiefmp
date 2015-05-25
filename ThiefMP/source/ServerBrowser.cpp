/*************************************************************
* File: ServerBrowser.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements server browser
*************************************************************/

#include "stdafx.h"

#include "DarkWinGui.h"
#include "GlobalServer.h"

CServerBrowser* g_pBrowser = NULL;

CServerBrowser::CServerBrowser() : m_csConnect("sb connect")
{
	m_hBrowserWnd = NULL;
	m_pSelectedServer = NULL;
	m_lastHeartbeatTime = NULL;
	m_gsc = NULL;

	// Set up global server connection
	try
	{
		m_gsc = GS_CreateDP8Client();
		if (m_gsc)
		{
			IGSClient::InitInfo info;
			info.pfnReceive = GS_OnReceive;
			info.pfnConnect = GS_OnConnectComplete;
			info.pfnConnLost = GS_OnConnectionLost;
			info.pfnHeartbeat = NULL;
			info.heartbeatInterval = 0;
			info.autoReconnect = true;
			info.appGUID = GAME_GUID;

			m_gsc->Init(info);
		}
		else
			ConPrintF("Server browser: failed to initialize global server connection.");
	}

	catch (DPlayException error)
	{
		Log.Print("Failed to initialize global server connection: %s (%x).", error.GetMessage(), error.GetCode());
	}
}

CServerBrowser::~CServerBrowser()
{
	Close();
}

bool CServerBrowser::CreateBrowserWindow(int dlgResource)
{
	CreateDialog(g_hInstDLL, MAKEINTRESOURCE(dlgResource), NULL, CServerBrowser::StaticDialogProc);
	if (!m_hBrowserWnd)
		return false;

	ShowWindow(m_hBrowserWnd, SW_SHOWNORMAL);

	if (m_gsc)
		m_gsc->SetBrowser(this);

	ConnectToGlobal();

	return true;
}

void CServerBrowser::Update()
{
	if (m_gsc)
	{
		m_gsc->Update();

		if (m_gsc->GetState() == IGSClient::CS_Connected)
			UpdatePings();
	}
}

void CServerBrowser::UpdatePings()
{
	for (CServerList::ServerList::iterator it = m_ServerList.GetServers().begin(); it != m_ServerList.GetServers().end(); it++)
	{
		CServerListEntry* pEntry = *it;

		if (pEntry->m_bNeedsEnumeration)
		{
			if (Debug.IsFlagSet(DEBUG_GLOBAL))
				ConPrintF("Pinging server %s on IP %s:%d...", pEntry->m_serverName, pEntry->m_serverIP, pEntry->m_port);

			if (!m_gsc->PingServer(pEntry->m_serverIP, pEntry->m_port))
				ConPrintF("Error: Failed to ping server.");

			pEntry->m_bNeedsEnumeration = false;
		}
	}
}

void CServerBrowser::OnStateChange(IGSClient::ConnState status)
{
	switch (status)
	{
	case IGSClient::CS_NotConnected:
		SetDlgItemText(m_hBrowserWnd, IDC_GLOBALSTATUS, "Disconnected.");
		EnableWindow(GetDlgItem(m_hBrowserWnd, IDREFRESHLIST), FALSE);
		break;
	case IGSClient::CS_Connecting:
		SetDlgItemText(m_hBrowserWnd, IDC_GLOBALSTATUS, "Connecting..."); 
		break;
	case IGSClient::CS_Connected:
		SetDlgItemText(m_hBrowserWnd, IDC_GLOBALSTATUS, "Connected to global server."); 
		EnableWindow(GetDlgItem(m_hBrowserWnd, IDREFRESHLIST), TRUE);
		OnRefreshList();
		break;
	case IGSClient::CS_Failed:
		SetDlgItemText(m_hBrowserWnd, IDC_GLOBALSTATUS, "Failed to connect to global server."); 
		EnableWindow(GetDlgItem(m_hBrowserWnd, IDREFRESHLIST), FALSE);
		break;
	case IGSClient::CS_FailedFull:
		SetDlgItemText(m_hBrowserWnd, IDC_GLOBALSTATUS, "Too many connections to global server. Please try again later."); 
		EnableWindow(GetDlgItem(m_hBrowserWnd, IDREFRESHLIST), FALSE);
		break;
	case IGSClient::CS_FailedRejected:
		SetDlgItemText(m_hBrowserWnd, IDC_GLOBALSTATUS, "The server rejected your connection. Try downloading an updated version."); 
		EnableWindow(GetDlgItem(m_hBrowserWnd, IDREFRESHLIST), FALSE);
		break;
	}
}

void CServerBrowser::HandlePingResponse(void* msgData)
{
	DPNMSG_ENUM_HOSTS_RESPONSE* pResponse = (DPNMSG_ENUM_HOSTS_RESPONSE*)msgData;

	wchar_t buf[1024] = L"";
	DWORD bufSize = sizeof(buf);
	DWORD componentDataType = DPNA_DATATYPE_STRING;
	HRESULT hr = pResponse->pAddressSender->GetComponentByName(DPNA_KEY_HOSTNAME, buf, &bufSize, &componentDataType);
	if (FAILED(hr))
	{
		ConPrintF("Failed to retrieve host name component for server.");
		return;
	}

	CServerListEntry* pEntry = m_ServerList.FindServerFromIP(NString(buf));
	if (!pEntry)
	{
		DbgPrint("Failed to find enumerated server in list.");
		return;
	}

	if (Debug.IsFlagSet(DEBUG_GLOBAL))
		ConPrintF("Received response from %s.", pEntry->m_serverName);

	const DPN_APPLICATION_DESC* pAppDesc = pResponse->pApplicationDescription;

	pEntry->m_currentPlayers = pAppDesc->dwCurrentPlayers;
	pEntry->m_maxPlayers = pAppDesc->dwMaxPlayers;
	pEntry->m_ping = pResponse->dwRoundTripLatencyMS;
	pEntry->m_bPinged = true;

	m_ServerList.UpdateListedServerData(pEntry);
}

void CServerBrowser::HandleServerInfo(void* msgData, DWORD dataSize)
{
	CDarkMarshalBuffer marshalBuff;
	Msg_DarkServerInfo msg;

	marshalBuff.SetBuffer((BYTE*)msgData, NULL);
	msg.Unmarshal(&marshalBuff);

	CServerListEntry* pEntry;
	int index;

	if ((pEntry = m_ServerList.FindServerFromIP(msg.serverIP)))
	{
		// Update from server info
		pEntry->m_build = msg.build;
		pEntry->m_port = msg.port;
		pEntry->m_stateFlags = msg.stateFlags;
		pEntry->m_gameID = msg.gameID;

		strcpy(pEntry->m_serverName, msg.serverName);
		strcpy(pEntry->m_serverIP, msg.serverIP);
		strcpy(pEntry->m_missionName, msg.mapName);

		m_ServerList.UpdateListedServerData(pEntry);
		UpdateServerCount();
	}
	else
	{
		pEntry = m_ServerList.AddServer(msg.serverIP, msg.serverName, msg.mapName, msg.build, msg.port, msg.stateFlags, msg.gameID);

		assert(pEntry);

		index = m_ServerList.m_lvServers.InsertLast("", (LPARAM)pEntry);
		if (index >= 0)
		{
			pEntry->m_listIndex = index;

			m_ServerList.UpdateListedServerData(pEntry);
			UpdateServerCount();
		}
		else
			ConPrintF("Failed to add server to list - invalid index.");
	}
}

void CServerBrowser::UpdateServerCount()
{
	NString str;

	str.Format("%d servers available.", m_ServerList.GetCount());
	SetDlgItemText(m_hBrowserWnd, IDC_NUMSERVERS, str.Str());
}

void CServerBrowser::HandleRemoveServer(void* msgData, DWORD dataSize)
{
	if (dataSize != sizeof(Msg_RemoveServer))
		return;

	CServerListEntry* pEntry;

	Msg_RemoveServer* msg = (Msg_RemoveServer*)msgData;
	if (msg->m_serverIP)
	{
		if ((pEntry = m_ServerList.FindServerFromIP(msg->m_serverIP)))
		{
			if (Debug.IsFlagSet(DEBUG_GLOBAL))
				ConPrintF("Removing server %s.", pEntry->m_serverName);

			int index = pEntry->m_listIndex;
			if (!m_ServerList.m_lvServers.DeleteItem(index))
			{
				if (Debug.IsFlagSet(DEBUG_GLOBAL))
					ConPrintF("Failed to remove server from list view.");
			}

			m_ServerList.RemoveServer(pEntry);
			UpdateServerCount();
		}
	}
	else
	{
		if (Debug.IsFlagSet(DEBUG_GLOBAL))
			ConPrintF("%s: failed to find server %s in list.", __FUNCTION__, msg->m_serverIP);
	}
}

void CServerBrowser::SendListingRequest()
{
	Msg_ListRequest msg;

	m_gsc->Send(&msg, sizeof(Msg_ListRequest), true);
}

INT_PTR CALLBACK CServerBrowser::StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (g_pBrowser)
		return g_pBrowser->DialogProc(hDlg, msg, wParam, lParam);

	return 0;
}

// For some reason the call to ConnectToServer() crashes in release mode with optimizations on here
//#pragma optimize("", off)

INT_PTR CALLBACK CServerBrowser::DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		m_hBrowserWnd = hDlg;
		OnInitDialog();
		break;

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDCLOSE: 
				// Tell the global server we're leaving
				//if (m_gsc)
					//m_gsc->SendQuitting(IGSClient::ExitReason_Quit);

				delete this;
				g_pBrowser = NULL;
				return TRUE;
			case IDREFRESHLIST:
				OnRefreshList();
				return TRUE;
			case IDCONNECT:
				ConnectToSelectedServer();
				return TRUE;
			}

			break;
		}
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_CLICK:
			OnSelectItem();
			break;
		case NM_DBLCLK:
			OnDoubleClickItem();
		}

		break;
	}

	return FALSE;
}

//#pragma optimize("", on)

void CServerBrowser::OnInitDialog()
{
	m_ServerList.InitializeControls(m_hBrowserWnd);
}

void CServerBrowser::Close()
{
	if (m_gsc)
	{
		//m_gsc->SendQuitting(IGSClient::ExitReason_Quit);
		GS_DestroyClient(m_gsc);
	}

	if (m_hBrowserWnd)
	{
		DestroyWindow(m_hBrowserWnd);
		m_hBrowserWnd = NULL;
	}
}

void CServerBrowser::OnSelectItem()
{
	if (g_pNetMan->IsNetworkGame())
		return;

	EnableWindow(GetDlgItem(m_hBrowserWnd, IDCONNECT), FALSE);
	CEListView* pView = m_ServerList.GetListView();

	int index = pView->GetNextItem(-1, LVNI_SELECTED);
	if (index == -1)
		return;

	CServerListEntry* pEntry = (CServerListEntry*)pView->GetItemData(index);
	if (pEntry > 0)
	{
		// Don't allow the connection if the game is closed or if the host is running a different version
		if ((pEntry->m_currentPlayers < pEntry->m_maxPlayers) && pEntry->m_stateFlags & GAME_STATE_OPEN && pEntry->m_build == TMP_BUILD)
		{
			EnableWindow(GetDlgItem(m_hBrowserWnd, IDCONNECT), TRUE);
			m_pSelectedServer = pEntry;
		}
	}
}

void CServerBrowser::OnDoubleClickItem()
{
	if (g_pNetMan->IsNetworkGame())
		return;

	CEListView* pView = m_ServerList.GetListView();

	int index = pView->GetNextItem(-1, LVNI_SELECTED);
	if (index == -1)
		return;

	CServerListEntry* pEntry = (CServerListEntry*)pView->GetItemData(index);
	if (pEntry > 0)
	{
		// Don't allow the connection if the game is closed or if the host is running a different version
		if (pEntry->m_stateFlags & GAME_STATE_OPEN && pEntry->m_build == TMP_BUILD)
				m_pSelectedServer = pEntry;
		else
			return;
	}

	ConnectToSelectedServer();
}

void CServerBrowser::ConnectToSelectedServer()
{
	m_csConnect.Lock(); // May not actually be necessary but we don't want this to get called from 2 different threads at the same time

	if (m_pSelectedServer)
	{
#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
		g_pNetMan->SetPlayerName(Cfg.GetString("PlayerName"));
#endif

		if (m_pSelectedServer->m_stateFlags & GAME_STATE_PASSWORD)
		{
			UI->CreateChildWindow(CW_PasswordDialog);
		}
		else
		{
			SwitchToThisWindow(g_pWinAppOps->m_GameWindow, TRUE);
			g_ConnectAttempt.Start(m_pSelectedServer->m_port, m_pSelectedServer->m_serverIP, NULL);
		}

		// Tell the server why we're leaving
		//m_gsc->SendQuitting(IGSClient::ExitReason_JoinedGame);

		m_csConnect.Unlock();

		delete this;
		g_pBrowser = NULL;
		return;
	}

	m_csConnect.Unlock();
}

void CServerBrowser::ConnectToGlobal()
{
	if (!m_gsc)
		return;

	assert(g_pBrowser);

	if (!m_gsc->Connect(Cfg.GetString("HostName"), Cfg.GetInt("GlobalPort"), GSConnect_ListenCreate | GSConnect_ListenRemove))
		ConPrintF("Failed to connect to global server. (%s: %d)", __FILE__, __LINE__);
}

void CServerBrowser::OnRefreshList()
{
	m_ServerList.DeleteAll();
	m_ServerList.m_lvServers.DeleteAllItems();

	// Reconnect if we lost connection for some reason
	if (m_gsc->GetState() != IGSClient::CS_Connected)
	{
		if (!m_gsc->Connect(Cfg.GetString("HostName"), Cfg.GetInt("GlobalPort"), GSConnect_ListenCreate | GSConnect_ListenRemove))
			ConPrintF("Failed to connect to global server. (%s: %d)", __FILE__, __LINE__);

		return;
	}

	UpdateServerCount();
	SendListingRequest();
}

void BrowserTerm()
{
	SAFE_DELETE(g_pBrowser);
}