#include "stdafx.h"
#if (GAME == GAME_THIEF)
#include "..\..\ThiefMP\include\Main.h"
#elif (GAME == GAME_DROMED)
#include "..\DromedHook\Main.h"
#endif

#if (GAME == GAME_THIEF)
#define DEF_MEMBERPTR(p, a1, a2, a3, a4) __asm mov dword ptr ds: [p], a1
#elif (GAME == GAME_SHOCK)
#define DEF_MEMBERPTR(p, a1, a2, a3, a4) __asm mov dword ptr ds: [p], a2
#elif (GAME == GAME_DROMED)
#define DEF_MEMBERPTR(p, a1, a2, a3, a4) __asm mov dword ptr ds: [p], a3
#elif (GAME == GAME_SHOCKED)
#define DEF_MEMBERPTR(p, a1, a2, a3, a4) __asm mov dword ptr ds: [p], a4
#endif

//======================================================================================
// Class function pointer definitions.
//======================================================================================
void FixupImports()
{
	// cGroundActionManeuverFactory methods
	DEF_MEMBERPTR(GroundActionManeuverFactory.CreatePlan, 0x00520070, NULL, 0x00571AC0, NULL);

	// cPlayerManeuverFactory methods
	DEF_MEMBERPTR(PlayerManeuverFactory.CreatePlan, 0x005215D0, NULL, NULL, NULL);

	// cLoadingSaveGame methods
	DEF_MEMBERPTR(LoadingSaveGame.OnLoopMsg, 0x00559D40, NULL, 0x005F0CB0, NULL);
}