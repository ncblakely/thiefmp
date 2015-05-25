#pragma once

#include "Engine\inc\NetMessage.h"
#include "Engine\inc\EventManager.h"
#include "Engine\inc\Lists.h"
#include "GlobalServer.h"

#include "DarkNet.h"

#ifdef _THIEFBUILD
#include "MessageManager.h"
#include "Packets.h"
#endif

class CDarkPeer : public IDarkNet, IEventListener
{
public:
	CDarkPeer(NetState state);
	virtual ~CDarkPeer();

	virtual bool ConnectToServer(const char* serverIP, const char* password);
	virtual void CrashCleanup();
	virtual void	CancelEnumeration();
	virtual void CancelAllEnumerations();
	virtual void Close();
	virtual bool DestroyPeer(DPNID dpnidPeer, const void* pDestroyData, DWORD dataSize);
	virtual void	EnumHosts(const char* serverIP, const char* password);
	virtual HANDLE GetConnectEventHandle();
	virtual bool GetSendQueueStats(QueueStats& stats);
	virtual void	GetSendQueues();
	virtual HRESULT GetHostConnectionInfo(DPN_CONNECTION_INFO* pConnectInfo);
	virtual HRESULT GetConnectionInfo(DPNID dpnidPeer, DPN_CONNECTION_INFO* pConnectInfo);
	virtual DPN_PLAYER_INFO* GetPeerInfo(DPNID dpnidPlayer);
	virtual IDirectPlay8Peer* GetPeer();
	virtual const SessionInfo& GetSessionInfo() const;
	virtual DPNID GetLocalID();
	virtual CDarkSessionManager* GetSessionManager();
	virtual void SendChat(bool system, const char* chatMessage);
	virtual void	Send(DPNID idTo, void* pData, DWORD dwDataSize, DWORD dwTimeout, DWORD dwFlags);
	virtual void	SendOP(DPNID idTo, void* lpData, DWORD dwDataSize, DWORD dwTimeout, DWORD dwFlags);
	virtual void SendToAll(void* pData, DWORD dwDataSize, DWORD dwTimeout, DWORD dwFlags);
	virtual void SendToHost(void* data, DWORD dataSize, DWORD flags);
	virtual HRESULT SetPlayerInfo();
	virtual void	Shutdown();
	virtual void EventHandler(int eventType, void* eventData);
	virtual void Update();
	virtual void SendHeartbeat();
	virtual OldPlayerData& GetOldPlayerData();
	virtual bool ReceivedPlayers() const;
	virtual void SetReceivedPlayers(bool bReceived);
	virtual struct NetMessage* PopQueuedMessage();

protected:
	void	OnEnumerationResponse(PDPNMSG_ENUM_HOSTS_RESPONSE);
	HRESULT	MessageHandler(void* pContext, DWORD messageType, void* pMessage);
	static HRESULT WINAPI StaticMessageHandler(void* pContext, DWORD messageType, void* pMessage);
	void	 HandleNewPacket(BYTE* pData, DWORD size, DPNID dpnidFrom);
	void HandleCreatePlayerMsg(DPNMSG_CREATE_PLAYER* msg);

	void InitClient();
	void	 InitServer();
	bool InitGlobalLink();

	// Event handlers
	void HandleGameModeEntry();
	void HandleMissionLoaded();

	OldPlayerData m_DP4PlayerData;
	bool m_bReceivedExistingPlayers;
	DPNID m_dpnidLocal;
	DPNID m_dpnidHost;
	HANDLE m_ConnectedEvent;
	IDirectPlay8Peer*	 m_pDP;
   IDirectPlay8Address* m_pDeviceAddress;
   IDirectPlay8Address* m_pHostAddress;
	IGSClient* m_gsc;
	CCriticalSection	 m_csEnumResponse;
	DPNHANDLE m_enumSessionAsyncOp;
	uint m_queuePeak;
	CDarkSessionManager* m_pSessionMgr;
	SimpleQueue<NetMessage> m_DarkQueue;

	static bool								m_bLoadedNetCommands;
	static bool								m_bLoadedBinds;
};

extern SimpleQueue<NetMessage> NewReceiveQueue;