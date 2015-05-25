#pragma once

#include "Msg_System.h"

class CMarshalBuffer;

// Dark engine game message IDs (Thief 2/Shock 2)
#define MsgID_DarkBase 200

// Server -> client
#define MsgID_DarkServerInfo (MsgID_DarkBase + 1)

// Client -> server
#define MsgID_DarkHeartbeat (MsgID_DarkBase + 50)

class Msg_DarkHeartbeat : public GLSPacket_V2
{
public:
	Msg_DarkHeartbeat() { type = (WORD)MsgID_DarkHeartbeat; }

	ushort build;
	ushort port;
	BYTE gameID;
	BYTE stateFlags;
	char gameName[32];
	char missionName[32];

	void Marshal(CMarshalBuffer* pBuffer);
	void Unmarshal(CMarshalBuffer* pBuffer);
};

class Msg_DarkServerInfo: public GLSPacket_V2
{
public:
	Msg_DarkServerInfo() { type = (WORD)MsgID_DarkServerInfo; }

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