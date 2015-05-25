/*************************************************************
* File: GlobalServer.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements Thief-specific event callbacks for the global server
* client.
*************************************************************/

#include "stdafx.h"

#include "GlobalServer.h"
#include "Client.h"

void GS_OnReceive(IGSClient* client, BYTE* data, DWORD dataSize)
{
	if (!client || !data)
		return;

	GLSPacket_V2* msg = (GLSPacket_V2*)data;
	IGSBrowser* browser = client->GetBrowser();

	switch (msg->type)
	{
	case MsgID_DarkServerInfo: 
		{		
			assert(browser);
			if (browser)
				browser->HandleServerInfo(data, dataSize); 
			break;
		}
	case MsgID_RemoveServer:
		{
			assert(browser);
			if (browser)
				browser->HandleRemoveServer(data, dataSize);
			break;
		}
	default: Log.Print("Unknown packet received from global server. (%d)", msg->type); break;
	}
}

void GS_OnConnectionLost(IGSClient* client)
{
	if (Debug.IsFlagSet(DEBUG_GLOBAL))
		ConPrintF("Lost connection to global server.");
}

void GS_OnConnectComplete(IGSClient* client, IGSClient::ConnectResult cr)
{
	switch (cr)
	{
	case IGSClient::CR_OK: DbgPrint("Connection to global server established."); break;
	default: ConPrintF("Failed to connect to global server."); break;
	}
}

void GS_SendHeartbeat(IGSClient* client)
{
	if (!client)
		return;

	BYTE buff[512];
	CDarkMarshalBuffer marshalBuff;
	Msg_DarkHeartbeat msg;

	msg.build = (ushort)TMP_BUILD;
	msg.port = (ushort)Cfg.GetInt("ServerPort");
	msg.stateFlags = 0;
	msg.gameID = (BYTE)GAME;
	strcpy(msg.gameName, Cfg.GetString("ServerName"));

	// Set the game state flags
	if (strlen(Cfg.GetString("Password")))
		msg.stateFlags |= GAME_STATE_PASSWORD;
	if (Client.m_missionStarted)
		msg.stateFlags |= GAME_STATE_CLOSED;
	else
		msg.stateFlags |= GAME_STATE_OPEN;

	if (!GetMissionName(_GetNextMission(), (char*)buff, sizeof(buff)))
		msg.missionName[0] = NULL;
	else
		safe_strcpy(msg.missionName, (char*)buff, sizeof(msg.missionName));

	marshalBuff.SetBuffer(buff, sizeof(buff));
	msg.Marshal(&marshalBuff);

	int size = marshalBuff.GetBytesWritten();

	client->Send((void*)marshalBuff.GetStartBuffer(), size, false);
}

CServerList::ServerList& CServerList::GetServers()
{
	return m_Servers;
}