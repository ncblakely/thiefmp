/*************************************************************
* File: ServerList.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "GlobalServer.h"

CServerListEntry::CServerListEntry(const char* serverIP, const char* serverName, const char* missionName, ushort build, ushort port, BYTE stateFlags, BYTE gameID)
{
	m_build = build;
	safe_strcpy(m_serverName, serverName, 32);
	safe_strcpy(m_serverIP, serverIP, 16);
	safe_strcpy(m_missionName, missionName, 32);

	m_bNeedsEnumeration = true;
	m_bPinged = false;
	m_ping = 0;
	m_currentPlayers = 0;
	m_maxPlayers = 0;
	m_listIndex = -1;
	m_port = port;
	m_stateFlags = stateFlags;
	m_gameID = gameID;
}

//CServerListEntry::~CServerListEntry
//{
//}

CServerListEntry* CServerList::AddServer(const char* serverIP, const char* serverName, const char* missionName, ushort build, ushort port, BYTE stateFlags, BYTE gameID)
{
	CServerListEntry* pEntry = new CServerListEntry(serverIP, serverName, missionName, build, port, stateFlags, gameID);

	if (Debug.IsFlagSet(DEBUG_GLOBAL))
		ConPrintF("Adding server: %s. (%s:%d - build %d).", serverName, serverIP, (int)port, build);

	m_Servers.push_back(pEntry);

	return pEntry;
}

CServerListEntry* CServerList::FindServerFromIP(const char* serverIPToFind)
{
	CServerListEntry* pEntry = NULL;
	for (ServerList::iterator it = m_Servers.begin(); it != m_Servers.end(); it++)
	{
		pEntry = *it;
		if (!_stricmp(pEntry->m_serverIP, serverIPToFind))
			return pEntry;
	}

	return NULL;
}

void CServerList::RemoveServer(CServerListEntry* pEntry)
{
	for (ServerList::iterator it = m_Servers.begin(); it != m_Servers.end(); it++)
	{
		if (*it == pEntry)
		{
			delete *it;
			m_Servers.erase(it);
			return;
		}
	}

	Log.Print("Attempted to remove an invalid server.");
}

CEListView* CServerList::GetListView()
{
	return &m_lvServers;
}

void CServerList::DeleteAll()
{
	for (ServerList::iterator it = m_Servers.begin(); it != m_Servers.end(); it++)
	{
		CServerListEntry* entry = *it;
		delete entry;
	}
	m_Servers.clear();
}

uint CServerList::GetCount()
{
	return (uint)m_Servers.size();
}

void CServerList::UpdateListedServerData(CServerListEntry* pEntry)
{
	char buff[128];
	int index = pEntry->m_listIndex;

	m_lvServers.SetItemText(index, SLC_Name, pEntry->m_serverName);
	m_lvServers.SetItemText(index, SLC_Mission, pEntry->m_missionName);

	if (pEntry->m_stateFlags & GAME_STATE_OPEN)
	{
		if (pEntry->m_stateFlags & GAME_STATE_PASSWORD)
			m_lvServers.SetItemText(index, SLC_Status, "Open - Password");
		else
			m_lvServers.SetItemText(index, SLC_Status, "Open");
	}
	else if (pEntry->m_stateFlags & GAME_STATE_CLOSED)
		m_lvServers.SetItemText(index, SLC_Status, "Closed - Playing");
	else
		m_lvServers.SetItemText(index, SLC_Status, "Unknown");

	if (!pEntry->m_bPinged)
	{
		m_lvServers.SetItemText(index, SLC_Players, "?/?");
		m_lvServers.SetItemText(index, SLC_Ping, "???");
	}
	else
	{
		sprintf(buff, "%d/%d", pEntry->m_currentPlayers, pEntry->m_maxPlayers);
		m_lvServers.SetItemText(index, SLC_Players, buff);

		sprintf(buff, "%d", pEntry->m_ping);
		m_lvServers.SetItemText(index, SLC_Ping, buff);
	}

	switch (pEntry->m_gameID)
	{
	case GAME_THIEF:
		m_lvServers.SetItemText(index, SLC_Game, "Thief 2"); break;
	case GAME_SHOCK:
		m_lvServers.SetItemText(index, SLC_Game, "Shock 2"); break;
	default:
		m_lvServers.SetItemText(index, SLC_Game , "???"); break;
	}

	sprintf(buff, "%d", pEntry->m_build);
	m_lvServers.SetItemText(index, SLC_Build, buff);
}

void CServerList::InitializeControls(HWND hDlg)
{
	// Initialize the server list view control
	m_lvServers.Init(hDlg, IDC_SERVERLIST);

	m_lvServers.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_lvServers.InsertColumn(SLC_Name, "Server Name");

	m_lvServers.InsertColumn(SLC_Mission, "Mission", LVCFMT_CENTER);
	m_lvServers.InsertColumn(SLC_Status, "Status", LVCFMT_CENTER);
	m_lvServers.InsertColumn(SLC_Players, "Players", LVCFMT_CENTER);
	m_lvServers.InsertColumn(SLC_Ping, "Ping", LVCFMT_CENTER);
	m_lvServers.InsertColumn(SLC_Build, "Version", LVCFMT_CENTER);
	//m_lvServers.InsertColumn(SLC_Game, "Game", LVCFMT_CENTER);

	RECT rect;
	GetWindowRect(m_lvServers.GetHandle(), &rect);

	int width = rect.right - rect.left;
	m_lvServers.SetColumnWidth(SLC_Name, (int)(width * 0.25f));
	m_lvServers.SetColumnWidth(SLC_Mission, (int)(width * 0.26f));
	m_lvServers.SetColumnWidth(SLC_Status, (int)(width * 0.17f));
	m_lvServers.SetColumnWidth(SLC_Ping, (int)(width * 0.07f));
	m_lvServers.SetColumnWidth(SLC_Players, (int)(width * 0.135f));
	m_lvServers.SetColumnWidth(SLC_Build, (int)(width * 0.112f));
	//m_lvServers.SetColumnWidth(SLC_Game, (int)(width * 0.088f));
}