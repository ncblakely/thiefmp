/*************************************************************
* File: DarkGameSrv.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements VTable hooks of cDarkGameSrv methods.
*************************************************************/

#include "stdafx.h"
#include "Main.h"
#include "Client.h"

namespace HookDarkGameSrv
{

long __stdcall EndMission(DWORD thisObj)
{
	if (g_pNetMan->IsNetworkGame())
	{
		if (!g_pNetMan->AmDefaultHost())
			return 0; // The host now notifies clients when the mission has ended
		else
		{
			SendEndMission();
		}
	}

	return cDarkGameSrv::EndMission(thisObj);

	//if (g_pNetMan->IsNetworkGame())
	//{
	//	else
	//	{
	//		if (_GetPlayerMode() == MODE_Dead)
	//		{
	//			IQuestData* pQD = (IQuestData*)_AppGetAggregated(IID_IQuestData);

	//			int completed = pQD->Get("MISSION_COMPLETE");
	//			pQD->Release();

	//			 // If convict.osm is trying to end the mission because we've died and the mission still isn't complete, return
	//			if (completed == 0)
	//				return 0;

	//			// If the mission has actually been completed, end it normally
	//		}

	//		// we're the host of the game, so we need to inform the clients that the mission has ended
	//		CNetMsg_EndMission msg;

	//		if (g_pDarkNet)
	//			g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, sizeof(CNetMsg_EndMission), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
	//	}
	//}

	//return cDarkGameSrv::EndMission(thisObj);
}

long __stdcall FadeToBlack(DWORD thisObj, float fadeTime)
{
	// If this is a network game, the client instance should handle the on-death fadeout
	if (g_pNetMan->IsNetworkGame() && _GetPlayerMode() == MODE_Dead)
		return 0;
	else
		return cDarkGameSrv::FadeToBlack(thisObj, fadeTime);
}

} // end HookDarkGameSrv namespace