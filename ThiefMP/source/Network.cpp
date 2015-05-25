/*************************************************************
* File: Network.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "Client.h"
#include "DarkOverlay.h"

static BYTE ObjDataBuffer[2048];

// Sends object data for new netcode
// currently unused
void ObjectSendData(int object, int opcode, void* pData, DWORD size, DWORD flags)
{
	ObjMsg_Header header;

	header.type = PKT_ObjData;
	header.opcode = opcode;
	header.id = g_pNetMan->ToGlobalObjID(object);

	//DbgPrint("Sent object packet with op %d (%x:%x, obj %d)", opcode, header.id.objIDhostNum, header.id.unk, object);

	// Copy the header
	BYTE* bufferPtr = ObjDataBuffer;
	memcpy(bufferPtr, &header, sizeof(ObjMsg_Header));

	if (pData)
	{
		bufferPtr += sizeof(ObjMsg_Header);
		memcpy(bufferPtr, pData, size);
	}

	g_pDarkNet->SendToAll(ObjDataBuffer, size + sizeof(ObjMsg_Header), NULL, flags | DPNSEND_NOLOOPBACK);
}

void SendTimeLimit()
{
	CNetMsg_TimeLimit msg;

	msg.timer = Client.GetTimeRemaining();
	g_pDarkNet->SendToAll(&msg, sizeof(CNetMsg_TimeLimit), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
}

void SendEnteredWorld()
{
	//CNetMsg_EnteredWorld msg;

	//if (g_pNetMan->AmDefaultHost())
	//{
	//	g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, sizeof(CNetMsg_EnteredWorld), 0, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
	//}
	//else
	//	g_pDarkNet->SendToHost((void*)&msg, sizeof(CNetMsg_EnteredWorld), 0, DPNSEND_GUARANTEED);

	//Client.HandleEnteredWorld(&msg);
}

void SendAddLoot(int object, unsigned long playerFrom)
{
	sLoot* loot = NULL;

	if (g_pLootProp->Get(object, (void**)&loot))
	{
		CNetMsg_AddLoot msg;

		msg.archetype = g_pTraitMan->GetArchetype(object);
		msg.loot = *loot;
		msg.containingPlayerNum = (BYTE)playerFrom;

		g_pDarkNet->SendToAll((void*)&msg, sizeof(CNetMsg_AddLoot), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
	}
}

void SendEndMission()
{
	if (g_pNetMan->AmDefaultHost())
	{
		CNetMsg_EndMission msg;

		if (g_pDarkNet)
			g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, sizeof(CNetMsg_EndMission), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);

		// 7/16/10 - allow players to join again after mission over
		Client.m_missionStarted = false;
		g_pNetMan->m_bSynchDone = FALSE; // needs to be false or host will kick joiners

		g_pDarkNet->SendHeartbeat();
	}
}

void SendPlayerAnimation(const char* animName, const char* playerTagName)
{
	int anim = DarkPlayerActionToSchemaIdx(animName, playerTagName);

	ObjMsg_PlayerAnim msg;

	msg.anim = (short)anim;

	ObjectSendData(*_gPlayerObj, OD_PlayerAnimation, &msg, sizeof(ObjMsg_PlayerAnim), NULL);
}

void SendPlayerStopAnimation()
{
	ObjectSendData(*_gPlayerObj, OD_PlayerStopAnimation, NULL, 0, NULL);
}

void NetSimCallback(int a1, int a2)
{
	static bool WaitingForPlayers = false;

	if (IsStagingMode())
		return;

	if (a1)
	{
		if (a2)
		{
			MessageMgr::Get()->AddLine(false, "Waiting for players...");
			WaitingForPlayers = 1;
		}
		else
		{
			MessageMgr::Get()->AddLine(false, "Game paused.");
			WaitingForPlayers = 0;
		}
	}
	else
	{
		if (WaitingForPlayers)
			MessageMgr::Get()->AddLine(false, "All players joined.");
		else
			MessageMgr::Get()->AddLine(false, "Game resumed.");
	}
}