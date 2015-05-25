/*************************************************************
* File: StringHashTable.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Quick and dirty hashing for script messages
*************************************************************/

#include "stdafx.h"
#include "Engine\inc\Crc32.h"
#include "Debug.h"

#include "StringHashTable.h"

StringHashTable* g_pStrHash = NULL;

static StringHash g_StringHashes[] =
{
	{"TurnOn", STRID_TurnOn},
	{"TurnOff", STRID_TurnOff},
	{"FrobWorldEnd", STRID_FrobWorldEnd},
	{"FrobToolEnd", STRID_FrobToolEnd},
	{"NowLocked", STRID_NowLocked},
	{"NowUnlocked", STRID_NowUnlocked},
	{"Contained", STRID_Contained},
	{"PhysCollision", STRID_PhysCollision},

	{"NetTurnOn", STRID_NetTurnOn},
	{"NetTurnOff", STRID_NetTurnOff},
	{"NetFrob", STRID_NetFrob},
	{"NetNowLocked", STRID_NetNowLocked},
	{"NetNowUnlocked", STRID_NetNowUnlocked},
};

StringHashTable::StringHashTable(StringHash* pHashEntries, int numEntries)
{
	for (int i = 0; i < numEntries; i++)
	{
		pHashEntries[i].hash = Crc32::CreateID(pHashEntries[i].str);
	}

	m_pHashEntries = pHashEntries;
	m_numEntries = numEntries;
}

StringHashID StringHashTable::FindString(const char* str)
{
	DWORD hashToFind = Crc32::CreateID(str);

	for (int i = 0; i < m_numEntries; i++)
	{
		if (m_pHashEntries[i].hash == hashToFind)
			return m_pHashEntries[i].shid;
	}

	return STRID_NotFound;
}

void StringHashInit()
{
	if (!g_pStrHash)
		g_pStrHash = new StringHashTable(g_StringHashes, sizeof(g_StringHashes) / sizeof(StringHash));
	else
		dbgassert(false && "tried to init string hash table twice");
}

void StringHashTerm()
{
	SAFE_DELETE(g_pStrHash);
}