/*************************************************************
* File: Commands.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Imports.h"
#include "DarkNet.h"
#include "Packets.h"
#include "DarkOverlay.h"
#include "Client.h"
#include "Config.h"
#include "Difficulty.h"

DWORD* lastWeap = (DWORD*)0x00671BA0;

typedef bool (*CommandPatchFilter)();

struct CommandPatchList
{
	const char* name;
	CommandPatchFilter filter;
	void* pfnCommand;
	void* pfnOld;
};

// Replacement for the original drop_item command handler.
// Calls cContainSys::Remove *after* the call to ThrowObject, to prevent
// PhysRegisterSphere messages being sent for objects that shouldn't send them
// (such as corpses).
void OnDropItem()
{
	int object = g_pInventory->Selection(IO_Item);
	if (!object)
		return;

	if (!*dword_6F55DC || *dword_671B8C == -1 || (DWORD)_GetSimTime() <= *dword_671B8C)
	{
		BOOL noDrop;
		if (!g_pNoDropProperty->Get(object, &noDrop) || noDrop == FALSE)
		{
			object = g_pContainSys->RemoveFromStack(object, 0, 1);
#if 1
			if (ThrowObjectInternal(*_gPlayerObj, object, 0.05f))
				g_pInventory->Remove(object);
#else
			// original behavior of this function
			g_pInventory->Remove(object);
			if (!ThrowObjectInternal(*_gPlayerObj, object, 0.05f))
				g_pInventory->Add(object);
#endif

			if (object == *dword_6F55D4)
				*dword_6F55D4 = 0;
		}
	}
}

void OnUseWeapon(BOOL finishingAttack)
{
	if (g_pOverlayMgr)
	{
		if (!finishingAttack)
		{
			// return if the click was sent to an overlay
			if (g_pOverlayMgr->HandleClick() == true)
				return;
		}
	}

	BOOL bowEquipped = _IsBowEquipped();
	int weaponObj = _GetWeaponObjID(*_gPlayerObj);

	if (!weaponObj)
		return;

	if (finishingAttack == FALSE)
	{
		*lastWeap = _GetSimTime();
		if (!bowEquipped)
		{
			Difficulty_OnMeleeAttack(); // check to see if we need to cancel leaning
			_StartWeaponAttack(*_gPlayerObj, weaponObj);

#ifdef NEW_NETCODE
			SendPlayerAnimation("attack, meleecombat, direction 0", "Player");
#endif
		}
	}
	else if (finishingAttack == TRUE)
	{
		//*lastWeap = -1;
		if (!bowEquipped)
			return _FinishWeaponAction(*_gPlayerObj, weaponObj);
	}

	HandleUseWeapon(finishingAttack);
}

void OnWinMission()
{
	if (!g_pNetMan->AmDefaultHost() || IsStagingMode())
		return;

	IPtr<IQuestData> pQuest = _AppGetAggregated(IID_IQuestData);
	pQuest->Set("MISSION_COMPLETE", 1);
	SendEndMission();

	return _win_mission();
}

void OnSimMenu()
{
	if (IsStagingMode())
		return;

	// Disable all overlays first, before allowing a return to the sim menu
	if (g_pOverlayMgr->AnyEnabled())
	{
		g_pOverlayMgr->DisableAll();
		return;
	}

#if 0
		g_pOverlayMgr->ToggleOverlay("overlay_sim");
#else
		_SwitchToSimMenuMode(1);
#endif
}

void OnObjectives()
{
#if 1
	if (IsStagingMode())
		return;

	if (!g_pOverlayMgr->OverlayEnabled("overlay_objectives"))
		g_pOverlayMgr->DisableAll();

	g_pOverlayMgr->ToggleOverlay("overlay_objectives");
#else
	_SwitchToObjectivesMode(1);
#endif
}

void OnAutomap()
{
	if (IsStagingMode())
		return;

	if (!g_pOverlayMgr->OverlayEnabled("overlay_map"))
		g_pOverlayMgr->DisableAll();

	g_pOverlayMgr->ToggleOverlay("overlay_map");
}

bool OverlayFilter()
{
	return Cfg.GetBool("UseOverlays");
}

CommandPatchList CommandList[] =
{
	{"use_weapon", NULL, OnUseWeapon, 0},
	{"drop_item", NULL, OnDropItem, 0},
	{"win_mission", NULL, OnWinMission, 0},
	{"sim_menu", NULL, OnSimMenu, 0},
	{"objectives", OverlayFilter, OnObjectives, 0},
	{"automap", OverlayFilter, OnAutomap, 0},
};

void PatchCommands(bool patch)
{
	for (int i = 0; i < sizeof(CommandList) / sizeof(CommandPatchList); i++)
	{
		// Call the filter to see whether we should do the patch or not
		if (CommandList[i].filter && CommandList[i].filter() == false)
			continue;

		Command* pCommand = _CommandFind(CommandList[i].name, (int)(strlen(CommandList[i].name)));
		dbgassert(pCommand);

		if (!pCommand)
			continue;

		if (patch)
		{
			assert(CommandList[i].pfnCommand);

			// Copy over the old command handler so we can restore it later
			CommandList[i].pfnOld = pCommand->func;

			// Write in the new handler
			pCommand->func = CommandList[i].pfnCommand;
		}
		else
		{
			assert(CommandList[i].pfnOld);

			// Restore the previous command handler, and make sure that we don't try to do this twice
			pCommand->func = CommandList[i].pfnOld;
			CommandList[i].pfnOld = NULL;
		}
	}
}

// Called when polling for keyboard input. Passes key presses to overlays.
//short HookKeyboardInput(_kbs_event& event, short& cooked, BYTE& P3, int zero)
//{
//	_kb_cook_real(event, cooked, P3, 0);
//	DialogElement* element = GetFocusElement();
//	if (P3 && element)
//	{
//		DbgPrint("Cooked: 0x%x", cooked);
//		//if (cooked == 0x174)
//		//	__debugbreak();
//		char ch = cooked & 0xFE; 
//		if (ch)
//		{
//			switch (element->GetType())
//			{
//			case ElementTypeEdit:
//				if (element->HandleKey(ch))
//					P3 = 0; // don't pass the key back to the game
//
//				break;
//			}
//		}
//	}
//
//	return 0;
//}