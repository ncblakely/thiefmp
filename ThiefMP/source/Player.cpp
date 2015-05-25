/*************************************************************
* File: Patcher.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Hooks various functions related to the player object
*************************************************************/
#include "stdafx.h"

#include "Main.h"
#include "Client.h"
#include "Gamesys.h"

void PlayerCreateHook(int event, int playerObj)
{
	OldPlayerCreateHook(event, playerObj);

	//if (event == 0)
	//{
	//	if (g_pNetMan->IsNetworkGame())
	//		g_pObjNet->ObjRegisterProxy(playerObj, playerObj, playerObj);
	//}
}

void ForceActivateLoadoutCache(int cacheObject)
{
	//g_pContainSys->MoveAllContents(*_gPlayerObj, cacheObject, 1);
	//g_pScriptMan->ForgetObj(cacheObject);

	//return;

	sSimMsg msg;

	if (cacheObject)
	{
		sSimMsgFns::Constructor(&msg, NULL, cacheObject, TRUE);

		g_pScriptMan->SendMessageA(&msg, NULL);

		sSimMsgFns::Destructor(&msg, NULL);

		g_pScriptMan->ForgetObj(cacheObject);
	}
}

void MPPlayerCreate()
{
	_PlayerDestroy();

	sLink link;
	link.destObj = -1;

	int objStartingPoint = PlayerFactoryHook();

	if (objStartingPoint)
	{
		long linkID = g_pPlayerFactoryRelation->GetSingleLink(objStartingPoint, NULL);
		g_pPlayerFactoryRelation->Get(linkID, &link);
	}

	*_gPlayerObj = g_pObjSys->BeginCreate(link.destObj, 1);
	AttachPlayerCamera();

	g_pObjSys->NameObject(*_gPlayerObj, "Player");

	PlayerCreateHook(0, *_gPlayerObj);

	if (objStartingPoint)
	{
		g_pObjSys->CloneObject(*_gPlayerObj, objStartingPoint);

		if (g_pNetMan->IsNetworkGame())
		{
			g_pNetMan->SuspendMessaging();

			int marker = g_pObjSys->GetObjectNamed("Marker");
			int object = 0;
			IObjectQuery* pQuery = g_pObjSys->Iter(1);

			// Convert projectiles to their pickup archetypes
			while (!pQuery->Done())
			{
				object = pQuery->Object();

				if (int pickup = Gamesys.LocalToPickupProjectile(g_pTraitMan->GetArchetype(object)))
				{
					if (!g_pAIProjectileRelation->GetSingleLink(0, object))
					{
						//DbgPrint("Setting arch for %s to pickup proj %d", _ObjEditName(object), pickup);
						g_pTraitMan->SetArchetype(object, pickup);
					}
				}
				pQuery->Next();
			}

			pQuery->Release();

			pQuery = g_pObjSys->Iter(1);

			// Activate all loadout caches/boxes in the mission
			// Without this, some players won't start with all the items they should
			while (!pQuery->Done())
			{
				object = pQuery->Object();

				if (ObjHasScript(object, "LoadoutCache") || ObjHasScript(object, "LoadoutBox"))
				{
					ForceActivateLoadoutCache(object); // force the script to activate
				}
				pQuery->Next();
			}

			pQuery->Release();

			// Miss1 conversation hack
			int m1gatehouse = g_pObjSys->GetObjectNamed("InGatehouse");
			if (g_pObjSys->Exists(m1gatehouse))
			{
				DbgPrint("Running miss1 conversation hack");
				_SetNetworkCategory(m1gatehouse, CAT_LocalOnly);
			}
		}

		g_pContainSys->MoveAllContents(*_gPlayerObj, objStartingPoint, 1);
		if (g_pNetMan->IsNetworkGame())
			g_pNetMan->ResumeMessaging();

		Position* pos = _ObjPosGet(*_gPlayerObj);
		if (pos)
			_ObjPosCopyUpdate(*_gPlayerObj, pos);
	}

	g_pObjSys->EndCreate(*_gPlayerObj);
}

void SendSetWeapon(int weapon)
{
	if (g_Net)
	{
		CNetMsg_SetWeapon msg;

		if (weapon)
		{
			if (g_pTraitMan->ObjHasDonor(weapon, Gamesys.Arch.Blackjack))
				msg.weapType =  CNetMsg_SetWeapon::WeaponType_Blackjack;
			else if (g_pTraitMan->ObjHasDonor(weapon, Gamesys.Arch.Projectile))
				msg.weapType =  CNetMsg_SetWeapon::WeaponType_Bow;
			else
				msg.weapType =  CNetMsg_SetWeapon::WeaponType_Sword;
		}
		else
			msg.weapType =  CNetMsg_SetWeapon::WeaponType_None;

		g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, sizeof(CNetMsg_SetWeapon), 0, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
	}
}

void __stdcall EquipBowHook(int playerObj, int arrowArch, int P1)
{
	SendSetWeapon(arrowArch);
	_SetWeapon(playerObj, arrowArch, P1);
}

void __stdcall UnEquipBowHook(int playerObj)
{
	SendSetWeapon(0);
	_UnSetWeapon(playerObj);
}

int LaunchProjectileHook(int launcherObject, int projObject, float velocity, DWORD flags, DWORD P1, DWORD P2, DWORD P3)
{
	if (g_Net)
	{
		int projArch = g_pTraitMan->GetArchetype(projObject);

		_launchProjectile(launcherObject, projObject, velocity, flags, P1, P2, P3);

		mxs_vector velocity;
		_PhysGetVelocity(projObject, &velocity);

		mxs_angvec emptyang;
		ZeroMemory(&emptyang, sizeof(mxs_angvec));

		SendProjectilePacket(launcherObject, projArch, 0x204, 1.35f, &_ObjPosGet(projObject)->vec, &emptyang, &velocity);

		return *_g_arrowObj;
	}
	else
	{
		_launchProjectile(launcherObject, projObject, velocity, flags, P1, P2, P3);
		return *_g_arrowObj;
	}
}

eFrobHandler __stdcall FrobItemHook(int objectID)
{
	return _FrobHandler(objectID);
}

// Broadcasts object position after a creature is thrown
void ThrowCreatureHook(int thrower, int thrown, float velocity, DWORD flags, DWORD P1, DWORD P2, DWORD P3)
{
	_PhysNetForceContainedMsgs(1);
	_launchProjectile(thrower, thrown, velocity, flags, P1, P2, P3);
	_PhysNetBroadcastObjPosByObj(thrown);
	_PhysNetForceContainedMsgs(0);
}