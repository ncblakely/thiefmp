/*************************************************************
* File: ScriptService.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"
#include "Main.h"
#include "Client.h"
#include "Ghost.h"
#include "Callbacks.h"

long __stdcall HookBowSrv::OnStartAttack()
{
	if (g_Net) // notify the player ghost that it has begun firing a ranged weapon
		_GhostNotify(*_gPlayerObj, GHOST_ACTION_FIRING);

	return cBowSrv::StartAttack(this);
}

long __stdcall HookBowSrv::OnFinishAttack()
{
	if (g_Net)	// set the standing animation once the player has finished firing
	{
		//GhostSetLocalAnimation(*_gPlayerObj, 357);
		SendPlayerStopAnimation();
	}

	return cBowSrv::FinishAttack(this);
}

namespace HookPickLockSrv
{

int __stdcall StartPicking(DWORD thisObj, int playerObject, int lockpickObject, int lockedObject)
{
	if (!g_pObjNet->ObjHostedHere(lockedObject))
		g_pObjNet->ObjTakeOver(lockedObject);

	return cPickLockSrv::StartPicking(thisObj, playerObject, lockpickObject, lockedObject);
}

int __stdcall FinishPicking(DWORD thisObj, int lockpickObject)
{
	return cPickLockSrv::FinishPicking(thisObj, lockpickObject);
}

} // HookPickLockSrv

namespace HookDebugScrSrv
{

long __stdcall MPrint(DWORD* thisObj, cScrStr& str1, cScrStr& str2, cScrStr& str3, cScrStr& str4, cScrStr& str5, cScrStr& str6, cScrStr& str7, cScrStr& str8)
{
	if (Cfg.GetInt("ShowScriptDebug"))
		ConPrintF("%s%s%s%s%s%s%s%s", str1.m_string, str2.m_string, str3.m_string, str4.m_string, str5.m_string, str6.m_string, str7.m_string, str8.m_string);

	return 0;
}
} // HookDebugScrSrv
