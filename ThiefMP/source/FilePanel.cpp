/*************************************************************
* File: FilePanel.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/


#include "stdafx.h"
#include "Main.h"
#include "SaveManager.h"

namespace HookDarkFilePanel
{

const char* __stdcall SlotFileName(int slot)
{
	g_pSaveMgr->SetLastSlot(slot);

	if (g_pNetMan->IsNetworkGame())
	{
		if (slot == QS_SLOT)
			return "netsaves\\quick.sav";
		else
		{
			sprintf((char*)0x006F5748, "netsaves\\game%04d.sav", slot);
			return (char*)0x006F5748;
		}
	}
	else
	{
		if (slot == QS_SLOT)
			return "saves\\quick.sav";
		else
		{
			sprintf((char*)0x006F5748, "saves\\game%04d.sav", slot);
			return (char*)0x006F5748;
		}
	}
}

} // end HookDarkFilePanel

namespace HookSaveFilePanel
{

void __fastcall DoFileOp(DWORD* thisObj, int, int slot)
{
	if (g_pNetMan->IsNetworkGame())
	{
		if (g_pNetMan->AmDefaultHost())
			g_pSaveMgr->SendSaveMsg(slot);
		else
			return;
	}

	cSaveFilePanelFns::DoFileOp(thisObj, NULL, slot);
}

} // end HookSaveFilePanel
