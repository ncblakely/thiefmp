/*************************************************************
* File: Packets.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Main.h"

#ifdef _THIEFBUILD
#include "PlayerDB.h"
#include "Client.h"
#include "Gamesys.h"

void HookFrobRequest(int msDown, int sourceObj, int destObj, int srcLoc, int destLoc, int frobber, int flags)
{
	return HandleFrobRequest(msDown, sourceObj, destObj, srcLoc, destLoc, frobber, flags);

	if (g_pNetMan->AmDefaultHost())
	{
		// Ensure that the item being frobbed is not in the player's inventory
		if (g_pContainSys->Contains(*_gPlayerObj, sourceObj))
			return HandleFrobRequest(msDown, sourceObj, destObj, srcLoc, destLoc, frobber, flags);
		else if (_IsAPlayer(frobber))
		{
			CNetMsg_FrobObject msg;
			msg.msDown = msDown;
			msg.sourceObj = g_pNetMan->ToGlobalObjID(sourceObj);
			msg.destObj = g_pNetMan->ToGlobalObjID(destObj);
			msg.srcLoc = srcLoc;
			msg.destLoc = destLoc;
			msg.frobberID = (BYTE)g_pNetMan->ObjToPlayerNum(frobber);
			msg.flags = flags;

			g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, sizeof(CNetMsg_FrobObject), 0, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);

			if (Debug.IsFlagSet(DEBUG_FROBS))
				ConPrintF("Frob send: %s was used by player %d", _ObjEditName(sourceObj), msg.frobberID);
		}
	}

	return HandleFrobRequest(msDown, sourceObj, destObj, srcLoc, destLoc, frobber, flags);
}

void HookFrobPickup(int frobber, int frobbedObj)
{
	if (Debug.IsFlagSet(DEBUG_FROBS))
		ConPrintF("Handle frob pickup for: %d, highlit obj %d", frobbedObj, *_highlit_obj);

	return HandleFrobPickup(frobber, frobbedObj);
}

void HookRegSphere(int object, int numSubmodels, DWORD flags, float radius, mxs_vector* location,  DWORD facing)
{
	//ConPrintF("reg sphere for obj %s, submodels %d, flags %x, radius %f, location %f, facing %f", _ObjEditName(object), numSubmodels, flags, radius, location, facing);

	return HandleRegSphere(object, numSubmodels, flags, radius, location, facing);
}

void HookHandleCreateQuestData(char* questName, int questData, int P2, DWORD thisObj)
{
	if (Debug.IsFlagSet(DEBUG_QUESTS))
		ConPrintF("Creating quest %s: %d, %d.", questName, questData, P2);

	HandleCreateQuestData(questName, questData, P2, thisObj);
}

void HookHandleSetQuestData(char* questName, int questData, DWORD thisObj)
{
	if (Debug.IsFlagSet(DEBUG_QUESTS))
		ConPrintF("Setting quest %s to: %d.", questName, questData);

	HandleSetQuestData(questName, questData, thisObj);
}

void HookHandleHaltSound(int handle, int object)
{
	//if (Debug.FlagSet(DEBUG_SOUNDS))
	//	ConPrintF("Halting handle %d, object %d", handle, object);

	//HandleHaltSound(handle, object);
}

void HookHandleRequestDestroy(int object)
{
	if (g_pObjSys->Exists(object))
	{
		if (Debug.IsFlagSet(DEBUG_OBJECTS))
			ConPrintF("Destroying object %s.", _ObjEditName(object));

		g_pObjSys->Destroy(object);
	}
	else
		Log.Print("Received a request to destroy %d, but the object did not exist.", object);
}

//==============================================================================
// HookHandleFinishSynch()
//
// Called when a player finishes synch.
//==============================================================================
void HookHandleFinishSynch(INetManager* pNetMan)
{
#if (GAME == GAME_THIEF)
	pNetMan->m_numSynched++;

	//if (Debug.IsFlagSet(DEBUG_NET))
		DbgPrint("Player synch completed: %d/%d synched.", pNetMan->m_numStartedSynch - 1, pNetMan->m_numSynched); // 1D9/1DD
#endif
}

//==============================================================================
// HookSynchReady()
//
// 
//==============================================================================
void HookSynchReady()
{
	DbgPrint("Sending finish synch: %d players, %d synching.", g_pNetMan->m_numPlayers, g_pNetMan->m_numStartedSynch);
	cNetManager_SynchReady(g_pNetMan, 0);
}

//==============================================================================
// HookHandlePlayerInfo()
//
// Called when player info is received (during synch or join).
//==============================================================================
void HookHandlePlayerInfo(int playerIndex, char* playerName, char* playerIP, int playerObject, INetManager* netman)
{
	if (!playerIP)
		playerIP = "";

	Players.SetPlayerInfo(playerIndex, playerName, playerObject);

	if (Debug.IsFlagSet(DEBUG_NET) || Debug.IsFlagSet(DEBUG_RECEIVES))
		ConPrintF("Received player info for player %d. Name: %s. IP: %s. Object: %d.", playerIndex, playerName, playerIP, playerObject);
	HandlePlayerInfo(playerIndex, playerName, playerIP, playerObject, netman);
}

//======================================================================================
// Name: HookHandleFireProjectile
//
// Desc: Replaces the fire projectile packet handler entirely and adds some missing functionality.
//======================================================================================
void HookHandleFireProjectile(int projOwner, int projArchetype, DWORD flags, float intensity, mxs_vector* pos, mxs_angvec* ang, mxs_vector* vel)
{
	const char* projSound = NULL;
	int proj = CreateProjectile(projOwner, projArchetype, flags, intensity, pos, ang, vel);

	_SchemaPlayObj("bowtwang_player", projOwner, 0);
	_ObjSetHasRefs(proj, 1);

	g_pLocalCopyProp->Set(proj, 1);
	//if ((*g_pPrjSoundProp)->Get(projArchetype, (const char**)&projSound))
	//{
	//	//ConPrintF("Playing %s on %s.", projSound, _ObjEditName(proj));
	//	_SchemaPlayObj(projSound, proj, 0);
	//}
}

#endif

void HookHandleRemap(BYTE P1, uint P2, sSingleRemap* P3)
{
	DbgPrint("Handle remap %d %d %x.", P1, P2, P3);

	cObjectNetworkingFns::HandleRemap(g_pObjNet, NULL, P1, P2, P3);
}

void HookHandleBeginCreate(int exemplar, int localObj, int senderObject)
{
	// exemplar is archetype, localObj is the ID of the object on the sender's end

#ifndef _RELEASE
	if (!g_pObjSys->Exists(exemplar))
	{
		ConPrintF("Exemplar object %d does not exist.", exemplar);
	}
#endif

	cObjectNetworkingFns::HandleBeginCreate(g_pObjNet, NULL, exemplar, localObj, senderObject);

	DbgPrint("BeginCreate: object with archetype %s, local obj %d, from sender obj %d. Objsys lock count: %d.", _ObjEditName(exemplar), localObj, senderObject, g_pObjSys->mLockCount);
}

void HookHandleEndCreate(int object)
{
	cObjectNetworkingFns::HandleEndCreate(g_pObjNet, NULL, object);

	DbgPrint("EndCreate: %s. Objsys lock count: %d.", _ObjEditName(object), g_pObjSys->mLockCount);
}

				//while (true)
				//{
				//	if (g_pObjSys->Exists(pIter->object))
				//	{
				//		//ConPrintF("Destroying %s", _ObjEditName(pIter->object));
				//		g_pObjNet->ObjTakeOver(pIter->object);
				//		g_pObjSys->Destroy(pIter->object);
				//	}

				//	if (!g_pContainSys->IterNext(pIter))
				//		break;
				//}

void HookHandleFrobPickup(int frobber, int object)
{
	if (!object)
		return;
	
	bool bUpdateLoot = false;
	bool bShouldCombine = true;

	if (g_pNetMan->Networking())
	{
		if (g_pTraitMan->ObjHasDonor(object, Gamesys.Arch.IsLoot) && g_pDarkNet->GetSessionInfo().shareLoot)
		{
			if (g_Net == STATE_Host)
			{
				bUpdateLoot = true;
			}
			else if (g_Net == STATE_Client)
			{
				CNetMsg_GiveObject CNetMsg_GiveObject;
				CNetMsg_GiveObject.object = g_pNetMan->ToGlobalObjID(object);

				g_pDarkNet->SendToHost((void*)&CNetMsg_GiveObject, sizeof(CNetMsg_GiveObject), DPNSEND_GUARANTEED);
				return;
			}
		}

		g_pObjNet->ObjTakeOver(object);

		if (g_pTraitMan->ObjHasDonor(object, g_pObjSys->GetObjectNamed("Projectile") && _NetworkCategory(object) == CAT_Hosted))
			bShouldCombine = false;
	}

	if (g_pTraitMan->ObjHasDonor(object, g_pObjSys->GetObjectNamed("Crystal")))
			bShouldCombine = false;

	pBeingTakenProp->Set(object, 0);

	int crystalArch = g_pObjSys->GetObjectNamed("Crystal");

	if (g_pInventory->GetType(object) != INVTYPE_Weapon)
	{
		if (bShouldCombine)
		{
			*dword_6F55D4 = object;
			sContainIter* pIter = g_pContainSys->IterStart(frobber);

			while (!pIter->bDone)
			{
				if (g_pContainSys->CanCombine(pIter->object, object, 0))
				{
					*dword_6F55D4 = pIter->object;
					break;
				}

				g_pContainSys->IterNext(pIter);
			}

			g_pContainSys->IterEnd(pIter);
		}
	}

	g_pContainSys->Add(frobber, object, 0, 1);

	//if (bUpdateLoot)
	//	SendAddLoot(object, g_pNetMan->MyPlayerNum());
}