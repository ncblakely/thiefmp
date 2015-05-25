/*************************************************************
* File: ScriptHooks.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Hacks in network awareness for game scripts
*************************************************************/

#include "stdafx.h"

#include "Main.h"

#define _DEFINE_SCRIPT_OFFSETS
#include "ScriptImports.h"
#include "StringHashTable.h"
#include "Patcher.h"
#include "Gamesys.h"

void __stdcall WorldFrobbableReceiveMessage(IRootScript* pThis, sScrMsg* pMsg, cMultiParm* pParms, enum eScrTraceAction trace)
{
	StringHashID strid = g_pStrHash->FindString(pMsg->msgName);

	switch (strid)
	{
	case STRID_FrobWorldEnd:
		if (g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			CNetMsg_ScriptMsg msg(pMsg->source, pMsg->dest, "NetFrob", NULL);

			msg.Send(ALL_PLAYERS, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
		}

		break;
	case STRID_NetFrob:
		if (!g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			if (Debug.IsFlagSet(DEBUG_FROBS))
			DbgPrint("Net frob debug: calling on frob world end for %s with source %d.", _ObjEditName(pMsg->dest), pMsg->source);

			int reverse = ProxyFrobGetInverse(pMsg->dest);
			if (OBJ_IS_CONCRETE(reverse) && reverse != pMsg->dest)
			{
				// Don't send this to an invalid object, or create an endless loop
				if (ProxyFrobGetInverse(reverse) == pMsg->dest)
				{
					DbgPrintEcho("Error: proxy frob reverse object is the same as the destination for %d!", pMsg->dest);
					return;
				}

				sScrMsg msg;
				ZeroMemory(&msg, sizeof(sScrMsg));

				msg.msgName = "NetFrob";
				msg.dest = reverse;
				msg.source = pMsg->source;
				msg.flags = SSCRMSG_ISBROADCAST;

				g_pScriptMan->SendMessageA(&msg, pParms);
			}

			return pThis->OnFrobWorldEnd(pMsg, pParms);
		}
	default:
		break;
	}

	return ScriptReceiveMessage(pThis, pMsg, pParms, trace);
}

void __stdcall HookStdElevatorReceiveMessage(StdElevatorScript* pThis, sScrMsg* pMsg, cMultiParm* pParms, enum eScrTraceAction trace)
{
	StringHashID strid = g_pStrHash->FindString(pMsg->msgName);

	switch (strid)
	{
	case STRID_TurnOn:
	case STRID_TurnOff:
		if (g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			cMultiParm parm;
			parm._int = 0;

			g_pNetSrv->Broadcast(pMsg->dest, strid == STRID_TurnOn ? "NetTurnOn" : "NetTurnOff", 0, parm);
			DbgPrint("Sent %s", pMsg->msgName);
		}
		break;
	case STRID_NetTurnOn:
	case STRID_NetTurnOff:
		{
			DbgPrint("Received %s", pMsg->msgName);
			pThis->OnActivate(strid == STRID_TurnOn ? true : false);
		}
		break;
	}

	return ScriptReceiveMessage(pThis, pMsg, pParms, trace);
}

void __stdcall HookContainerReceiveMessage(IRootScript* pThis, sScrMsg* pMsg, cMultiParm* pParms, enum eScrTraceAction trace)
{
	StringHashID strid = g_pStrHash->FindString(pMsg->msgName);

	switch (strid)
	{
	case STRID_FrobWorldEnd:
		if (g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			CNetMsg_ScriptMsg msg(((sFrobMsg*)pMsg)->frobber, pMsg->dest, "NetFrob", NULL);
			msg.Send(ALL_PLAYERS, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
		}

		break;
	case STRID_NetFrob:
		if (!g_pObjNet->ObjHostedHere(pMsg->dest))
			return pThis->OnFrobWorldEnd(pMsg, pParms);

	default:
		break;
	}

	return ScriptReceiveMessage(pThis, pMsg, pParms, trace);
}

void __stdcall HookLockSoundsReceiveMessage(IRootScript* pThis, sScrMsg* pMsg, cMultiParm* pParms, enum eScrTraceAction trace)
{
	StringHashID strid = g_pStrHash->FindString(pMsg->msgName);

	switch (strid)
	{
	case STRID_NowLocked:
		if (g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			CNetMsg_ScriptMsg msg(NULL, pMsg->dest, "NetLocked", NULL);

			msg.Send(ALL_PLAYERS, DPNSEND_NOLOOPBACK);
		}

		break;
	case STRID_NowUnlocked:
		if (g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			CNetMsg_ScriptMsg msg(NULL, pMsg->dest, "NetUnlocked", NULL);

			msg.Send(ALL_PLAYERS, DPNSEND_NOLOOPBACK);
		}

		break;
	case STRID_NetNowLocked:
		if (!g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			cScrStr tag;

			tag.m_string = "Event StateChange, LockState Locked";
			g_cSoundScrSrvScriptService->PlayEnvSchema(pThis->m_scriptObjID, tag, pThis->m_scriptObjID, NULL, (eEnvSoundLoc)1, (eSoundNetwork)0);
		}

		break;
	case STRID_NetNowUnlocked:
		if (!g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			cScrStr tag;

			tag.m_string = "Event StateChange, LockState Unlocked";
			g_cSoundScrSrvScriptService->PlayEnvSchema(pThis->m_scriptObjID, tag, pThis->m_scriptObjID, NULL, (eEnvSoundLoc)1, (eSoundNetwork)0);
		}

		break;
	default:
		break;
	}
			
	return ScriptReceiveMessage(pThis, pMsg, pParms, trace);
}

void __stdcall HookStdBookReceiveMessage(IRootScript* pThis, sScrMsg* pMsg, cMultiParm* pParms, enum eScrTraceAction trace)
{
	StringHashID strid = g_pStrHash->FindString(pMsg->msgName);

	switch (strid)
	{
	case STRID_FrobWorldEnd:
		if (g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			int frobber = ((sFrobMsg*)pMsg)->frobber;
			if (frobber != *_gPlayerObj && _IsAPlayer(frobber))
			{
				DPNID dpnidObjHost = g_pNetMan->ToNetPlayerID(g_pObjNet->ObjHostPlayer(frobber));

				if (dpnidObjHost != -1)
				{
					CNetMsg_ScriptMsg msg(((sFrobMsg*)pMsg)->frobber, pMsg->dest, "NetFrob", NULL);

					msg.Send(dpnidObjHost, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
				}

				// Don't open the readable on the host's screen
				return;
			}
		}

		break;
	case STRID_NetFrob:
		return pThis->OnFrobWorldEnd(pMsg, pParms);
	default:
		break;
	}

	return ScriptReceiveMessage(pThis, pMsg, pParms, trace);
}

void __stdcall HookStdKeyReceiveMessage(IRootScript* pThis, sScrMsg* pMsg, cMultiParm* pParms, enum eScrTraceAction trace)
{
	StringHashID strid = g_pStrHash->FindString(pMsg->msgName);

	switch (strid)
	{
	case STRID_FrobToolEnd:
		{
			int frobber = ((sFrobMsg*)pMsg)->frobber;
			int dest = ((sFrobMsg*)pMsg)->frobDest;

			if (frobber == *_gPlayerObj && !g_pObjNet->ObjHostedHere(((sFrobMsg*)pMsg)->frobDest))
			{
				ConPrintF("Taking over door %s", _ObjEditName(dest));
				g_pObjNet->ObjTakeOver(dest);

				// If this door was owned by another player, the scripts running on it will have no idea that the sim has started.
				// This ensures that locked objects will play the appropriate sounds when locked/unlocked, and unlocked objects will open automatically.
				sScrDatumTag tag;
				tag.dataName = "Sim";
				tag.scriptName = "LockSounds";
				tag.object = dest;
				g_pScriptMan->SetScriptData(&tag, &sMultiParm(1));

				tag.scriptName = "StdDoor";
				g_pScriptMan->SetScriptData(&tag, &sMultiParm(1));

				tag.scriptName = "NonAutoDoor";
				g_pScriptMan->SetScriptData(&tag, &sMultiParm(1));

				tag.scriptName = "ToggleDoor";
				g_pScriptMan->SetScriptData(&tag, &sMultiParm(1));
			}

			return pThis->OnFrobToolEnd(pMsg, pParms);
		}
	default:
		break;
	}

	return ScriptReceiveMessage(pThis, pMsg, pParms, trace);
}

void __stdcall HookCameraAlertReceiveMessage(IRootScript* pThis, sScrMsg* pMsg, cMultiParm* pParms, enum eScrTraceAction trace)
{
	StringHashID strid = g_pStrHash->FindString(pMsg->msgName);

	switch (strid)
	{
	case STRID_TurnOn:
		if (g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			DbgPrint("Net camera alert: sending turn on");
			CNetMsg_ScriptMsg msg(((sFrobMsg*)pMsg)->frobber, pMsg->dest, "NetTurnOn", NULL);

			msg.Send(ALL_PLAYERS, DPNSEND_NOLOOPBACK | DPNSEND_GUARANTEED);
		}
		break;
	case STRID_TurnOff:
		if (g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			DbgPrint("Net camera alert: sending turn off");
			CNetMsg_ScriptMsg msg(((sFrobMsg*)pMsg)->frobber, pMsg->dest, "NetTurnOff", NULL);

			msg.Send(ALL_PLAYERS, DPNSEND_NOLOOPBACK | DPNSEND_GUARANTEED);
		}
		break;
	case STRID_NetTurnOn:
		if (!g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			DbgPrint("Net camera alert: turn on");
			pMsg->msgName = "TurnOn";
			return pThis->OnMessage(pMsg, pParms);
		}
		return;
	case STRID_NetTurnOff:
		if (!g_pObjNet->ObjHostedHere(pMsg->dest))
		{
			DbgPrint("Net camera alert: turn off");
			pMsg->msgName = "TurnOff";
			return pThis->OnMessage(pMsg, pParms);
		}
		return;
	}

	return ScriptReceiveMessage(pThis, pMsg, pParms, trace);
}

void __stdcall HookArrowReceiveMessage(IRootScript* pThis, sScrMsg* pMsg, cMultiParm* pParms, enum eScrTraceAction trace)
{
	StringHashID strid = g_pStrHash->FindString(pMsg->msgName);

	if (strid == STRID_Contained && ((sContainedMsg*)pMsg)->event == 2)
	{
		//DbgPrint("Arrow %s contained by %d with event %d", _ObjEditName(pThis->m_scriptObjID), ((sContainedMsg*)pMsg)->containedBy, ((sContainedMsg*)pMsg)->event);
		if (_NetworkCategory(pThis->m_scriptObjID) == CAT_Hosted && ((sContainedMsg*)pMsg)->containedBy == *_gPlayerObj)
		{
			//DbgPrint("Trying to remap obj with arch %s", _ObjEditName(g_pTraitMan->GetArchetype(pThis->m_scriptObjID)));
			int localProjectile = Gamesys.PickupToLocalProjectile(g_pTraitMan->GetArchetype(pThis->m_scriptObjID));
			if (OBJ_IS_ABSTRACT(localProjectile))
			{
				int stackCount = 1;
				int obj = g_pObjSys->Create(localProjectile, 1);

				//DbgPrint("Remapped %d to %d, new obj %d", pThis->m_scriptObjID, localProjectile, obj);

				gStackCountProp->Get(pThis->m_scriptObjID, &stackCount);
				gStackCountProp->Set(obj, stackCount);

				g_pContainSys->Remove(*_gPlayerObj, pThis->m_scriptObjID);
				g_pObjSys->Destroy(pThis->m_scriptObjID);
				g_pContainSys->Add(*_gPlayerObj, obj, 0, 1);
			}
			else
				ConPrintEchoF("ERROR: Could not remap projectile %d with archetype %s.", pThis->m_scriptObjID, _ObjEditName(g_pTraitMan->GetArchetype(pThis->m_scriptObjID)));
		}
	}

	return ScriptReceiveMessage(pThis, pMsg, pParms, trace);
}

void __stdcall HookCollisionStickReceiveMessage(CollisionStickScript* pThis, sScrMsg* pMsg, cMultiParm* pParms, enum eScrTraceAction trace)
{
	StringHashID strid = g_pStrHash->FindString(pMsg->msgName);

	if (strid == STRID_PhysCollision)
	{
		try
		{
			BOOL localCopy;
			if (g_pLocalCopyProp->Get(pThis->m_scriptObjID, &localCopy))
			{
				// Destroy the local copy of the arrow. The host of the object will create the hosted pickup object
				pParms->_type = kMT_Int;
				pParms->_int = 3;
				return;
			}

			sPhysMsg* pPhysMsg = (sPhysMsg*)pMsg;
			true_bool tbCanAttach = pThis->ObjSupportsAttach(pPhysMsg->collideWith);

			if (tbCanAttach)
			{
				float timing = pThis->GetScriptTiming();

				int pickupObj = g_pObjSys->BeginCreate(Gamesys.LocalToPickupProjectile(g_pTraitMan->GetArchetype(pThis->m_scriptObjID)), 1);
				Position* pos = _ObjPosGet(pThis->m_scriptObjID);
				_ObjPosCopyUpdate(pickupObj, pos);

				_PhysDeregisterModel(pickupObj);

				gStackCountProp->Set(pickupObj, 1);

				g_pObjSys->EndCreate(pickupObj);

				if (timing > 0.0)
				{
					cMultiParm parm;
					parm._type = kMT_Undef;

					sScrDatumTag tag;

					tag.dataName = "timeout";
					tag.scriptName = pThis->GetName();
					tag.object = pickupObj;
					int timerHandle = g_pScriptMan->SetTimedMessage2(pickupObj, "Tick", (unsigned long)(timing * 1000.0f), kSTM_OneShot, parm);
					g_pScriptMan->SetScriptData(&tag, &sMultiParm(timerHandle));
				}
			}

			pParms->_type = kMT_Int;
			pParms->_int = 3;

			return;
		}

		catch (...) { ConPrintEchoF("ERROR: failed to execute CollisionStick."); }
	}

	return ScriptReceiveMessage(pThis, pMsg, pParms, trace);
}

void __stdcall HookVictoryCheckOnSlain(sScrMsg* pMsg, cMultiParm* pMultiParm)
{
	g_cDarkGameSrvScriptService->KillPlayer();

	return;
}

PtrPatchList g_GenOsmPatches[] =
{
	// Elevators
	{0x4FFE1AE0, &HookStdElevatorReceiveMessage, PatchType_Normal, 0}, // StdElevator
	//{0x4FFE1FB8, &HookStdElevatorReceiveMessage, PatchType_Normal, 0}, // StdTerrPoint

	// Levers
	{0x4FFE1308, &WorldFrobbableReceiveMessage, PatchType_Normal, 0}, // StdLever
	{0x4FFE1410, &WorldFrobbableReceiveMessage, PatchType_Normal, 0}, // LeverNoChain
	{0x4FFE1518, &WorldFrobbableReceiveMessage, PatchType_Normal, 0}, // JumperSwitch
	{0x4FFE1620, &WorldFrobbableReceiveMessage, PatchType_Normal, 0}, // StdGauge
	{0x4FFE1918, &WorldFrobbableReceiveMessage, PatchType_Normal, 0}, // DoubleButton

	// Doors
	{0x4FFE08B8, &WorldFrobbableReceiveMessage, PatchType_Normal, 0}, // StdDoor
	{0x4FFE09A8, &WorldFrobbableReceiveMessage, PatchType_Normal, 0}, // NonAutoDoor
	{0x4FFE0B70, &WorldFrobbableReceiveMessage, PatchType_Normal, 0}, // ToggleDoor

	// Buttons
	{0x4FFE1738, &WorldFrobbableReceiveMessage, PatchType_Normal, 0}, // StdButton
	//{0x10051828, &WorldFrobbableReceiveMessage, &ScriptReceiveMessage}, // NumberButton

	// StdBook
	{0x4FFE3C88, &HookStdBookReceiveMessage, PatchType_Normal, 0},

	// Container
	{0x4FFE6800, &HookContainerReceiveMessage, PatchType_Normal, 0},

	// StdKey
	{0x4FFE0D30, &HookStdKeyReceiveMessage, PatchType_Normal, 0},

	// CameraAlert
	//{0x4FFE2AA8, &HookCameraAlertReceiveMessage, 0},

	// Arrow
	{0x4FFE5A48, &HookArrowReceiveMessage, PatchType_Normal, 0},

	// CollisionStick
	{0x4FFE5D70, &HookCollisionStickReceiveMessage, PatchType_Normal, 0},

	// LockSounds
	{0x4FFE0EB0, &HookLockSoundsReceiveMessage, PatchType_Normal, 0},

	PATCHARRAY_END
};

PtrPatchList g_ConvictOsmPatches[] =
{
	{0x3FFE7E70, &HookVictoryCheckOnSlain},
	{0x3FFE7D50, &HookVictoryCheckOnSlain},

	PATCHARRAY_END
};

void ApplyGenPatches()
{
	HMODULE hMod = GetModuleHandle("gen.osm");

	dbgassert(hMod);

	if (hMod)
	{
		char* pAddress = (char*)hMod + (0x51308); // 0x4FFE1308
		SafeRead((void*)pAddress, &ScriptReceiveMessage, 4);
		MemPatcher::WritePtr(g_GenOsmPatches, hMod, 0x4FF90000);
	}
}

void ApplyConvictPatches()
{
	HMODULE hMod = GetModuleHandle("convict.osm");

	dbgassert(hMod);

	if (hMod)
	{
		MemPatcher::WritePtr(g_ConvictOsmPatches, hMod, 0x3FFD0000);
	}
}

bool IsScriptModule(const char* s1, const char* s2)
{
	char buff[128];

	sprintf(buff, "%s.osm", s2);
	return (!_stricmp(s1, s2) || !_stricmp(s1, buff));
}

namespace HookScriptMan
{

long __stdcall AddModule(DWORD thisObj, const char* modName)
{
	long ret = cScriptMan::AddModule(thisObj, modName);

	if (g_pNetMan->IsNetworkGame())
	{
		if (IsScriptModule(modName, "gen"))
		{
			DbgPrint("Patching script module %s.", modName);
			ApplyGenPatches();
		}
		else if (IsScriptModule(modName, "convict"))
		{
			DbgPrint("Patching script module %s.", modName);
			ApplyConvictPatches();
		}
		// mission 12 hack - why the hell doesn't this just use gen.osm like every other mission?
		//else if (IsScriptModule(modName, "miss12"))
		//{
		//	long ret = cScriptMan::AddModule(thisObj, "gen.osm");
		//	ApplyGenPatches();
		//}
	}

	return ret;
}

long __stdcall SendMessageA(DWORD thisObj, sScrMsg* pMsg, sMultiParm* pParms)
{
	if (Debug.IsFlagSet(DEBUG_SCRIPTS))
		ConPrintF("Sending message %s from %d to %s.", pMsg->msgName, pMsg->source, _ObjEditName(pMsg->dest));

	return cScriptMan::SendMessageA(thisObj, pMsg, pParms);
}

} // end HookScriptMan