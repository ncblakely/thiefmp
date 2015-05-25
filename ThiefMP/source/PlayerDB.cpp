/*************************************************************
* File: PlayerDB.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"
#include "PlayerDB.h"

PlayerDB Players;
const char* kDefaultName = "Unnamed Player";

PlayerDB::PlayerDB()
{
	ClearPlayerInfo();
}

const char* PlayerDB::NameFromIndex(int index)
{
	if (Players[index].playerName[0])
		return Players[index].playerName;
	else
		return kDefaultName;
}

const char* PlayerDB::NameFromDPNID(DPNID dpnid)
{
	for (int i = 1; i <= kMaxPlayers; i++)
		if (Players[i].dpnid == dpnid)
			return Players[i].playerName;

	return kDefaultName;
	//int playerObj = g_pNetMan->FromNetPlayerID(dpnid);
	//if (!playerObj)
	//	return kDefaultName;

	//int index = g_pNetMan->ObjToPlayerNum(playerObj);
	//if (!index)
	//	return kDefaultName;

	//return m_PlayerData[index].playerName;
}

const char* PlayerDB::NameFromObject(int object)
{
	for (int i = 1; i <= kMaxPlayers; i++)
	{
		PlayerData& pd = Players[i];
		if (pd.object == object)
		{
			if (!pd.playerName.Empty())
				return Players[i].playerName;
			else
				break;
		}
	}

	return kDefaultName;
}

void PlayerDB::ClearPlayerInfo()
{
	for (int i = 1; i <= kMaxPlayers; i++)
	{
		Players[i].playerName = "";
		Players[i].object = 0;
		Players[i].dpnid = 0;
		Players[i].connected = false;
	}
}

void PlayerDB::SetPlayerInfo(int index, const char* name, int object)
{
	Players[index].playerName = name;
	Players[index].object = object;
	if (object)
		Players[index].dpnid = g_pNetMan->ToNetPlayerID(object);
	Players[index].connected = true;
}