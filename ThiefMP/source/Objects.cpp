/*************************************************************
* File: Objects.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements VTable hooks of object-related methods.
*************************************************************/

#include "stdafx.h"
#include "Main.h"
#include "Player.h"
#include "Client.h"
#include "Gamesys.h"

int ObjectIsVisible(int object)
{
	if (!g_pObjSys->Exists(object) && g_Net)
	{
		return false;
	}
	else
		return _rendobj_object_is_visible(object);
}

//======================================================================================

namespace HookObjectNetworking
{

void __stdcall ClearTables(IObjectNetworking* thisObj)
{
	return cObjectNetworking::ClearTables(thisObj);
}

// shortObj is also obj index?
void __stdcall ObjRegisterProxy(IObjectNetworking* thisObj, int ownerID, short shortObj, int object)
{
	return cObjectNetworking::ObjRegisterProxy(thisObj, ownerID, shortObj, object);
}

int __stdcall ObjGetProxy(IObjectNetworking* thisObj, int P1, short P2)
{
	return cObjectNetworking::ObjGetProxy(thisObj, P1, P2);
}

void __stdcall ObjDeleteProxy(IObjectNetworking* thisObj, int P1)
{
	return cObjectNetworking::ObjDeleteProxy(thisObj, P1);
}

int __stdcall ObjIsProxy(IObjectNetworking* thisObj, int object)
{
	return cObjectNetworking::ObjIsProxy(thisObj, object);
}

void __stdcall ObjHostInfo(IObjectNetworking* thisObj, int P1, int* P2, short* P3)
{
	return cObjectNetworking::ObjHostInfo(thisObj, P1, P2, P3);
}

void __stdcall ObjTakeOver(IObjectNetworking* thisObj, int objectID)
{
	if (Debug.IsFlagSet(DEBUG_OBJECTS))
		ConPrintF("ObjTakeOver: %s.", _ObjEditName(objectID));

	return cObjectNetworking::ObjTakeOver(thisObj, objectID);
}

void __stdcall ObjGiveTo(IObjectNetworking* thisObj, int P1, int P2, int P3)
{
	if (Debug.IsFlagSet(DEBUG_OBJECTS))
		ConPrintF("ObjGiveTo: %d, %d, %d.", P1, P2, P3);

	return cObjectNetworking::ObjGiveTo(thisObj, P1, P2, P3);
}

void __stdcall ObjGiveWithoutObjID(IObjectNetworking* thisObj, int P1, int P2)
{
	if (Debug.IsFlagSet(DEBUG_OBJECTS))
		ConPrintF("ObjGiveWithoutObjID: %s, %s.", _ObjEditName(P1), _ObjEditName(P2));

	return cObjectNetworking::ObjGiveWithoutObjID(thisObj, P1, P2);
}

void __stdcall StartBeginCreate(IObjectNetworking* pThis, int exemplar, int objectID)
{
	return cObjectNetworking::StartBeginCreate(pThis, exemplar, objectID);

	//if (!pThis->m_bGameModeStarted)
	//{
	//	pThis->x38 = objectID;
	//	return;
	//}

	//cObjectNetworkingFns::ClearProxyEntry(pThis, NULL, objectID);
	//BOOL bSuspended = false;

	//if (pThis->mpNetMan->Suspended())
	//{
	//	_SetNetworkCategory(objectID, CAT_LocalOnly);
	//	bSuspended = true;
	//}

	//int var = pThis->x30++ + 1;

	//if (var <= 1)
	//{
	//	if (pThis->m_lastBeginCreateObject)
	//	{
	//		pThis->ObjRegisterProxy(pThis->m_lastBeginCreateSender, (short)pThis->m_lastBeginCreateObject, objectID);
	//		pThis->m_lastBeginCreateObject = 0;
	//		pThis->mpNetMan->SuspendMessaging();

	//		return;
	//	}

	//	if (_NetworkCategory(exemplar) != CAT_LocalOnly && !bSuspended)
	//	{
	//		pThis->ObjRegisterProxy(*_gPlayerObj, objectID, objectID);
	//		pThis->m_pBeginCreateMsg->Send(0, exemplar, objectID);
	//	}

	//}

	//pThis->mpNetMan->SuspendMessaging();
}

void __stdcall FinishBeginCreate(IObjectNetworking* thisObj, int objectID)
{
	if (Debug.IsFlagSet(DEBUG_OBJECTS))
		ConPrintF("Object creation: %s.", _ObjEditName(objectID));

	return cObjectNetworking::FinishBeginCreate(thisObj, objectID);
}

void __stdcall StartEndCreate(IObjectNetworking* thisObj, int objectID)
{
	return cObjectNetworking::StartEndCreate(thisObj, objectID);
}

void __stdcall FinishEndCreate(IObjectNetworking* pThis, int objectID)
{
	return cObjectNetworking::FinishEndCreate(pThis, objectID);

	//Position* pos;

	//if (!pThis->m_bGameModeStarted)
	//{
	//	pThis->x38 = objectID;
	//	return;
	//}

	//pThis->mpNetMan->ResumeMessaging();

	//if (!pThis->x1c)
	//{
	//	if (pThis->mpObjSys->Exists(objectID))
	//	{
	//		if (_NetworkCategory(objectID) != CAT_LocalOnly)
	//		{
	//			pos = _ObjPosGet(objectID);
	//			if (pos)
	//			{
	//				pThis->m_pCreateObjPosMsg->Send(0, objectID, pos, &pos->angle.pitch);
	//			}
	//		}
	//	}
	//	else
	//	{
	//	}
	//}
}

void __stdcall StartDestroy(IObjectNetworking* thisObj, int objectID)
{
	if (Debug.IsFlagSet(DEBUG_OBJECTS))
		ConPrintF("Object destruction: %s", _ObjEditName(objectID));
	return cObjectNetworking::StartDestroy(thisObj, objectID);
}

void __stdcall NotifyObjRemapped(IObjectNetworking* p, int mapFrom, int mapTo)
{
	DbgPrintEcho("REMAP: file %d; mapping %d->%d", _dbCurrentFilenum, mapFrom, mapTo);

	return cObjectNetworking::NotifyObjRemapped(p, mapFrom, mapTo);
}

} // HookObjectNetworking

