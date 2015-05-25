/*************************************************************
* File: SaveManager.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements network-aware saving/loading and limited saves
*************************************************************/

#include "stdafx.h"
#include "Main.h"
#include "SaveManager.h"
#include "Client.h"

SaveManager* g_pSaveMgr;

const int kInfiniteSaves = -1;

SaveManager::SaveManager()
{
	m_lastSelectedSlot = 0;
	m_savesRemaining = kInfiniteSaves;

	_mkdir("netsaves");

	EventManager::RegisterListener(ET_MissionLoaded, this);
}

SaveManager::~SaveManager()
{
	EventManager::UnregisterListener(ET_MissionLoaded, this);
}

void SaveManager::SendLoadMsg()
{
	CNetMsg_LoadGame msg;
	msg.slot = (BYTE)m_lastSelectedSlot;

	g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, sizeof(CNetMsg_LoadGame), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
}

void SaveManager::SendSaveMsg(int slot)
{
	CNetMsg_SaveGame save;

	save.slot = (BYTE)slot;
	g_pDarkNet->Send(ALL_PLAYERS, (void*)&save, sizeof(CNetMsg_SaveGame), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
}

void SaveManager::SetLastSlot(int slot)
{
	m_lastSelectedSlot = slot;
}

const int kSaveUsageDelay = 5000; // Number of frames to delay before using up saves

void SaveManager::DoNetworkSave(int slot)
{
	HRESULT hRes;
	char saveDescription[MAX_SAVE_DESC];
	char fileName[32];

	if (m_savesRemaining != kInfiniteSaves && _GetSimTime() > kSaveUsageDelay)
	{
		if (m_savesRemaining <= 0)
		{
			MessageMgr::Get()->AddLine(true, "No more saves allowed.");
			return;
		}

		m_savesRemaining--;
	}

	g_pNetMan->PreFlush();

	cAnsiStr str;
	sprintf(saveDescription, "short_%d", _GetNextMission());
	str = FetchUIString("titles", saveDescription, "strings");

	// Drop any junk the player might be carrying (has caused problems in the past)
	if (g_pInventory->WieldingJunk())
		HandleDropItem();

	if (slot == QS_SLOT) // quicksave slot
	{
		safe_snprintf(saveDescription, MAX_SAVE_DESC, "%s - %s (Quick)", str.m_pString, g_pNetMan->m_playerName);
		sprintf(fileName, "netsaves\\quick.sav", slot);

		DeleteFile("netsaves\\quick.bak");
		rename("netsaves\\quick.sav", "netsaves\\quick.bak");
	}
	else
	{
		safe_snprintf(saveDescription, MAX_SAVE_DESC, "%s - %s", str.m_pString, g_pNetMan->m_playerName);
		sprintf(fileName, "netsaves\\game%04d.sav", slot);
	}

	hRes = _DarkSaveGameFile(saveDescription, fileName);

	if (hRes == S_OK)
		MessageMgr::Get()->AddLine(true, "Successfully saved game.");
	else
		MessageMgr::Get()->AddLine(true, "Failed to save game.");

	if (m_savesRemaining > 0)
		MessageMgr::Get()->AddLineFormat(false, "%d %s remaining.", m_savesRemaining, m_savesRemaining > 1 ? "saves" : "save");
	else if (m_savesRemaining == 0)
		MessageMgr::Get()->AddLine(false, "No saves remaining.");

	str.Destroy();

	//if (g_Net == STATE_Host)
	//	g_pNetMan->SynchFlush(0, 0);
}

void SaveManager::DoNetworkLoad(int slot)
{
	char fileName[MAX_PATH];

	if (slot == QS_SLOT)
		sprintf(fileName, "netsaves\\quick.sav");
	else
		sprintf(fileName, "netsaves\\game%04d.sav", slot);

	int result = _DarkQuickLoadGameFile(fileName);
	if (result == -1024)
	{
		MessageMgr::Get()->AddLineFormat(true, "Failed to load game (%d).", result);
		return;
	}
	else if (result == -1023)
	{
		_MissionLoopReset(0);
		_UnwindToMissionLoop();
		return;
	}
	else
		MessageMgr::Get()->AddLine(true, "Successfully loaded game.");

	//if (g_Net == STATE_Host)
	//	g_pNetMan->SynchFlush(0, 0);
}

//======================================================================================
// Name: OnLoadRemoteGhosts
//
// Desc: Loads remote ghost data from a save file.
// Passes on calls to the original function and loads new multiplayer-specific data.
//======================================================================================
void SaveManager::OnLoadRemoteGhosts(TagFileRead* tag, unsigned long P2)
{
	// Call original function
	HRESULT hRes = LoadRemoteGhosts(tag, P2);

#ifdef _DEBUG
	assert(hRes != E_FAIL);
#endif

	hRes = tag->OpenBlock(&TimerTag, &T2Version);
	if (hRes == S_OK)
	{
		if (g_pNetMan->AmDefaultHost())
		{
			MPDataBlock data;

			if (tag->Read((char*)&data, sizeof(MPDataBlock)) == sizeof(MPDataBlock))
			{
				Client.SetTimeRemaining(data.timeLeft);
				m_savesRemaining = data.savesLeft;
			}
		}
	}
	//else
		//Log.Print("Failed to read data from save file");
}

//======================================================================================
// Name: OnSaveRemoteGhosts
//
// Desc: Writes remote ghost data to a save file. 
// Complete replacement for the original function with improved error handling, and support for writing new data to
// save files.
//======================================================================================
void SaveManager::OnSaveRemoteGhosts(TagFileWrite* tag, unsigned long P2)
{
	tHashSetHandle hs;
	sGhostRemote* pGhost;
	int object;
	SavedGhostRem ghostSave;
	HRESULT hRes;

	hs.x00 = -1;
	hs.x04 = 0;

	hRes = tag->OpenBlock(GhostRemTag, &T2Version);
	if (hRes == S_OK)
	{
		pGhost = (sGhostRemote*)cHashSetBaseFns::ScanNext(_gGhostRemotes, NULL, &hs);
		while (pGhost)
		{
			object = pGhost->obj;
			if (g_pObjSys->Exists(object) && g_pObjSys->IsObjSavePartition(object, P2))
			{
				ghostSave.objectID = object;
				ghostSave.x04 = pGhost->cfg.detail;
				ghostSave.flags = pGhost->cfg.flags;
				ghostSave.x0c = pGhost->critter.r_state;
				ghostSave.weaponObj = pGhost->weap.weaponObj;
				ghostSave.weaponID = pGhost->weap.nWeapon;

				if (!_PhysObjHasPhysics(object))
					ghostSave.flags |= 0x40000;

				if (Debug.IsFlagSet(DEBUG_GHOSTS))
					Log.Print("Saving ghost for %s", _ObjEditName(object));

				tag->Write((char*)&ghostSave, sizeof(SavedGhostRem));

				if (!(pGhost->cfg.flags & 0xC))
				{
					if (pGhost->critter.pMotCoord)
					{
						if (Debug.IsFlagSet(DEBUG_GHOSTS))
							Log.Print("Saving motion coordinator for: %s", _ObjEditName(object));
						pGhost->critter.pMotCoord->Save(tag);
					}
					else
						Log.Print("SaveRemoteGhosts: invalid motion coordinator for %s, dropping", _ObjEditName(object));
				}
			}

			pGhost = (sGhostRemote*)cHashSetBaseFns::ScanNext(_gGhostRemotes, NULL, &hs);
		}
	}
	else
		Log.Print("%s: OpenBlock failed (%x).", __FUNCTION__, hRes);

	tag->CloseBlock();

	// Write new data to save file
	hRes = tag->OpenBlock(&TimerTag, &T2Version);
	if (hRes == S_OK)
	{
		MPDataBlock data;

		if (Client.IsTimeLimitSet())
		{
			data.timeLeft = Client.GetTimeRemaining();
			data.savesLeft = m_savesRemaining;

			tag->Write((char*)&data, sizeof(MPDataBlock));
		}

		tag->CloseBlock();
	}
	else
		Log.Print("%s: OpenBlock failed (%x).", __FUNCTION__, hRes);
}

void SaveManager::EventHandler(int eventType, void* eventData)
{
	switch (eventType)
	{
	case ET_MissionLoaded:
		// Set up save limits
		int saveLimit = Cfg.GetInt("SaveLimit");
		if (saveLimit > 0)
			m_savesRemaining = saveLimit;

		break;
	}
}

//======================================================================================
// Quicksave and quickload hooks
//======================================================================================

void __stdcall SaveManager::HookQuickSave()
{
	if (!g_pNetMan->IsNetworkGame())
		return _DarkQuickSaveGame();

	if (g_Net == STATE_Host)
	{
		g_pSaveMgr->SendSaveMsg(QS_SLOT);
		return g_pSaveMgr->DoNetworkSave(QS_SLOT);
	}
}

//======================================================================================

void __stdcall SaveManager::HookQuickLoad()
{
	if (!g_pNetMan->IsNetworkGame())
		return _DarkQuickLoadGame();

	if (g_Net == STATE_Host)
	{
		g_pSaveMgr->SetLastSlot(QS_SLOT);
		return g_pSaveMgr->DoNetworkLoad(QS_SLOT);
	}	
}

// Called when reading GHOSTREM block from a tag file
void __stdcall SaveManager::HookLoadRemoteGhosts(TagFileRead* tag, unsigned long P2)
{
	g_pSaveMgr->OnLoadRemoteGhosts(tag, P2);
}

// Called when writing GHOSTREM block to a tag file
void SaveManager::HookSaveRemoteGhosts(TagFileWrite* write, unsigned long P2)
{
	g_pSaveMgr->OnSaveRemoteGhosts(write, P2);
}

//======================================================================================

// Initialization/shutdown
void SaveManagerInit()
{
	if (!g_pSaveMgr)
		g_pSaveMgr = new SaveManager;
}

void SaveManagerTerm()
{
	SAFE_DELETE(g_pSaveMgr);
}