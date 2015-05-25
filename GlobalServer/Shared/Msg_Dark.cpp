/*************************************************************
* File: Msg_Dark.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements Thief-specific packets for the global server client.
*************************************************************/

#include "stdafx.h"

#include "Msg_Dark.h"
#include "Engine\inc\MarshalBuffer.h"

void Msg_DarkHeartbeat::Marshal(CMarshalBuffer* pBuffer)
{
	pBuffer->CopyWord(type);
	pBuffer->CopyWord(build);
	pBuffer->CopyWord(port);
	pBuffer->CopyByte(gameID);
	pBuffer->CopyByte(stateFlags);
	pBuffer->CopyString(gameName, sizeof(gameName));
	pBuffer->CopyString(missionName, sizeof(missionName));
}

void Msg_DarkHeartbeat::Unmarshal(CMarshalBuffer* pBuffer)
{
	// Skip the packet type
	pBuffer->SkipBytes(sizeof(type));

	build = pBuffer->GetWord();
	port = pBuffer->GetWord();
	gameID = pBuffer->GetByte();
	stateFlags = pBuffer->GetByte();
	pBuffer->GetString(gameName, sizeof(gameName));
	pBuffer->GetString(missionName, sizeof(missionName));
}

void Msg_DarkServerInfo::Marshal(CMarshalBuffer* pBuffer)
{
	pBuffer->CopyWord(type);
	pBuffer->CopyWord(build);
	pBuffer->CopyWord(port);
	pBuffer->CopyByte(gameID);
	pBuffer->CopyByte(stateFlags);
	pBuffer->CopyString(serverIP, sizeof(serverIP));
	pBuffer->CopyString(serverName, sizeof(serverName));
	pBuffer->CopyString(mapName, sizeof(mapName));
}

void Msg_DarkServerInfo::Unmarshal(CMarshalBuffer* pBuffer)
{
	// Skip the packet type
	pBuffer->SkipBytes(sizeof(type));

	build = pBuffer->GetWord();
	port = pBuffer->GetWord();
	gameID = pBuffer->GetByte();
	stateFlags = pBuffer->GetByte();
	pBuffer->GetString(serverIP, sizeof(serverIP));
	pBuffer->GetString(serverName, sizeof(serverName));
	pBuffer->GetString(mapName, sizeof(mapName));
}