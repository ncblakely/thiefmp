/*************************************************************
* File: LoopClients.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Hooks LoopClient update functions.
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "Client.h"
#include "LoopClients.h"
#include "NetProperties.h"
#include "DarkOverlay.h"
#include "Gamesys.h"
#include "Chat.h"

void InitGame();
void TermGame();

int __stdcall SimulationLoopHook(DWORD P1, eLoopMsgType type, tLoopMessageData__* pMsgData)
{
	switch (type)
	{
	case kMsgSave:
		//_config_load("multi.cfg");
		InitGame();
		_dark_init_game();

		return 0;
	case kMsgUserReserved1:
		TermGame();
		_dark_term_game();
		return 0;
	default:
		return DarkSimulationLoop(P1, type, pMsgData);
	}
}

//======================================================================================
// Name: RenderLoopHook
//
// Desc: Draws chat and limits framerates to (close to) the value set in ThiefMP.ini.
//======================================================================================
int __stdcall RenderLoopHook(DWORD P1, eLoopMsgType type, tLoopMessageData__* pMsgData)
{
	switch (type)
	{
	case kMsgBeginFrame:
		if (_SimStateCheckFlags(0x8000))
		{
			if (g_pOverlayMgr)
			{
				_ScrnLockDrawCanvas();
				g_pOverlayMgr->OnFrame();
				_ScrnUnlockDrawCanvas();
			}

			// Perform per-frame updates
			Client.Update();
			MessageMgr::Get()->Update();
		}
		break;
	case kMsgFrameReserved2:
	case kMsgSuspendMode:
			if (g_pOverlayMgr)
				g_pOverlayMgr->InitAll();

			DarkChatInit();
		break;
	case kMsgEnterMode:
	case kMsgResumeMode:
			if (g_pOverlayMgr)
				g_pOverlayMgr->TermAll();

			DarkChatTerm();
		break;
	}


	return DarkRenderingLoopClient(P1, type, pMsgData);
}

int __stdcall SimLoopClientHook(DWORD P1, eLoopMsgType type, tLoopMessageData__* pMsgData)
{
	if (g_pNetMan && g_pNetMan->IsNetworkGame() 
		|| Client.GameIsEnding()) // 7/25/10 - keep FPS limit enabled when game ending
	{
		switch (type)
		{
		case kMsgBeginFrame:
			g_DeltaFrame.Calculate(g_pNetMan->IsNetworkGame() ? true : false);
			if (pMsgData)
			{
				((sSimLoopMessageData*)pMsgData)->frameTime = (int)(1000.0f * g_DeltaFrame.Get());
			}
	default:
		break;
		}
	}

	return SimLoopClient(P1, type, pMsgData);
}

int __stdcall GameModeLoopClientHook(DWORD P1, eLoopMsgType type, tLoopMessageData__* pMsgData)
{
	int ret = GameModeLoopClient(P1, type, pMsgData);

	switch (type)
	{
	case kMsgFrameReserved2:
		// Hack: enable mouse movement events when entering sim with the staging overlay on
		if (g_pNetMan->IsNetworkGame() && g_pOverlayMgr->OverlayEnabled("overlay_staging"))
		{
			g_pInputBinder->SetValidEvents(-9);
		}
		break;
	}

	return ret;
}

int __stdcall ObjSysLoopClientHook(DWORD P1, eLoopMsgType type, tLoopMessageData__* pMsgData)
{
	int ret = ObjectSystemLoopClient(P1, type, pMsgData);

	// this rather messy code determines whether a gamesys is currently being loaded, and if so initializes the arch cache
	// see 004212C0 in thief2.exe - if _PhysRefSystemResize is called from the object system loop client, a gamesys load is underway

	if (pMsgData && type == kMsgModeReserved3)
	{
		DWORD msgFlags = pMsgData->flags;
		if ((msgFlags & 0xFF) == 1 && msgFlags & 0x20000)
		{
			EventManager::Broadcast(ET_PreLoadWorld, NULL);

			Client.OnLoadWorld();

			EventManager::Broadcast(ET_PostLoadWorld, NULL);

			if (g_pNetMan->IsNetworkGame())
				Gamesys.InitializeArchCache(pMsgData->flags);
		}
	}

	return ret;
}