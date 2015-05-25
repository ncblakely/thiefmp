/*************************************************************
* File: Console.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements console command functionality.
*************************************************************/

#include "stdafx.h"

#include "Engine\inc\Crc32.h"

#include "Main.h"
#include "DarkWinGui.h"
#include "GlobalServer.h"
#include "Client.h"
#include "SaveManager.h"
#include "DarkOverlay.h"
#include "Chat.h"

extern CCriticalSection g_CoreThread;
extern bool g_bEchoDebug;

void AddConsoleMessage(const char* messageText)
{
	ConPrintF(messageText);
}

void ConPrintF(const char* str, ...)
{
	g_CoreThread.Lock();

	if (Cfg.GetInt("MenuEnabled"))
	{
		char buffer[4096];
		va_list marker;
		va_start(marker, str);

		_vsnprintf(buffer, sizeof(buffer) - 3, str, marker);

		strcat(buffer, "\r\n\0");

		if (UI && UI->m_pConsole)
			UI->m_pConsole->PostMessage(&buffer[0]);

		if (g_bEchoDebug)
			MessageMgr::Get()->AddLine(false, buffer);

		va_end(marker);
	}

	g_CoreThread.Unlock();
}

void ConPrintEchoF(const char* str, ...)
{
	if (Cfg.GetInt("MenuEnabled"))
	{
		char buffer[4096];
		va_list marker;
		va_start(marker, str);

		_vsnprintf(buffer, sizeof(buffer) - 3, str, marker);

		strcat(buffer, "\r\n\0");

		if (UI && UI->m_pConsole)
			UI->m_pConsole->PostMessage(&buffer[0]);

		MessageMgr::Get()->AddLine(false, buffer);
		va_end(marker);
	}
}

//======================================================================================
// Console command implementations
//======================================================================================

extern bool g_bDisableAIPrediction;
void CmdAIPrediction()
{
	g_bDisableAIPrediction ? g_bDisableAIPrediction = false : g_bDisableAIPrediction = true;
}

void CmdClear()
{
	MessageMgr::Get()->ClearMessages();
}

void CmdMissLoad(int missID)
{
	Client.LoadMission(missID);
}

bool g_bNetStats = 0;
void CmdNetStats()
{
	g_bNetStats = !g_bNetStats;
}

bool g_bVisStats = 0;
void CmdVisStats()
{
	g_bVisStats = !g_bVisStats;
}

bool g_bMiscStats = 0;
void CmdMiscStats()
{
	g_bMiscStats = !g_bMiscStats;
}

void CmdNetState()
{
	if (g_pNetMan->IsNetworkGame())
	{
		int nPlayers = g_pNetMan->NumPlayers();

		if (g_pNetMan->AmDefaultHost())
			ConPrintF("Hosting a game with %d players.", nPlayers);
		else
			ConPrintF("Client in a game with %d players.", nPlayers);
	}
	else
		ConPrintF("Not network game.");
}

void CmdObjPlayers()
{
	int players[8];
	GetPlayers(players);

	for (int i = 0; i < kMaxPlayers; i++)
	{
		if (players[i])
			ConPrintF("Player %d: %s (%x)", i, _ObjEditName(players[i]), players[i]);
	}
}

void CmdContainList(int containerObj)
{
	int iterobj = 0;
	sContainIter* iter;

	if (containerObj)
		iter = g_pContainSys->IterStart(containerObj);
	else
		iter = g_pContainSys->IterStart(*_gPlayerObj);

	iterobj = iter->object;

	ConPrintF("Inventory contains: ");
	while (iterobj)
	{
		ConPrintF("%s", _ObjEditName(iter->object));

		iterobj = g_pContainSys->IterNext(iter);
	}
	g_pContainSys->IterEnd(iter);
}

