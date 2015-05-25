/*************************************************************
* File: AI.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: General utility functions for AI
*************************************************************/

#include "stdafx.h"

#include "Imports.h"

//======================================================================================
// Name: EquipAIWeaponHook
//
// Desc: Prevents EquipAIWeapon from being called for players.
//======================================================================================
int __stdcall EquipAIWeaponHook(int objectID, DWORD P1, DWORD P2)
{
	if (!_IsAPlayer(objectID))
		return _EquipAIWeapon(objectID, P1, P2);
	else
		return 1;
}

int __stdcall OnIsValidTarget(int object)
{
	int retval;
	DWORD thisObj;
	__asm mov [thisObj], ecx

	// prevent AI creatures from acquiring a dead player as a target
	if (_IsAPlayer(object))
	{
		int hitPoints = 0;

		if (_ObjGetHitPoints(object, &hitPoints))
		{
			if (hitPoints <= 0)
				return 0;
		}
	}

	// pass this object on to IsValidTarget if this isn't a player or the player is alive
	__asm
	{
		mov ecx, [thisObj]
		push [object]
		call cAICombat::IsValidTarget
		mov [retval], eax
	}

	return retval;
}

namespace AITools
{

bool SetAIAwareness(IAI* pAI, int objAwareOf, int awarenessLevel)
{
	if (pAI)
	{
		IAISenses* pAISenses = pAI->AccessSenses();

		if (pAISenses)
		{
			sAIAwareness* pAwareness;

			long handle = cAISensesFns::GetAwarenessLink(pAISenses, NULL, objAwareOf, &pAwareness);

			pAwareness->level = awarenessLevel;

			cAISensesFns::SetAwareness(pAISenses, NULL, pAwareness->object, handle, pAwareness);

			pAI->Release();
			return true;
		}
		else
		{
			pAI->Release();
			return false;
		}

		pAI->Release();
	}

	return false;
}

bool SetAIAwareness(int objAI, int objAwareOf, int awarenessLevel)
{
	IAI* pAI = g_pAIMan->GetAI(objAI);

	return SetAIAwareness(pAI, objAwareOf, awarenessLevel);
}

}