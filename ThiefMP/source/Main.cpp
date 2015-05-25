/*************************************************************
* File: Main.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#define _DEFINE_OFFSETS
#include "Main.h"
#include "DarkWinGui.h"
#include "GlobalServer.h"

void ValidateScriptModules();
void InitHook();
void ShutdownHook();

//======================================================================================
// Globals
//======================================================================================
HINSTANCE g_hInstDLL = NULL;

Logger Log;
CCriticalSection g_CoreThread("core");

//====================================================================================== 
// DllMain
//====================================================================================== 
BOOL APIENTRY DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		g_hInstDLL = hInstDLL;
		DisableThreadLibraryCalls(hInstDLL);

		InitCommonControls();
		Timing::Init();
		InitHook();

		// Open log file and overwrite previous
		Log.Open("ThiefMP.log", true);
		Log.EnableTimestamps(false);

		// Ensure that the 1.18 version of gen.osm is present
		ValidateScriptModules();

		// Fixup imported function pointers (obsolete)
		FixupImports();
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
		ShutdownHook();

   return TRUE;
}

void __stdcall PumpEventsHook(DWORD* thisObj, int P1, enum eWinPumpDuration pumpDuration)
{
	if (UI)
		UI->Update();

	if (g_pBrowser)
		g_pBrowser->Update();

	if (g_pDarkNet)
		g_pDarkNet->Update();

	PumpEvents(thisObj, P1, pumpDuration);
}