#if (GAME == GAME_THIEF || GAME == GAME_DROMED)

extern int g_giveObj;

namespace HookContainSys
{

long __stdcall Add(IContainSys* pContainSys, int objReceiving, int objectID, unsigned int P1, unsigned int P2)
{
	if (Debug.IsFlagSet(DEBUG_INVENTORY))
		DbgPrint("Adding %s to %d's inventory.", _ObjEditName(objectID), objReceiving);

	if (g_pDarkNet && g_pDarkNet->GetSessionInfo().shareLoot)
	{
		if (objReceiving == *_gPlayerObj && g_pTraitMan->ObjHasDonor(objectID, Gamesys.Arch.IsLoot))
		{
			if (g_Net == STATE_Client)
			{
				CNetMsg_GiveObject CNetMsg_GiveObject;
				CNetMsg_GiveObject.object = g_pNetMan->ToGlobalObjID(objectID);

				g_pDarkNet->SendToHost((void*)&CNetMsg_GiveObject, sizeof(CNetMsg_GiveObject), DPNSEND_GUARANTEED);
				return 1;
			}
			else
			{
				if (g_giveObj != objectID)
					SendAddLoot(objectID, g_pNetMan->MyPlayerNum());
			}
		}
	}

	return cContainSys::Add(pContainSys, objReceiving, objectID, P1, P2);
}

} // HookContainSys

namespace HookInventory
{

long __stdcall Select(IInventory* thisObj, int object)
{
	if (g_pNetMan->IsNetworkGame())
	{
		// Is this a melee weapon?
		if (!_has_projectile_type(object) && _IsWeapon(object))
		{
			// Make sure we aren't trying to equip a "junk" weapon, such as a hammer
			int type;
			if (g_pInventory->m_pInvTypeProp->Get(object, &type))
			{
				if (type == INVTYPE_Weapon)
				{
#ifdef _DEBUG
					ConPrintF("Equipping weapon %s", _ObjEditName(object));
#endif
					SendSetWeapon(object);
					_EquipWeapon(*_gPlayerObj, object, g_pTraitMan->ObjHasDonor(object, Gamesys.Arch.Blackjack) ? 1 : 0);
				}
			}
		}
	}

	return cInventory::Select(thisObj, object);
}

long __stdcall ClearSelection(IInventory* thisObj, eWhichInvObj invObj)
{
	if (g_pNetMan->IsNetworkGame())
	{
		if (invObj == IO_Weapon)
		{
			// Notify other players that we're unequipping a weapon
			int object = g_pInventory->Selection(invObj);

			if (!_has_projectile_type(object) && _IsWeapon(object))
			{
				SendSetWeapon(0);
				_UnEquipWeapon(*_gPlayerObj, object);
			}
		}
	}

	return cInventory::ClearSelection(thisObj, invObj);
}

} // end HookInventory

namespace HookObjectSystem
{

long __stdcall Unlock(IObjectSystem* pObjSys)
{
	long ret = cObjectSystem::Unlock(pObjSys);

//#ifndef _RELEASE
//	if (!(g_pObjSys->mLockCount >= 0))
//		__debugbreak();
//#endif

	return ret;
}

} // end HookObjectSystem

#endif // GAME == GAME_THIEF || GAME == GAME_DROMED