void CmdInvAdd(const char* name)
{
	if (name)
	{
		int archID = g_pObjSys->GetObjectNamed(name);

		if (archID)
		{
			int objID = g_pObjSys->Create(archID, 1);

			if (objID)
			{
				if (g_pContainSys->Add(*_gPlayerObj, objID, 0, 1))
					ConPrintF("Failed to add object to inventory");
			}
		}
		else
			ConPrintF("Unable to find object named %s.", name);
	}
}

void CmdInvRem(int objectID)
{
	if (g_pContainSys->Remove(objectID, *_gPlayerObj))
		ConPrintF("Removed %s.", _ObjEditName(objectID));
	else
		ConPrintF("Failed to remove object.");
}

void CmdSuicide()
{
	g_cDarkGameSrvScriptService->KillPlayer();
}

void CmdSummonObj(const char* objName)
{
	if (objName)
	{
		int objectArch = g_pObjSys->GetObjectNamed(objName);
		int objectID;

		if (objectArch)
		{
			objectID = SummonObject(objectArch);
			if (objectID)
				ConPrintF("Created an object with ID %d.", objectID);
			else
				ConPrintF("Failed to create object.");
		}
		else
			ConPrintF("Unable to find object named %s.", objName);
	}
}

void CmdSimTime()
{
	ConPrintF("Sim time: %d", _GetSimTime());
}

void CmdSimPause(int pause)
{
	if (pause)
		_SimStatePause();
	else
		_SimStateUnpause();
}

void CmdPacketStats()
{
#ifdef _DEBUG
	//for (std::map<char*, int>::iterator iter = PacketSendsMap.begin(); iter != PacketSendsMap.end(); iter++)
	//{
	//	Log.Print("Packet %s was sent %d times.", iter->first, iter->second);
	//}

	for (std::map<char*, int>::iterator iter = PropSendsMap.begin(); iter != PropSendsMap.end(); iter++)
	{
		Log.Print("Property %s was sent %d times.", iter->first, iter->second);
	}
#endif
}

void CmdVisGet(int object)
{
	if (g_pObjSys->Exists(object))
	{
		sAIVisibility* vis;

		g_pAIVisibilityProperty->Get(object, (void**)&vis);

		ConPrintF("Level %d light rating %d movement %d exposure %d, last update %d.", vis->level, vis->lightRating, vis->movementRating, vis->exposureRating, vis->lastUpdate);
	}
	else
		ConPrintF("Object does not exist.");
}

void CmdSfxReset()
{
	_SFXReset();
	ConPrintF("Reinitialized SFX.");
}

void CmdReload()
{
	Config::Reload();

	ConPrintF("Settings reloaded.");
}

void CmdTeleport(char* loc)
{
	mxs_vector vec;
	mxs_angvec ang;
	string teleportLoc;

	if (loc)
		teleportLoc = loc;
	else
		teleportLoc = Cfg.GetString("Teleport");

	if (teleportLoc.empty())
	{
		ConPrintF("Teleported to last location.");
		vec.x = (float)atof(GetParam(teleportLoc, ' ', 0).c_str());
		vec.y = (float)atof(GetParam(teleportLoc, ' ', 1).c_str());
		vec.z = (float)atof(GetParam(teleportLoc, ' ', 2).c_str());
	}
	else
	{
		ConPrintF("Teleported to coordinates.");
		vec.x = (float)atof(GetParam(teleportLoc, ' ', 0).c_str());
		vec.y = (float)atof(GetParam(teleportLoc, ' ', 1).c_str());
		vec.z = (float)atof(GetParam(teleportLoc, ' ', 2).c_str());

		IniFile ini(".\\ThiefMP.ini");
		ini.WriteString("History", "Teleport", loc);
	}

	ConPrintF("coords: x %f y %f z %f", vec.x, vec.y, vec.z);

	cGameTools::TeleportObject(0, *_gPlayerObj, &vec, &ang, 1);
}

void CmdTransMax(double max)
{
	Cfg.SetFloat("VisCapMin", (float)max);
}

void CmdTransDistance(double distance)
{
	Cfg.SetFloat("DistanceModifier", (float)distance);
}

