/*************************************************************
* File: Config.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Reads config variables.
*************************************************************/

#include "stdafx.h"
#include "Main.h"
#include "Engine\inc\Crc32.h"
#include "Client.h"
#include "Patcher.h"

ConfigManager Cfg;

bool g_bEchoDebug;

namespace Config
{

void ApplySettings()
{
	// Change ddraw.dll string to ddfix.dll if ddfix is enabled
	if (Cfg.GetBool("UseDDFix"))
	{
#if (GAME == GAME_THIEF)
		SafeWriteData((void*)0x0067996E, 0x786966, 3);
#else if (GAME == GAME_DROMED)
		SafeWriteData((void*)0x007EB566, 0x786966, 3);
#endif
	}

	if (Cfg.GetInt("MaximumPlayers") > kMaxPlayers)
		Cfg.SetInt("MaximumPlayers", kMaxPlayers);
}

void GetSettings()
{	
	// Debug
	Cfg.ReadBool("Debug", "AllowJoinsInProgress", false, false);
	Cfg.ReadBool("Debug", "DisableNetPhysCap", false, false);
	Cfg.ReadBool("Debug", "ShowScriptDebug", false, false);

	// General
	Cfg.ReadBool("General", "UseDDFix", false, false);
	Cfg.ReadInt("General", "TextColor", 0, true);

	// Global Server
	Cfg.ReadString("Global Server", "HostName", 30, "t2mp.servegame.com", true);
	Cfg.ReadInt("Global Server", "GlobalPort", 5199, true);

	// Network
	Cfg.ReadInt("Network", "BufferSize", 4096, true); // was 8192
	Cfg.ReadInt("Network", "HeartbeatFrequencyMax", 35, true);

	// Menu
	Cfg.ReadBool("Menu", "MenuEnabled", true, true);

	// Overlays
	Cfg.ReadBool("Overlays", "UseOverlays", true, true);
	Cfg.ReadBool("Overlays", "AutomapMouseMode", true, true);

	// Player
	Cfg.ReadString("Player", "PlayerName", (kMaxPlayerName - 1), "Player", true);
	Cfg.ReadInt("Player", "Model", 0, true);

	// Server
	Cfg.ReadBool("Server", "NoLoadout", true, true);
	Cfg.ReadBool("Server", "TeamDamage", true, true);
	Cfg.ReadInt("Server", "MaximumPlayers", 8, true);
	Cfg.ReadInt("Server", "ServerPort", DEFAULT_PORT, true);
	Cfg.ReadString("Server", "ServerName", kMaxGameName - 1, "Thief 2 Server", true);
	Cfg.ReadBool("Server", "RespawnEnabled", true, true);
	Cfg.ReadBool("Server", "ShareLoot", true, true);
	Cfg.ReadInt("Server", "RespawnDelay", 10, true);
	Cfg.ReadBool("Server", "PlayerTransparency", true, true);
	Cfg.ReadInt("Server", "StartingLoot", 0, true);
	Cfg.ReadBool("Server", "ListOnGlobal", false, true);
	Cfg.ReadString("Server", "Password", kMaxPassword, "", true);
	Cfg.ReadBool("Server", "CheatsEnabled", false, true);
	Cfg.ReadBool("Server", "IncreaseDifficulty", false, true);

	// Mission
	Cfg.ReadInt("Mission", "SaveLimit", 0, true);
	Cfg.ReadInt("Mission", "TimeLimit", 0, true);

	// Transparency
	Cfg.ReadFloat("Transparency", "VisCapMin", 0.65f, true);
	Cfg.ReadFloat("Transparency", "DistanceModifier", 50.0f, true);

	// History
	Cfg.ReadString("History", "IPAddress", NULL, "", true);
	Cfg.ReadString("History", "Teleport", NULL, "", true);
	Cfg.ReadInt("History", "Mission", 0, true);
}

void Reload()
{
	Cfg.ClearVariables();

	GetSettings();
	ApplySettings();
}

}

void ConfigInit()
{
	Cfg.SetIniFile(".\\ThiefMP.ini");
	Config::GetSettings();
	Config::ApplySettings();
}