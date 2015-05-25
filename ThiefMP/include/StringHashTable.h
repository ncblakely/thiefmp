#pragma once

enum StringHashID
{
	STRID_NotFound = 0,

	// Original Dark script messages
	STRID_TurnOn = 1,
	STRID_TurnOff,
	STRID_FrobWorldEnd,
	STRID_NowUnlocked,
	STRID_NowLocked,
	STRID_FrobToolEnd,
	STRID_Contained,
	STRID_PhysCollision,

	// New net script messages
	STRID_NetTurnOn,
	STRID_NetTurnOff,
	STRID_NetFrob,
	STRID_NetNowLocked,
	STRID_NetNowUnlocked,
};

struct StringHash
{
	const char* str;
	StringHashID shid;
	DWORD hash;
};

class StringHashTable
{
public:
	StringHashTable(StringHash* pHashEntries, int numEntries);

	StringHashID FindString(const char* str);

protected:
	StringHash* m_pHashEntries;
	int m_numEntries;
};

extern StringHashTable* g_pStrHash;