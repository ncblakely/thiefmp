/*************************************************************
* File: Client.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"
#include "Main.h"
#include "Client.h"
#include "PlayerDB.h"
#include "DarkOverlay.h"
#include "Gamesys.h"
#include "Chat.h"
#include "DarkUIUtils.h"

CClient Client;

#define FadeOutTime 3500 // death fade out time (in milliseconds)
#define TimeLimitSyncTime 20.0f // how long to wait for before resynching time limits between players

void CClient::DrawPlayerNames()
{
	if (g_pTraitMan->ObjHasDonor(*_highlit_obj, Gamesys.Arch.MPAvatar))
	{
		if (g_pNetMan->IsNetworkGame())
		{
			int playerNum = g_pNetMan->ObjToPlayerNum(*_highlit_obj);
			if (playerNum)
			{
				TextDrawCentered(g_TextFont, CLR_Red, Players.NameFromIndex(playerNum), 0.5f, 0.5f);
			}
		}

		*_highlit_obj = 0;
		*_g_PickCurrentObj = 0;
		*_frobWorldSelectObj = 0;
	}
}

void CClient::DebugDrawVisibility()
{
	sAIVisibility* vis;
	float alpha;

	if (g_pAIVisibilityProperty->Get(*_highlit_obj, (void**)&vis) && gAlphaRenderProp->Get(*_highlit_obj, &alpha))
	{
		NString str;
		str.Format("Level: %d Exposure: %d Rating %d Transparency %.2f", vis->level, vis->exposureRating, vis->lightRating, alpha);
		TextDrawCentered(g_TextFont, CLR_Red, str, 0.5f, 0.5f);
	}

	if (g_pTraitMan->ObjHasDonor(*_highlit_obj, Gamesys.Arch.MPAvatar))
		*_highlit_obj = 0;
}

void CClient::DebugDrawNetStats()
{
	if (g_pNetMan->IsNetworkGame())
	{
		QueueStats stats;
		NString str;

		g_pDarkNet->GetSendQueueStats(stats);

		if (!g_pNetMan->AmDefaultHost())
		{
			DPN_CONNECTION_INFO ci;

			ci.dwSize = sizeof(DPN_CONNECTION_INFO);

			HRESULT hRes = g_pDarkNet->GetHostConnectionInfo(&ci);
			if (FAILED(hRes))
				return;

			if (!g_pDarkNet)
				return;

			int timeouts = ci.dwMessagesTimedOutHighPriority + ci.dwMessagesTimedOutLowPriority + ci.dwMessagesTimedOutNormalPriority;

			str.Format("Ping: %d\nPackets dropped: %d\nTimeouts: %d\nGuaranteed sends: %d\nNon-guaranteed sends: %d\nQueue: %d\nQueue peak: %d\n", ci.dwRoundTripLatencyMS, ci.dwPacketsDropped, 
				timeouts, ci.dwPacketsSentGuaranteed, ci.dwPacketsSentNonGuaranteed, stats.queued, stats.peak);
		}
		else
			str.Format("Queue %d\nQueue peak: %d", stats.queued, stats.peak);

		TextDrawCentered(g_TextFont, CLR_Red, str, 0.98f, 0.01f);
	}
}

void CClient::DebugDrawMiscStats()
{
	mxs_vector vec;
	NString str;
	float totalSpeed;
	Position* pos;

	pos = _ObjPosGet(*_gPlayerObj);
	_PhysGetVelocity(*_gPlayerObj, & vec);
	totalSpeed = sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
	sGhostLocal* pGhost = _GhostGetLocal(*_gPlayerObj);
	
	str.Format("ftime %d\nstime %d\nforward %0.4f\nside %0.4f\noverall %0.4f\n[%0.4f %0.4f %0.4f] \ndelta %f\nfps %0.2f\nanm %d aflags %x", _GetSimFrameTime(), _GetSimTime(), g_pPlayerMode->m_forwardSpeed, g_pPlayerMode->m_sidestepSpeed, 
		totalSpeed, pos->vec.x, pos->vec.y, pos->vec.z, g_DeltaFrame.Get(), 1.0f / g_DeltaFrame.Get(), pGhost ? pGhost->playing.schema_idx : 0, pGhost ? pGhost->playing.motion_num : 0);

	TextDrawCentered(g_TextFont, CLR_Red, str, 0.0f, 0.0f);
}

extern bool g_bNetStats;
extern bool g_bVisStats;
extern bool g_bMiscStats;

void CClient::RenderOverlays()
{
	if (g_bNetStats)
		DebugDrawNetStats();

	if (g_bVisStats)
		DebugDrawVisibility();
	else
		DrawPlayerNames();

	if (g_bMiscStats)
		DebugDrawMiscStats();
}

void CClient::LoadMission(int missionID)
{
	if (missionID > 0)
		_SetNextMission(missionID);

	_MissionLoopReset(1);
	_UnwindToMissionLoop();
}

bool CClient::MissionStarted()
{
	return m_missionStarted;
}

//======================================================================================
// Name: OnDeath
//
// Desc: Player death callback, inserted by cPlayerMode::InstallOnDeathCallback.
//======================================================================================
void CClient::StaticOnDeath()
{
	return Client.OnDeath();
}

void CClient::OnDeath()
{
	if (g_pNetMan->IsNetworkGame())
	{
		const SessionInfo& info = g_pDarkNet->GetSessionInfo();
		if (info.respawnEnabled)
		{
			// fade the player's screen, and initialize the respawn timer
			m_respawnTimer = info.respawnDelay;
			m_playerIsDead = true;
			//m_respawnTimer = (uint64)(_GetSimTime() + (info.respawnDelay * 1000));
			StartFadeOut(FadeOutTime, 0, 0, 0);
		}

		if (g_pOverlayMgr)
			g_pOverlayMgr->DisableAll();

		// notify the host that we've died
		//m_PacketHandler.SendPlayerDeath(g_pNetMan->MyPlayerNum());

		// remove AIAttack links to the dead player
		RemoveRelationByID(*_gPlayerObj, g_pLinkMan->GetRelationNamed("AIAttack")->GetID());

		if (int weapon = g_pInventory->Selection(IO_Weapon))
			_UnEquipWeapon(*_gPlayerObj, weapon);

		// notify the player's ghost
		_GhostNotify(*_gPlayerObj, GHOST_ACTION_DYING);
	}
}

void CClient::OnPlayerCreate()
{
	__asm pushad

	if (!g_pNetMan->IsNetworkGame())
	{
		_DarkPrepLevelForDifficulty();
		_PlayerCreate();
	}

	_DarkLootPrepMission();

	__asm popad
}
//======================================================================================
// Name: OnJump
//
// Desc: Checks to see if the player is dead when the jump key is pressed, and ends the mission normally if not in a
// network game.
//======================================================================================
int CClient::OnJump()
{
	if (_GetPlayerMode()== MODE_Dead)
	{
		if (!g_pNetMan->IsNetworkGame())
			_UnwindToMissionLoop();
		return 1;
	}
	else
		return 0;
}

void __stdcall CClient::StartGameModeCallback(DWORD P1)
{
	if (g_pNetMan->IsNetworkGame())
	{
		cPlayerModeFns::InstallOnDeathCallback(g_pPlayerMode, NULL, &CClient::StaticOnDeath);

		Client.SetSimRunning(true);

		// Reset cash bonus for the next mission
		if (g_pDarkNet->GetSessionInfo().startingLoot)
			_config_set_int("cash_bonus", 0);

		if (g_Net == STATE_Host)
		{
			assert(g_pNetMan->AmDefaultHost());

			if (Client.IsTimeLimitSet())
			{
				SendTimeLimit();
			}
		}
		// _g_SimRunning?

		EventManager::Broadcast(ET_GameMode, NULL);
		//Client.SendEnteredWorld();
	}

	// This needs to be called even in single player games
	//DarkChatInit();

	_dark_start_gamemode(P1);
}

bool CClient::IsSimRunning()
{
	return m_bSimRunning;
}

void CClient::SetSimRunning(bool bRunning)
{
	m_bSimRunning = bRunning;
}

void CClient::RespawnPlayer()
{
	mxs_vector vec;
	int hitPoints;
	int startPoint;

	_ObjGetMaxHitPoints(*_gPlayerObj, &hitPoints);
	hitPoints = (int)(hitPoints * 0.47f);

	ShockSetMode(0, 0);
	_mx_zero_vec(vec);
	_PhysSetRotationalVelocity(*_gPlayerObj, &vec);

	g_cDamageSrvScriptService->Resurrect(object(*_gPlayerObj), 0);
	_ObjSetHitPoints(*_gPlayerObj, hitPoints);
	_GhostNotify(*_gPlayerObj, GHOST_ACTION_REVIVE);

	// get the starting point's location, and set the player's location to it
	startPoint = PlayerFactoryHook();
	_ObjPosSetLocation(*_gPlayerObj, _ObjPosGet(startPoint));

	StartFadeIn(2000, 0, 0, 0);

	m_respawnTimer = 0;
	m_playerIsDead = false;
}

void CClient::OnLoadWorld()
{
	m_respawnTimer = 0.0f;
	m_endGameTimer = 0.0f;
	m_timeLimit = 0.0f;

	m_playerIsDead = false;
	m_numEnteredWorld = 0;
	m_missionStarted = true;

	if (g_pDarkNet)
		g_pNetMan->PreFlush();

	MessageMgr::Get()->ClearMessages();

	if (Debug.IsFlagSet(DEBUG_GENERAL))
		ConPrintF("%s: client vars reset.", __FUNCTION__);

	if (Cfg.GetInt("DisableNetPhysCap"))
		*net_cap_physics = 0;
}

float CClient::GetTimeRemaining()
{
	return m_timeLimit;
}

void CClient::SetTimeRemaining(float timeLimit)
{
	if (timeLimit > 0.0f)
	{
		m_timeLimit = timeLimit;
		m_timeLimitActive = true;
	}
}

bool CClient::IsTimeLimitSet()
{
	return (m_timeLimit > 0);
}

void CClient::StartFadeIn(unsigned long fadeTime, BYTE r, BYTE g, BYTE b)
{
	if (fadeTime)
	{
		cPlayerMode* pMode = g_pPlayerMode;

		pMode->fadeR = r;
		pMode->fadeG = g;
		pMode->fadeB = b;
		pMode->fadeTime = (float)-1.0 / fadeTime;
		pMode->tmFadeStart = _GetSimTime();
	}
}

void CClient::StartFadeOut(unsigned long fadeTime, BYTE r, BYTE g, BYTE b)
{
	if (fadeTime)
	{
		cPlayerMode* pMode = g_pPlayerMode;

		pMode->fadeR = r;
		pMode->fadeG = g;
		pMode->fadeB = b;
		pMode->fadeTime = (float)1.0 / fadeTime;
		pMode->tmFadeStart = _GetSimTime();
	}
}

bool CClient::IsClient()
{
	return (g_pNetMan->IsNetworkGame() && !g_pNetMan->AmDefaultHost());
}

bool CClient::IsHost()
{
	return (g_pNetMan->IsNetworkGame() && g_pNetMan->AmDefaultHost());
}

bool CClient::GameIsEnding()
{
	return (m_endGameTimer != 0.0f);
}

void CClient::StartEndGameTimer(float endTimeSeconds, EndReason er)
{
	switch (er)
	{
	case ER_HostLeft:
		MessageMgr::Get()->AddLine(true, "Host has left the game. Game ending..."); break;
	case ER_Kicked:
		MessageMgr::Get()->AddLine(true, "You have been removed from the game."); break;
	case ER_KickedCrc:
		MessageMgr::Get()->AddLine(true, "You have been removed from the game: your game files do not match the host's."); break;
	case ER_TimeExpired:
		MessageMgr::Get()->AddLine(true, "Time limit reached. Game ending..."); break;
	}

	m_endGameTimer = endTimeSeconds;
	m_gameIsEnding = true;
}

void CClient::Update()
{
	if (_SimStateCheckFlags(0x8000)) // is the sim running?
	{
		float delta = g_DeltaFrame.Get();

		if (g_pNetMan->IsNetworkGame())
		{
			if (IsSimTimePassing())
			{
				// Check up on the time limit (if we have one)
				if (m_timeLimitActive)
				{
					m_timeLimit -= delta;

					if (m_timeLimit <= 0)
					{
						if (g_pNetMan->AmDefaultHost())
						{
							if (!m_gameIsEnding)
								StartEndGameTimer(EndGameTime, ER_TimeExpired);
						}

						m_timeLimit = 0;
					}

					// Resync time limits for clients occasionally
					if (g_pNetMan->AmDefaultHost())
					{
						m_timeLimitSyncTimer -= delta;
						if (m_timeLimitSyncTimer <= 0.0f)
						{
							SendTimeLimit();
							m_timeLimitSyncTimer = TimeLimitSyncTime;
						}
					}
				}

				// Check up on respawning
				if (m_playerIsDead)
				{
					m_respawnTimer -= delta;
					if (m_respawnTimer <= 0)
						RespawnPlayer();
				}
			}

			RenderOverlays();
			UpdateTransparency();
		}

		if (m_gameIsEnding)
		{
			m_endGameTimer -= delta;
			if (m_endGameTimer <= 0.0f)
			{
				// End the mission, and if we're the host inform the clients about it
				if (!g_pNetMan->AmDefaultHost() && IsStagingMode())
				{
					StagingClose();
				}
				else
				{
					_UnwindToMissionLoop();
					SendEndMission();
				}

				m_gameIsEnding = false;
				m_endGameTimer = 0;
			}
		}
	}
}

void CClient::UpdateTransparency()
{
	if (!g_pDarkNet)
		return;

	if (!g_pDarkNet->GetSessionInfo().playerTransparency)
		return;

	int players[8];
	GetPlayers(players);

	float visCapMin = Cfg.GetFloat("VisCapMin");
	float cfgDistanceMod = Cfg.GetFloat("DistanceModifier");

	for (int i = 0; i < 8; i++)
	{
		if (players[i] && players[i] != *_gPlayerObj)
		{
			sGhostRemote* ghost;
			sAIVisibility* vis;
			float alpha, distanceMod;

			ghost = _GhostGetRemote(players[i]);
			if (!ghost)
				continue;

			float distance = _mx_dist_vec(&_ObjPosGet(*_gPlayerObj)->vec, &_ObjPosGet(players[i])->vec);
			distance -= cfgDistanceMod;
			distanceMod = distance * 0.02f;

			if (!g_pAIVisibilityProperty->Get(players[i], (void**)&vis))
				continue; 	// Skip if we can't get the visibility prop

			alpha = (float)(vis->lightRating * 0.04f - distanceMod);
			if (alpha > 1.0f)
				alpha = 1.0f;
			else if (alpha < visCapMin)
				alpha = visCapMin;

			gAlphaRenderProp->Set(players[i], alpha);
			gAlphaRenderProp->Set(ghost->weap.weaponObj, alpha);
		}
	}
}

void CClient::EventHandler(int eventType, void* eventData)
{
	//case ET_MissionLoaded:
	//	m_missionStarted = true;
}