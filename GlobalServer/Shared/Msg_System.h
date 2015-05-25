#pragma once

// Game state flags used for some heartbeats
#define GAME_STATE_OPEN (1<<0)
#define GAME_STATE_CLOSED (1<<1)
#define GAME_STATE_PASSWORD (1<<2)

class CMarshalBuffer;

// System message IDs (used for all games)
#define MsgID_SysBase 100

// Server -> client
#define MsgID_ListRequest (MsgID_SysBase + 1)
#define MsgID_RemoveServer (MsgID_SysBase + 2)
#define MsgID_ClientExit (MsgID_SysBase + 3)

// Client -> server
#define MsgID_ServerClosed (MsgID_SysBase + 50)

enum GLSPacketType_V1 // Message IDs for protocol 1.0
{
	GPKT_Heartbeat,
	GPKT_ListRequest,
	GPKT_ServerClosed,
	GPKT_ServerInfo,
	GPKT_RemoveServer,
};

enum GLSConnectionType
{
	CT_ListViewer, // This user has connected to view the server list
	CT_ServerLister,	// This user is only connected to list their server and should not receive any updates
};

struct GLSPacket_V1 
{
public:
	BYTE type;
};

// Client packets
struct MsgListRequest_V1 : public GLSPacket_V1
{
	MsgListRequest_V1() { type = (char)GPKT_ListRequest; }
};

class MsgHeartbeat_V1 : public GLSPacket_V1
{
public:
	MsgHeartbeat_V1() { type = (char)GPKT_Heartbeat; }

	ushort build;
	ushort port;
	BYTE gameID;
	BYTE stateFlags;
	char gameName[32];
	char missionName[32];

	void Marshal(CMarshalBuffer* pBuffer);
	void Unmarshal(CMarshalBuffer* pBuffer);
};

struct MsgServerClosed_V1 : public GLSPacket_V1
{
	MsgServerClosed_V1() { type = (char)GPKT_ServerClosed; }
};

// Server packets
class MsgServerInfo_V1: public GLSPacket_V1
{
public:
	MsgServerInfo_V1() { type = (char)GPKT_ServerInfo; }

	ushort build;
	ushort port;
	BYTE stateFlags;
	BYTE gameID;
	char serverIP[16];
	char serverName[32];
	char mapName[32];

	void Marshal(CMarshalBuffer* pBuffer);
	void Unmarshal(CMarshalBuffer* pBuffer);
};

struct MsgRemoveServer_V1: public GLSPacket_V1
{
	MsgRemoveServer_V1() { type = (char)GPKT_RemoveServer; }

	char m_serverIP[16];
};

struct GLSPacket_V2
{
public:
	WORD type;
};

// Client packets
struct Msg_ListRequest : public GLSPacket_V2
{
	Msg_ListRequest() { type = (WORD)MsgID_ListRequest; }
};

struct Msg_ServerClosed : public GLSPacket_V2
{
	Msg_ServerClosed() { type = (WORD)MsgID_ServerClosed; }
};

struct Msg_ClientExit : public GLSPacket_V2
{
	Msg_ClientExit() { type = (WORD)MsgID_ClientExit; }

	BYTE reason;
};

// Server packets

struct Msg_RemoveServer: public GLSPacket_V2
{
	Msg_RemoveServer() { type = (WORD)MsgID_RemoveServer; }

	char m_serverIP[16];
};