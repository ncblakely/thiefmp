/*************************************************************
* File: DarkPanels.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Hooks menu button click callbacks.
*************************************************************/

#include "stdafx.h"
#include "Imports.h"
#include "Client.h"
#include "MultiUI.h"

void __fastcall MainMenuBlistHook(cDarkPanel* panel, int, unsigned short P1, int selection)
{
	if (!(P1 & 0x4))
		return;

	switch (selection)
	{
	case MainMenu_NewGame:
		if (Client.IsClient())
			return;
		else
			return _SwitchToNewGameMode(1);
	case MainMenu_LoadGame:
		if (Client.IsClient())
			return;
		else
			return _SwitchToLoadGameMode(1);
	case MainMenu_Options:
		return _SwitchToOptionsMode(1);
	case MainMenu_Credits:
		return _PushMovieOrBookMode("credits");
	case MainMenu_Intro:
#ifndef _RELEASE
		panel->m_panelMode->Exit();
		//return SwitchToMultiplayUIMode(1);
		//return _SwitchToShockMultiplayUIMode(0);
#else
		return _PushMovieOrBookMode("intro");
#endif
	case MainMenu_Quit:
		return _quit_game();
	}
}

void __fastcall SimMenuBlistHook(cDarkPanel* pPanel, int, unsigned short P1, int selection)
{
	if (!(P1 & 0x4))
		return;

	switch (selection)
	{
	case 0:
		pPanel->m_panelMode->AddRef();
		pPanel->m_panelMode->Exit();
		if (pPanel->m_panelMode)
			pPanel->m_panelMode->Release();
		return;
	case 1:
		return _SwitchToOptionsMode(1);
	case 2:
		if (Client.IsClient())
			return;
		else
			return _SwitchToSaveGameMode(1);
	case 3:
		if (Client.IsClient())
			return;
		else
			return _SwitchToLoadGameMode(1);
	case 4:
		return _SwitchToDarkAutomapMode(1);
	case 5:
		return _SwitchToObjectivesMode(1);
	case 6:	// quit button
		_MissionLoopReset(0);	
		_UnwindToMissionLoop();
		if (g_pNetMan->IsNetworkGame())
			g_pNetMan->Leave();
		return;
	case 7: // restart button
		if (!Client.IsClient())
		{
			_MissionLoopReset(2);
			_UnwindToMissionLoop();
			SendEndMission();
		}
		return;
	}
}

void __fastcall DebriefBlistHook(cDarkPanel* pPanel, int, unsigned short P1, int selection)
{
	if (!(P1 & 0x4))
		return;

	switch (selection)
	{
	case 1:
		if (Client.IsClient())
			return;
	default:
		cDebrief::OnButtonList(pPanel, NULL, P1, selection);
	}
}