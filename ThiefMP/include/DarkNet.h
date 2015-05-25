#pragma once

#define ALL_PLAYERS DPNID_ALL_PLAYERS_GROUP

const int kInvalidHostID = -1;
const int kMaxPlayers = 8;
const int kMaxGameName = 32;
const int kMaxPassword = 16;
//extern CCriticalSection csReceive;

#define EndGameTime 6.5f // time before game ends (after lost connection/kicked)

enum NetState
{
	STATE_None = 0,
	STATE_Client = 1,
	STATE_Host = 2,
};

struct QueueStats
{
	uint peak;
	uint queued;
};

#pragma pack(push, 1)

struct SessionInfo
{
	BYTE gameStarted;
	BYTE difficulty;
	BYTE nextMission;
	BYTE build;
	short respawnDelay;
	short startingLoot;
	short saveLimit;
	int timeLimit;
	bool respawnEnabled;
	bool shareLoot;
	bool playerTransparency;
	bool noLoadoutScreen;
	bool teamDamage;
	bool cheatsEnabled;
	bool increaseDifficulty;
};

#pragma pack(pop)

class ConnectAttempt
{
public:
	void Start(uint port, const char* ip, const wchar_t* password);
	void Clear();

	const wchar_t* GetPassword() const;
	uint GetPort() const;

	bool IsActive() { return m_active; }

protected:
	uint m_port;
	string m_ip;
	wstring m_password;
	bool m_active; // Connect in progress
};

typedef std::map<DPNID, DP4Data*> OldPlayerData;	// old player data that used to be stored in DirectPlay 4

class CDarkSessionManager
{
public:
	CDarkSessionManager(IDirectPlay8Peer* pDP);
	virtual ~CDarkSessionManager();

	const DPN_APPLICATION_DESC* GetApplicationDesc();
	void SetApplicationDesc(const DPN_APPLICATION_DESC* pAppDesc);
	void SetSessionInfo(const DPN_APPLICATION_DESC* desc);
	const SessionInfo& GetSessionInfo() const;
	const char* GetServerName() const;
	void UpdateSessionInfo();

protected:
	void DPGetApplicationDesc();

	IDirectPlay8Peer* m_pDP;
	IPtr<IQuestData> m_pQuestData;
	DPN_APPLICATION_DESC* m_pTempAppDesc;
	SessionInfo m_sessionDesc;
	NString m_serverName;
};

interface IDarkNet
{
	virtual ~IDarkNet() { }
	virtual bool ConnectToServer(const char* serverIP, const char* password) = 0;
	virtual void CrashCleanup() = 0;
	virtual void	CancelEnumeration() = 0;
	virtual void CancelAllEnumerations() = 0;
	virtual void Close() = 0;
	virtual bool DestroyPeer(DPNID dpnidPeer, const void* pDestroyData, DWORD dataSize) = 0;
	virtual void	EnumHosts(const char* serverIP, const char* password) = 0;
	virtual HANDLE GetConnectEventHandle() = 0;
	virtual bool GetSendQueueStats(QueueStats& stats) = 0;
	virtual void	GetSendQueues() = 0;
	virtual HRESULT GetHostConnectionInfo(DPN_CONNECTION_INFO* pConnectInfo) = 0;
	virtual HRESULT GetConnectionInfo(DPNID dpnidPeer, DPN_CONNECTION_INFO* pConnectInfo) = 0;
	virtual DPN_PLAYER_INFO* GetPeerInfo(DPNID dpnidPlayer) = 0;
	virtual IDirectPlay8Peer* GetPeer() = 0;
	virtual const SessionInfo& GetSessionInfo() const = 0;
	virtual DPNID GetLocalID() = 0;
	virtual CDarkSessionManager* GetSessionManager() = 0;
	virtual void SendChat(bool system, const char* chatMessage) = 0;
	virtual void	Send(DPNID idTo, void* pData, DWORD dwDataSize, DWORD dwTimeout, DWORD dwFlags) = 0;
	virtual void	SendOP(DPNID idTo, void* lpData, DWORD dwDataSize, DWORD dwTimeout, DWORD dwFlags) = 0;
	virtual void SendToAll(void* pData, DWORD dwDataSize, DWORD dwTimeout, DWORD dwFlags) = 0;
	virtual void SendToHost(void* data, DWORD dataSize, DWORD flags) = 0;
	virtual HRESULT SetPlayerInfo() = 0;
	virtual void	Shutdown() = 0;
	virtual void EventHandler(int eventType, void* eventData) = 0;
	virtual void Update() = 0;
	virtual void SendHeartbeat() = 0;
	virtual OldPlayerData& GetOldPlayerData() = 0;
	virtual bool ReceivedPlayers() const = 0;
	virtual void SetReceivedPlayers(bool bReceived) = 0;
	virtual struct NetMessage* PopQueuedMessage() = 0;
};

void GetPlayers(int players[kMaxPlayers]);

const GUID guidThief = 
{ 
	0x5ce0e9d6, 0xa4d7, 0x40f0, 
	{ 0x9f, 0x20, 0x50, 0x84, 0xa9, 0x40, 0x40, 0x2d } 
};

const GUID guidShock = 
{ 
	0xcde64b0d, 0xd0e0, 0x4107, 
	{ 0xac, 0xdc, 0xc0, 0x99, 0x9c, 0xd7, 0x59, 0x24 } 
};

extern IDarkNet* g_pDarkNet;
extern ConnectAttempt g_ConnectAttempt;
extern int g_Net;