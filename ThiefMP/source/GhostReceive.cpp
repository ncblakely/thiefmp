/*************************************************************
* File: GhostReceive.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/


#include "stdafx.h"

#include "Main.h"

namespace HookGhostReceive
{

void ReceiveAIHeartbeat(int objectID, int seqID, sGhostHeartbeat* data)
{
	//sGhostRemote* ghost;
	//sGhostHeartbeat heartBeat;

	//if (ghost = _GhostGetRemote(objectID))
	//{	
	//	heartBeat = ghost->hbLocal;

	//	heartBeat.pos.x = data->posX;
	//	heartBeat.pos.y = data->posY;
	//	heartBeat.rot.x = data->rotX;
	//	heartBeat.rot.y = data->rotY;
	//	heartBeat.flag1 |= HBEAT_AI;
	//	heartBeat.s2 = data->x10;
	//	heartBeat.s1 = data->x12;
	//	//ConPrintF("hb for %s: p %.4f %.4f r %.4f %.4f flags %x s: %hd %hd", _ObjEditName(objectID), heartBeat.pos.x, heartBeat.pos.y, heartBeat.rot.x, heartBeat.rot.y, heartBeat.hbFlags, heartBeat.s, heartBeat.s1);

	//	HandleGhostFullHB(objectID, seqID, NULL, &heartBeat, NULL);
	//}
}

void ReceiveFullHeartbeat(int objectID, int seqID, int relObj, sGhostHeartbeat* hbData, sGhostMoCap* mcData)
{
	//ConPrintF("mc data for %s: %d, %d (%s)", _ObjEditName(objectID), mcData->schema, mcData->motion, g_pMotionSet->GetName(mcData->motion));
	_GhostRecvPacket(objectID, seqID & 0xFFFF, relObj, hbData, mcData);
}

void ReceiveRotHeartbeat(int objectID, int seqID, sGhostHeartbeat* data)
{
	//sGhostRemote* ghost;
	//sGhostHeartbeat heartBeat;

	//if (ghost = _GhostGetRemote(objectID))
	//{
	//	heartBeat = ghost->hbLocal;
	//	heartBeat.pos.x = data->posX;
	//	heartBeat.pos.y = (short)data->posY;

	//	HandleGhostFullHB(objectID, seqID, NULL, &heartBeat, NULL);
	//}
}

} // HookGhostReceive