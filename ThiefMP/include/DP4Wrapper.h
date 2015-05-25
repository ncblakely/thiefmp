#pragma once

class CDPWrapper : IDirectPlay4A
{
public:
	CDPWrapper() 
	{ 
		m_refCount = 1;
	}

	// IUnknown
	HRESULT  __stdcall QueryInterface(REFIID riid, void** ppvObj);
	ULONG    __stdcall AddRef(void);
	ULONG    __stdcall Release(void);

	// IDirectPlay4
	HRESULT __stdcall AddGroupToGroup(DPID idParentGroup,  DPID idGroup);
	HRESULT __stdcall AddPlayerToGroup(DPID idGroup,  DPID idPlayer);
	HRESULT __stdcall CancelMessage(DWORD dwMsgID, DWORD dwFlags);
	HRESULT __stdcall CancelPriority(DWORD dwMinPriority, DWORD dwMaxPriority, DWORD dwFlags);
	HRESULT __stdcall Close();
	HRESULT __stdcall CreateGroup(LPDPID lpidGroup,  LPDPNAME lpGroupName,  LPVOID lpData,  DWORD dwDataSize,  DWORD dwFlags);
	HRESULT __stdcall CreateGroupInGroup(DPID idParentGroup, LPDPID lpidGroup, LPDPNAME lpGroupName,  LPVOID lpData, DWORD dwDataSize, DWORD dwFlags);
	HRESULT __stdcall CreatePlayer(LPDPID lpidPlayer, LPDPNAME lpPlayerName, HANDLE hEvent, LPVOID lpData, DWORD dwDataSize, DWORD dwFlags);
	HRESULT __stdcall DeleteGroupFromGroup(DPID idParentGroup,  DPID idGroup);
	HRESULT __stdcall DeletePlayerFromGroup(DPID idGroup,  DPID idPlayer);
	HRESULT __stdcall DestroyGroup(DPID idGroup);
	HRESULT __stdcall DestroyPlayer(DPID idPlayer);
	HRESULT __stdcall EnumConnections(LPCGUID lpguidApplication, LPDPENUMCONNECTIONSCALLBACK lpEnumCallback, LPVOID lpContext, DWORD dwFlags);
	HRESULT __stdcall EnumGroupPlayers(DPID idGroup, LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2, LPVOID lpContext, DWORD dwFlags);
	HRESULT __stdcall EnumGroups(LPGUID lpguidInstance,  LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2,  LPVOID lpContext,  DWORD dwFlags);
	HRESULT __stdcall EnumGroupsInGroup(DPID idGroup,  LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumCallback, LPVOID lpContext, DWORD dwFlags);
	HRESULT __stdcall EnumPlayers(LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2, LPVOID lpContext, DWORD dwFlags);
	HRESULT __stdcall EnumSessions(LPDPSESSIONDESC2 lpsd, DWORD dwTimeout, LPDPENUMSESSIONSCALLBACK2 lpEnumSessionsCallback2, LPVOID lpContext, DWORD dwFlags);
	HRESULT __stdcall GetCaps(LPDPCAPS lpDPCaps, DWORD dwFlags);
	HRESULT __stdcall GetGroupConnectionSettings(DWORD dwFlags,  DPID idGroup,  LPVOID lpData,  LPDWORD lpdwDataSize);
	HRESULT __stdcall GetGroupData(DPID idGroup,  LPVOID lpData,  LPDWORD lpdwDataSize,  DWORD dwFlags);
	HRESULT __stdcall GetGroupFlags(DPID idGroup,  LPDWORD lpdwFlags);
	HRESULT __stdcall GetGroupName(DPID idGroup, LPVOID lpData, LPDWORD lpdwDataSize);
	HRESULT __stdcall GetGroupOwner(DPID idGroup, LPDPID lpidOwner);
	HRESULT __stdcall GetGroupParent(DPID idGroup, LPDPID lpidParent);
	HRESULT __stdcall GetMessageCount(DPID idPlayer, LPDWORD lpdwCount);
	HRESULT __stdcall GetMessageQueue(DPID idFrom, DPID idTo, DWORD dwFlags, LPDWORD lpdwNumMsgs, LPDWORD lpdwNumBytes);
	HRESULT __stdcall GetPlayerAccount(DPID idPlayer, DWORD dwFlags, LPVOID lpData, LPDWORD lpdwDataSize);
	HRESULT __stdcall GetPlayerAddress(DPID idPlayer, LPVOID lpData, LPDWORD lpdwDataSize);
	HRESULT __stdcall GetPlayerCaps(DPID idPlayer, LPDPCAPS lpPlayerCaps,  DWORD dwFlags);
	HRESULT __stdcall GetPlayerData(DPID idPlayer, LPVOID lpData,  LPDWORD lpdwDataSize,  DWORD dwFlags );
	HRESULT __stdcall GetPlayerFlags(DPID idPlayer, LPDWORD lpdwFlags);
	HRESULT __stdcall GetPlayerName(DPID idPlayer, LPVOID lpData, LPDWORD lpdwDataSize);
	HRESULT __stdcall GetSessionDesc(LPVOID lpData, LPDWORD lpdwDataSize);
	HRESULT __stdcall Initialize(LPGUID lpGUID);
	HRESULT __stdcall InitializeConnection(LPVOID lpConnection, DWORD dwFlags);
	HRESULT __stdcall Open(LPDPSESSIONDESC2 lpsd, DWORD dwFlags);
	HRESULT __stdcall Receive(LPDPID lpidFrom, LPDPID lpidTo, DWORD dwFlags, LPVOID lpData, LPDWORD lpdwDataSize);
	HRESULT __stdcall SecureOpen(LPCDPSESSIONDESC2 lpsd, DWORD dwFlags, LPCDPSECURITYDESC lpSecurity, LPCDPCREDENTIALS lpCredentials);
	HRESULT __stdcall Send(DPID idFrom, DPID idTo, DWORD dwFlags, LPVOID lpData, DWORD dwDataSize);
	HRESULT __stdcall SendChatMessage(DPID idFrom, DPID idTo, DWORD dwFlags, LPDPCHAT lpChatMessage);
	HRESULT __stdcall SendEx(DPID idFrom, DPID idTo, DWORD dwFlags, LPVOID lpData, DWORD dwDataSize, DWORD dwPriority, DWORD dwTimeout, LPVOID lpContext, LPDWORD lpdwMsgID);
	HRESULT __stdcall SetGroupConnectionSettings(DWORD dwFlags, DPID idGroup, LPDPLCONNECTION lpConnection);
	HRESULT __stdcall SetGroupData(DPID idGroup, LPVOID lpData, DWORD dwDataSize, DWORD dwFlags);
	HRESULT __stdcall SetGroupName(DPID idGroup, LPDPNAME lpGroupName, DWORD dwFlags);
	HRESULT __stdcall SetGroupOwner(DPID idGroup, DPID idOwner);
	HRESULT __stdcall SetPlayerData(DPID idPlayer, LPVOID lpData, DWORD dwDataSize, DWORD dwFlags);
	HRESULT __stdcall SetPlayerName(DPID idPlayer, LPDPNAME lpPlayerName, DWORD dwFlags);
	HRESULT __stdcall SetSessionDesc(LPDPSESSIONDESC2 lpSessDesc, DWORD dwFlags);
	HRESULT __stdcall StartSession(DWORD dwFlags, DPID idGroup);

protected:
	int m_refCount;
};

extern CDPWrapper* g_pDPWrapper;