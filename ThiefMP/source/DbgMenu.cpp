/*************************************************************
* File: DbgMenu.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "resource.h"
#include "Debug.h"

#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
extern bool g_bEchoDebug;

void DbgMenuToggleEcho()
{
	g_bEchoDebug ? g_bEchoDebug = false : g_bEchoDebug = true;
}
#endif

void EnableDebugMenuItems(HMENU hMenu)
{
	CheckMenuItem(hMenu, ID_DEBUG_DAMAGE, Debug.IsFlagSet(DEBUG_DAMAGE) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_FROBS, Debug.IsFlagSet(DEBUG_FROBS) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_GENERAL, Debug.IsFlagSet(DEBUG_GENERAL) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_GHOSTS, Debug.IsFlagSet(DEBUG_GHOSTS) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_INVENTORY, Debug.IsFlagSet(DEBUG_INVENTORY) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_SCRIPTS, Debug.IsFlagSet(DEBUG_SCRIPTS) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_NETWORK, Debug.IsFlagSet(DEBUG_NET) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_OBJECTS, Debug.IsFlagSet(DEBUG_OBJECTS) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_PACKETSENDS, Debug.IsFlagSet(DEBUG_SENDS) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_PACKETRECEIVES, Debug.IsFlagSet(DEBUG_RECEIVES) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_PROPERTYSENDS, Debug.IsFlagSet(DEBUG_PROPSENDS) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_QUESTDATA, Debug.IsFlagSet(DEBUG_QUESTS) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_SOUNDS, Debug.IsFlagSet(DEBUG_SOUNDS) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_GLOBALSERVER, Debug.IsFlagSet(DEBUG_GLOBAL) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_DEBUG_PROXYACTIONS, Debug.IsFlagSet(DEBUG_PROXYACTS) ? MF_CHECKED : MF_UNCHECKED);

#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
	CheckMenuItem(hMenu, ID_DEBUG_ECHO, g_bEchoDebug ? MF_CHECKED : MF_UNCHECKED);
#endif
}