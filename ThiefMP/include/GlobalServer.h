#pragma once

#include "Main.h"

#include "..\..\GlobalServer\Shared\IGSClient.h" // Main global server client interface
#include "..\..\GlobalServer\Shared\IGSBrowser.h" // Main global server client interface
#include "..\..\GlobalServer\Shared\Msg_Dark.h" // Dark engine specific packets
#include "..\..\GlobalServer\Shared\DPlayException.h"

// Forward declarations
class CServerBrowser;

using namespace EngCtrls;

class CServerListEntry
{
public:
	CServerListEntry(const char* serverIP, const char* serverName, const char* missionName, ushort build, ushort port, BYTE stateFlags, BYTE gameID);
	virtual ~CServerListEntry() { }

	char m_serverIP[16];
	char m_serverName[32];
	char m_missionName[32];
	ushort m_build;
	ushort m_port;
	BYTE m_stateFlags;
	BYTE m_gameID;

	// Data added by enumeration
	bool m_bNeedsEnumeration;
	bool m_bPinged;
	int m_ping;
	int m_currentPlayers;
	int m_maxPlayers;
	int m_listIndex;
};

class CServerList
{
	friend class CServerBrowser;
public:
	typedef std::list<CServerListEntry*> ServerList;

	CServerList() { }
	virtual ~CServerList() { }

	CServerListEntry* AddServer(const char* serverIP, const char* serverName, const char* missionName, ushort build, ushort port, BYTE stateFlags, BYTE gameID);
	CServerListEntry* FindServerFromIP(const char* serverIPToFind);
	CEListView* GetListView();
	ServerList& GetServers();
	void InitializeControls(HWND hDlg);
	void RemoveServer(CServerListEntry* pEntry);
	void UpdateListedServerData(CServerListEntry* pEntry);
	void DeleteAll();
	uint GetCount();
protected:
	CEListView						m_lvServers;

	
	ServerList m_Servers;
};

enum EServerListColumns
{
	SLC_Name = 0,
	SLC_Mission,
	SLC_Status,
	SLC_Players,
	SLC_Ping,
	SLC_Build,
	SLC_Game,
};

class CServerBrowser : public IGSBrowser
{
public:
	CServerBrowser();
	virtual ~CServerBrowser();

	virtual void OnStateChange(IGSClient::ConnState status);
	virtual void HandlePingResponse(void* msgData);
	virtual void HandleServerInfo(void* msgData, DWORD dataSize);
	virtual void HandleRemoveServer(void* msgData, DWORD dataSize);
	bool CreateBrowserWindow(int dlgResource);
	void Close();

	// Send handlers
	void SendListingRequest();

	void Update();

	INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

	void UpdateServerCount();

	// Receive handlers
	void ConnectToGlobal();
	void ConnectToSelectedServer();

	CServerListEntry* GetSelectedServer() { return m_pSelectedServer; }

protected:
	void	OnConnect();
	void OnInitDialog();
	void OnRefreshList();

	void OnSelectItem();
	void OnDoubleClickItem();

	void UpdatePings();
	HWND m_hBrowserWnd;
	CServerListEntry* m_pSelectedServer;
	IGSClient* m_gsc;
	CServerList m_ServerList;
	DWORD m_lastHeartbeatTime;
	CCriticalSection	 m_csConnect;
};

void GS_OnReceive(IGSClient* client, BYTE* data, DWORD dataSize);
void GS_OnConnectionLost(IGSClient* client);
void GS_OnConnectComplete(IGSClient* client, IGSClient::ConnectResult cr);
void GS_SendHeartbeat(IGSClient* client);

extern CServerBrowser* g_pBrowser;