void CmdObjDetails(int objectID)
{
	if (g_pObjSys->Exists(objectID))
	{
		eNetworkCategory cat;
		cCreature* pCreature;
		Position* pos;
		sSpeech* pSpeech;
		sFrobInfo* pFrob;
		int hitpts;
		char buffer[256];
		mxs_vector vec;
		//int deathStage;

		ConPrintF("Details for object %s:", _ObjEditName(objectID));

		ConPrintF("  Archetype: %s", _ObjEditName(g_pTraitMan->GetArchetype(objectID))); 
		if (_ObjGetModelName(objectID, buffer))
			ConPrintF("  Model name: %s", buffer);

		if (_ObjGetHitPoints(objectID, &hitpts))
			ConPrintF("  Hit points: %d", hitpts);

		if (pFrobInfoProp->Get(objectID, (void**)&pFrob))
			ConPrintF("  Frob info: world = %x inv = %x tool = %x", pFrob->worldActionFlags, pFrob->invActionFlags, pFrob->toolActionFlags);

		if (g_pSpeechProp->Get(objectID, (void**)&pSpeech))
			ConPrintF("  Speech: schema %s, concept %s.", _ObjEditName(pSpeech->schemaID), cNameMapFns::NameFromID(g_Domain, NULL, pSpeech->concept));

		if (pos = _ObjPosGet(objectID))
		{
			mxs_vector vec = pos->vec;
			ConPrintF("  Position: %0.3f %0.3f %0.3f", vec.x, vec.y, vec.z);

			if (objectID != *_gPlayerObj)
				ConPrintF("  Distance from player: %0.2f", _mx_dist_vec(&_ObjPosGet(*_gPlayerObj)->vec, &_ObjPosGet(objectID)->vec));
		}

		_PhysGetVelocity(objectID, &vec);
		ConPrintF("  Velocity: %0.2f %0.2f %0.2f - overall speed %0.2f", vec.x, vec.y, vec.z, sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z)));

		cat = _NetworkCategory(objectID);
		if (cat == CAT_Hosted)
		{
			if (g_pObjNet->ObjHostedHere(objectID))
				ConPrintF("  Object is hosted locally.");
			else
				ConPrintF("  Object is hosted by %d.", g_pObjNet->ObjHostPlayer(objectID));
		}
		else if (cat == CAT_LocalOnly)
			ConPrintF("  Object is local only.", _ObjEditName(objectID));
		else
			ConPrintF("  Object has an unknown network category.");

		//IDamageModel* pDamageModel = (IDamageModel*)_AppGetAggregated(IID_IDamageModel);
		//if (pDamageModel)
		//{
		//	if (pDamageModel->deathStageProp->Get(objectID, &deathStage))
		//		ConPrintF("  Death stage: %d.", deathStage);

		//	pDamageModel->Release();
		//}

		if (int container = g_pContainSys->GetContainer(objectID))
		{
			ConPrintF("  Contained by: %s.", _ObjEditName(container));
		}

		if (OBJ_IS_CONCRETE(objectID) && (pCreature = _CreatureFromObj(objectID)))
			ConPrintF("  Creature = 0x%X", pCreature);

		if (cPhysModel* pModel = PhysModelFromObj(objectID))
		{
			ConPrintF("  Physics submodels: %d", pModel->m_numSubmodels);
			ConPrintF("  Phys model flags: 0x%x", pModel->m_flagState);
			ConPrintF("  Phys gravity: %0.2f", _PhysGetGravity(objectID));
			ConPrintF("  Phys mass: %0.2f (physmod %0.2f)", _PhysGetMass(objectID), pModel->m_mass);
			ConPrintF("  Phys model elasticity: %0.2f", pModel->m_elasticity);

			float friction;
			if (_PhysGetFriction(objectID, &friction))
				ConPrintF("  Phys friction: %0.2f", friction);
			ConPrintF("  Location controlled: %d", cPhysModelFns::IsLocationControlled(pModel, NULL));
		}
	}
	else
		ConPrintF("Object does not exist.");
}

