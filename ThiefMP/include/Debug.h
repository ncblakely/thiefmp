#pragma once

#define DEBUG_GENERAL (1<<0)
#define DEBUG_SENDS (1<<1)
#define DEBUG_RECEIVES (1<<2)
#define DEBUG_OBJECTS (1<<3)
#define DEBUG_FROBS (1<<4)
#define DEBUG_DAMAGE (1<<5)
#define DEBUG_NET (1<<6)
#define DEBUG_PROPSENDS (1<<7)
#define DEBUG_QUESTS (1<<8)
#define DEBUG_SOUNDS (1<<9)
#define DEBUG_SCRIPTS (1<<10)
#define DEBUG_INVENTORY (1<<11)
#define DEBUG_GHOSTS (1<<12)
#define DEBUG_GLOBAL (1<<13)
#define DEBUG_PROXYACTS (1<<14)

class CDebugManager
{
public:
	CDebugManager();

	void LoadFlags();
	void SaveFlags();

	void SetExceptionFilter();
	void SetExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER pFilter);
	void RestoreExceptionFilter();

	void ToggleFlag(unsigned int flag);
	void ClearFlag(unsigned int flag);
	void ClearAllFlags();

	inline unsigned int IsFlagSet(unsigned int flag) { return m_DebugFlags & flag; }

protected:
	static long __stdcall ExceptFilter(PEXCEPTION_POINTERS data);

	LPTOP_LEVEL_EXCEPTION_FILTER m_LastExceptionFilter;
	unsigned int m_DebugFlags;
};

extern CDebugManager Debug;