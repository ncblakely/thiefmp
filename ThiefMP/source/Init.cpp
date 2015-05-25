/*************************************************************
* File: Init.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Init.h"
#include "Engine\inc\EventManager.h"
#include "MessageManager.h"
#include "DarkUIUtils.h"

// Performs basic initialization of T2 MP
void InitHook()
{
	PatcherInit();
	StringHashInit();

	EventManager::Initialize();

	// Edit some property descriptions before they're initialized by the game
	PatchPropDescs();

	// Read settings from .ini file
	ConfigInit();

	// Initialize debug manager
	DebugInit();

	// Start up win32 interface
	WinGuiInit();
}

// Called before dark_init_game
void InitGame()
{
	NetPropertiesInit();
	GamesysInit();
	SaveManagerInit();
	DarkUIUtilsInit();
	OverlayInit();
}

// Called before dark_term_game
void TermGame()
{
	NetPropertiesTerm();
	GamesysTerm();
	SaveManagerTerm();
	MessageMgr::Shutdown();
	DarkUIUtilsTerm();
	OverlayTerm();
}

void ShutdownHook()
{
	WrapperTerm();
	WinGuiTerm();
	BrowserTerm();
	PatcherTerm();
	StringHashTerm();

	EventManager::Shutdown();
}