void CmdObjID(const char* objName)
{
	if (objName)
		ConPrintF("%d", g_pObjSys->GetObjectNamed(objName));
}

void CmdQuestGet(const char* qvarName)
{
	if (qvarName)
	{
		IPtr<IQuestData> pQD = _AppGetAggregated(IID_IQuestData);
		ConPrintF("qvar %s: %d", qvarName, pQD->Get(qvarName));
	}
}

void CmdGhostList()
{
	sGhostRemote* pGhost;

	tHashSetHandle hs;
	hs.x00 = -1;
	hs.x04 = 0;

	pGhost = (sGhostRemote*)cHashSetBaseFns::ScanNext(_gGhostRemotes, NULL, &hs);

	ConPrintF("*** GHOST LIST: ***");

	while (pGhost)
	{
		ConPrintF("%s: hosted by %d.", _ObjEditName(pGhost->obj), g_pNetMan->PlayerNumToObj(g_pObjNet->ObjHostPlayer(pGhost->obj)));

		pGhost = (sGhostRemote*)cHashSetBaseFns::ScanNext(_gGhostRemotes, NULL, &hs);
	}

	ConPrintF("*** END LIST ***");
}

void CmdFileCrc(const char* file)
{
	if (file)
		ConPrintF("File CRC: %x.", Crc32::ScanFile(file));
}

void CmdHelp()
{
	// Loop through the chat commands and display help text, but skip the last two hidden commands
	for (int i = 0; i < (g_numCommands - 2); i++)
		ConPrintF(" %s - %s", ConsoleCmds[i].strCommand, ConsoleCmds[i].strHelp);
}

void CmdObjHighlit()
{
	ConPrintF("Highlit obj: %d.", *_highlit_obj);
}

void CmdFindAll(const char* archName)
{
	if (archName)
	{
		int arch = g_pObjSys->GetObjectNamed(archName);
		int object;
		IObjectQuery* pQuery = g_pObjSys->Iter(1);

		while (!pQuery->Done())
		{
			object = pQuery->Object();

			if (g_pTraitMan->GetArchetype(object) == arch)
				ConPrintF("%s is a %s.", _ObjEditName(object), archName);

			pQuery->Next();
		}

		pQuery->Release();
	}
}

void CmdTest(sGhostRemote* rem)
{
	rem->critter.r_state = 0;
}

void CmdObjectives()
{
	g_pOverlayMgr->EnableOverlay("objectives_overlay", true);
}


//======================================================================================
//======================================================================================
//======================================================================================

