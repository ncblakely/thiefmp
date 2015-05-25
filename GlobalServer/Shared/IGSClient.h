#pragma once

#define PROTOCOL_VER_1_0 1000
#define PROTOCOL_VER_1_1 1100

#define PROTOCOL_VER PROTOCOL_VER_1_1 // Current protocol version

// Forward declarations
interface IGSBrowser;

// Flags passed on connection
#define GSConnect_ListenCreate 0x1 // Client should be informed of server creations
#define GSConnect_ListenRemove 0x2 // Client should be informed of server removals

// Defines a client connection to the global server.
interface IGSClient
{
	enum ConnState
	{
		CS_NotConnected = 0,
		CS_Connecting,
		CS_Connected,
		CS_Failed,
		CS_FailedFull,
		CS_FailedRejected,
	};

	enum ConnectResult
	{
		CR_OK,
		CR_FailedFull,
		CR_FailedRejected,
		CR_FailedUnknown,
	};

	enum ExitReason
	{
		ExitReason_None, // Not exiting
		ExitReason_JoinedGame, // Client was viewing server list and joined a game
		ExitReason_AppCrash, // Client application has crashed
		ExitReason_Quit, // Client quit normally
	};

	// Event callbacks
	typedef void (*GSReceiveCB)(IGSClient* client, BYTE* data, DWORD dataSize); // Called when data is received from the global server
	typedef void (*GSConnectCB)(IGSClient* client, IGSClient::ConnectResult cr);  // Called when the connection to the global server is established
	typedef void (*GSConnectionLostCB)(IGSClient* client); // Called when the connection to the global server is lost
	typedef void (*GSHeartbeatCB)(IGSClient* client); // Called when it's time to send a heartbeat

	struct InitInfo
	{
		InitInfo()
		{
			// Set up defaults
			pfnReceive = NULL;
			pfnConnect = NULL;
			pfnConnLost = NULL;
			pfnHeartbeat = NULL;
			ZeroMemory((void*)&appGUID, sizeof(appGUID));
			heartbeatInterval = 0;
			autoReconnect = true;
			noQueuedReceives = false;
		}

		GSReceiveCB pfnReceive;
		GSConnectCB pfnConnect;
		GSConnectionLostCB pfnConnLost;
		GSHeartbeatCB pfnHeartbeat;
		GUID appGUID; // DirectPlay GUID of game
		DWORD heartbeatInterval; // Time to wait between heartbeats (in ms)
		bool autoReconnect; // Reconnect if connection is lost
		bool noQueuedReceives; // Receive packets immediately instead of queueing (should almost never be used)
	};

	virtual ~IGSClient() { }

	/* Connects to the specified host and port. Flags are a combination of GSConnect_ flags. */
	virtual bool Connect(const char* hostName, DWORD port, DWORD flags) = 0;

	/* Initializes the client. Must be called before Connect() */
	virtual bool Init(const InitInfo& info) = 0;

	/* Pings a game server on the specified host/port. */
	virtual bool PingServer(const char* serverIP, unsigned short port, int numTries = kDefaultNumPingTries) = 0;

	/* Returns server browser associated with this client. */
	virtual IGSBrowser* GetBrowser() = 0;

	/* Sets server browser associated with this client. */
	virtual void SetBrowser(IGSBrowser* pBrowser) = 0;

	/* Sends data to the global server. */
	virtual bool Send(void* data, DWORD dataSize, bool guaranteed) = 0;

	/* Sends an exit reason to the global server. Should be called before the client is destroyed. */
	virtual void SendQuitting(ExitReason er) = 0;

	/* Notifies the global server that the server this client is responsible for updating has closed.
	* This should only be called if the server has closed but the connection to the global
	* is remaining active for some reason -- the server will be removed from the list automatically when the
	* client disconnects. */
	virtual void SendShutdownNotification() = 0;

	/* Gets/sets connection state */
	virtual ConnState GetState() = 0;
	virtual void SetState(ConnState state) = 0;

	/* Updates the connection. Checks to see whether heartbeats need to be sent and processes packets
	/* sent from the global. Should be called every frame. */
	virtual void Update() = 0;

protected:
	static const int kDefaultNumPingTries = 2;
};

// Login structures

struct GSClientLogin_V1 // Deprecated login packet for old ThiefMP builds
{
	unsigned short build;
	BYTE gameID;
	BYTE connectType;
};

struct GSClientLogin_V2 // Current login
{
	unsigned short protocolVer;
	GUID appGuid;
	DWORD flags; // combination of ConnectFlags
};

IGSClient* GS_CreateDP8Client();
void GS_DestroyClient(IGSClient*& client);