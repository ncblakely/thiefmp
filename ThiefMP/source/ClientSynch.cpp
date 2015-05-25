/*************************************************************
* File: ClientSynch.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Handles client synchronization during level loading
*************************************************************/

#include "stdafx.h"
#include "Main.h"
#include "Player.h"
#include "Client.h"
#include "SaveManager.h"

cLoadingSaveGameFns LoadingSaveGame;

#define STATE_CONTINUE 0
#define STATE_LOADING 1
#define STATE_DONE 2

//#define IMMEDIATE_LOADS

extern bool g_bNeedSnapshot;

namespace HookLoading
{

void __fastcall OnLoopMsg(cLoading* pLoading, int, int flags, tLoopMessageData__* loopMsg)
{
	int networking = g_pNetMan->Networking();

	switch (flags)
	{
		case kMsgBeginFrame:
		case kMsgNormalFrame:
		if (!networking && g_pNetMan->IsNetworkGame())
		{
			if (pLoading->m_loadState == STATE_CONTINUE)
			{
				if (!g_pNetMan->AmDefaultHost())
					pLoading->m_loadState = STATE_LOADING; // skip straight to loading if we're not the host
			}
			else if (pLoading->m_loadState == STATE_LOADING)
			{
				if (Client.IsSimRunning())
					Client.SetSimRunning(false);
			}
			else if (pLoading->m_loadState == STATE_DONE)
			{
				if (!*cNetManager::gm_bSynchronizing)
				{

					if (g_pNetMan->IsNetworkGame())
					{
						//_DarkPrepLevelForDifficulty();
						if (!*_gPlayerObj)
							_PlayerCreate();
					}

					// Begin synchronization
					g_pNetMan->m_bShouldSendPlayerContents = TRUE;
					g_pNetMan->StartSynch();

					pLoading->m_Elements[0].elementText = "Synching...";
					_region_expose(_LGadCurrentRoot(), &pLoading->m_Rects[1]);

					if (g_pDarkNet->GetSessionInfo().noLoadoutScreen)
						*pMissFlags |= 0x4;

					if (g_Net == STATE_Host)
					{
						Client.SetTimeRemaining(Cfg.GetInt("TimeLimit") * 60.0f);

#ifndef IMMEDIATE_LOADS
						if (g_pNetMan->AmDefaultHost())
						{
							IPtr<IQuestData> pQuest = _AppGetAggregated(IID_IQuestData);

							CNetMsg_StartMission msg;
							msg.difficulty = pQuest->Get("Difficulty");
							msg.missionID = _GetNextMission();

							g_pDarkNet->GetSessionManager()->UpdateSessionInfo();

							g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, sizeof(CNetMsg_StartMission), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
						}
#endif

#ifdef ALLOW_LATE_JOINS
						if (g_bNeedSnapshot)
						{
							CNetMsg_RequestSnapshot msg;

							DbgPrint("Requesting snapshot");

							g_pNetMan->m_numStartedSynch = g_pNetMan->NumPlayers(); // 8/8/10 //g_pDarkNet->GetSessionManager()->GetApplicationDesc()->dwCurrentPlayers - 1;
							DbgPrint("Other players: %d", g_pNetMan->m_numStartedSynch);

							cNetManagerFns::_LoopFunc((void*)0x006C9420, kMsgBeginFrame, NULL);
							cNetManagerFns::_LoopFunc((void*)0x006C9420, kMsgEndFrame, NULL);

							g_pDarkNet->SendToHost((void*)&msg, sizeof(CNetMsg_RequestSnapshot), DPNSEND_GUARANTEED);

							g_bNeedSnapshot = false;
						}
#endif

						EventManager::Broadcast(ET_MissionLoaded, NULL);
					}
				}

				return; // Return without passing over to the original function until we're done synchronizing
			}
		}
		default:
			cLoadingFns::OnLoopMsg(pLoading, NULL, flags, loopMsg);
	}
}

void __fastcall DoFileLoad(cLoading* pLoading, int, ITagFile* pTag)
{
#ifdef IMMEDIATE_LOADS
	if (g_pNetMan->AmDefaultHost())
	{
		IPtr<IQuestData> pQuest = _AppGetAggregated(IID_IQuestData);

		CNetMsg_StartMission msg;
		msg.difficulty = pQuest->Get("Difficulty");
		msg.missionID = _GetNextMission();

		g_pDarkNet->GetSessionManager()->UpdateSessionInfo();

		g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, sizeof(CNetMsg_StartMission), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
	}
#endif

	cLoadingFns::DoFileLoad(pLoading, NULL, pTag);

	if (g_pNetMan->IsNetworkGame())
	{
		_DarkPrepLevelForDifficulty();
	}

	//if (g_pNetMan->IsNetworkGame())
	//{
	//	_DarkPrepLevelForDifficulty();
	//	if (!*_gPlayerObj)
	//		_PlayerCreate();
	//}
}

} // HookLoading

namespace HookLoadingSaveGame
{

void __fastcall OnLoopMsg(cLoadingSaveGame* pLoadingSaveGame, int, int flags, tLoopMessageData__* loopMsg)
{
	if (!g_pNetMan->Networking() && g_pNetMan->IsNetworkGame())
	{
		if (pLoadingSaveGame->m_loadState == STATE_LOADING)
		{
			if (Client.IsSimRunning())
				Client.SetSimRunning(false);
		}
		else if (pLoadingSaveGame->m_loadState == STATE_DONE)
		{
			if (!*cNetManager::gm_bSynchronizing)
			{
				// Begin net manager synchronization
				g_pNetMan->StartSynch();

				if (g_Net == STATE_Host)
					g_pSaveMgr->SendLoadMsg();
			}

			return;
		}
	}

	LoadingSaveGame.OnLoopMsg(pLoadingSaveGame, NULL, flags, loopMsg);
}

} // HookLoadingSaveGame