ConsoleCommand ConsoleCmds[] =
{
#ifndef _RELEASE
	{"/file_crc", "/fc", ARG_STRING, (CmdHandler)&CmdFileCrc, "Get file CRC"},
	{"/miss_load", "/ml", ARG_INT, (CmdHandler)&CmdMissLoad, "Load specified mission"},
	{"/packet_stats", "/ps", ARG_NONE, (CmdHandler)&CmdPacketStats, "Dump packet send stats"},
	{"/misc_stats", "/mst", ARG_NONE, (CmdHandler)&CmdMiscStats, "Display misc debug stats"},
	{"/vis_get", "/vg", ARG_INT, (CmdHandler)&CmdVisGet, "<obj num> Get object visibility"},
	{"/sim_pause", "/sp", ARG_INT, (CmdHandler)&CmdSimPause, "<0/1> Set sim pause state"},
#endif
	{"/ai_prediction", "", ARG_NONE, (CmdHandler)&CmdAIPrediction, "Toggle AI movement prediction"},
	{"/clear", "", ARG_NONE, (CmdHandler)&CmdClear, "Clear chat messages"},
	{"/contain_list", "/cl", ARG_INT, (CmdHandler)&CmdContainList, "<obj num> Enumerate container contents"},
	{"/find_all", "/fa", ARG_STRING, (CmdHandler)&CmdFindAll, "<obj/arch name> Find all objects of the specified type"},
	{"/ghost_list", "", ARG_NONE, (CmdHandler)&CmdGhostList, "List all remote ghosts"},
	{"/inv_add", "/ia", ARG_STRING, (CmdHandler)&CmdInvAdd, "<obj/arch name> Add an item to the player's inventory", true},
	{"/inv_rem", "/ir", ARG_INT, (CmdHandler)&CmdInvRem, "<obj num> Remove an item from the player's inventory", true},
	{"/net_state", "/ns", ARG_NONE, (CmdHandler)&CmdNetState, "Display network state"},
	{"/net_stats", "/nst", ARG_NONE, (CmdHandler)&CmdNetStats, "Draw network stats"},
	{"/obj_players", "/op", ARG_INT, (CmdHandler)&CmdObjPlayers, "Display all player objects", true},
	{"/obj_details", "/od", ARG_INT, (CmdHandler)&CmdObjDetails, "<obj num> Display object details"},
	{"/obj_id", "/oi", ARG_STRING, (CmdHandler)&CmdObjID, "<obj name> Display object ID"},
	{"/obj_highlit", "/ohi", ARG_NONE, (CmdHandler)&CmdObjHighlit, "Display highlighted obj num"},
	{"/obj_overlay", "", ARG_NONE, (CmdHandler)&CmdObjectives, "Display objectives overlay"},
	{"/quest_get", "/qg", ARG_STRING, (CmdHandler)&CmdQuestGet, "<qvar> Get quest variable"},
	{"/reload", "/r", ARG_NONE, (CmdHandler)&CmdReload, "Reload settings from .ini file", true},
	{"/suicide", "/kill", ARG_NONE, (CmdHandler)&CmdSuicide, "Kill local player"},
	{"/summon", "/s", ARG_STRING, (CmdHandler)&CmdSummonObj, "<obj name> Create an object at the player's position", true},
	{"/sfx_reset", "/sfxr", ARG_NONE, (CmdHandler)&CmdSfxReset, "Reinitialize sound effects system"},
	{"/sim_time", "/st", ARG_NONE, (CmdHandler)&CmdSimTime, "Show sim time"},
	{"/teleport", "/tp", ARG_STRING, (CmdHandler)&CmdTeleport, "<coords> Move to the specified x, y, z coordinates", true},
	{"/trans_max", "/tm", ARG_DOUBLE, (CmdHandler)&CmdTransMax, "Set max player transparency"},
	{"/trans_distance", "/td", ARG_DOUBLE, (CmdHandler)&CmdTransDistance, "Set transparency distance modifier"},
	{"/vis_stats", "/vst", ARG_NONE, (CmdHandler)&CmdVisStats, "Draw visibility stats for highlighted obj"},
	{"/test", "", ARG_INT, (CmdHandler)&CmdTest, ""},
	{"/help", "", ARG_NONE, (CmdHandler)&CmdHelp, ""},
};

int g_numCommands = (sizeof(ConsoleCmds) / sizeof(ConsoleCommand));

void CmdOpen()
{
	if (g_pOverlayMgr->OverlayEnabled("overlay_map") || g_pOverlayMgr->OverlayEnabled("overlay_book") || g_pOverlayMgr->OverlayEnabled("overlay_players"))
		return;

	if (g_pNetMan->IsNetworkGame())
	{
		ChatboxOpen();
	}
}

void CmdShowPlayers()
{
	if (g_pNetMan->IsNetworkGame())
	{
		if (g_pOverlayMgr)
			g_pOverlayMgr->ToggleOverlay("overlay_players");
	}
}

void InitNetCommands()
{
	static Command NetCommands[] = 
	{
		{"chat", ARG_NONE, (CmdHandler)&CmdOpen, "", 0},
		{"showplayers", ARG_NONE, (CmdHandler)&CmdShowPlayers, "", 0},
	};

	_CommandRegister(&NetCommands[0], (sizeof(NetCommands) / sizeof(Command)), -1);
}