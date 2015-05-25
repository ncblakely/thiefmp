#pragma once

#include "Engine\inc\Engine.h"
#include "Engine\inc\WinClasses.h"

#ifdef _THIEFBUILD
#define GAME_GUID guidThief
#else
#define GAME_GUID guidShock
#endif

#define TMP_BUILD 218

//#define NEW_NETCODE
//#define ALLOW_LATE_JOINS

#ifndef _RELEASE
#define DbgPrint ConPrintF
	#ifndef _SHOCKBUILD
	#define DbgPrintEcho ConPrintEchoF
	#else
	#define DbgPrintEcho ConPrintF
	#endif
#else
#define DbgPrint
#define DbgPrintEcho
#endif

#include "Imports.h"
#include "Console.h"
#include "Packets.h"
#include "DarkNet.h"
#include "asm.h"
#include "Config.h"
#include "Debug.h"
#include "Events.h"
#include "Engine\inc\Timing.h"
#include "LG.h"
#include "resource.h"

HRESULT __stdcall FakeCoCreateInstance(REFCLSID rclsid,LPUNKNOWN pUnkOuter,DWORD dwClsContext, REFIID riid,LPVOID *ppv);
void __stdcall PumpEventsHook(DWORD* thisObj, int P1, enum eWinPumpDuration pumpDuration);
void __stdcall StartGameModeCallback(DWORD P1);
void MessageHandler(HWND hwnd, uint msg, WPARAM wParam, LPARAM lParam);
void Shutdown();
void FixupImports();
void __stdcall DebugStringHook(LPCTSTR lpOutputString);
int __stdcall EquipAIWeaponHook(int objectID, DWORD P1, DWORD P2);
int __stdcall OnIsValidTarget(int object);
int SummonObject(int objectArch);
int ObjectIsVisible(int object);
int LaunchProjectileHook(int launcherObject, int projArchetype, float velocity, DWORD flags, DWORD P1, DWORD P2, DWORD P3);

// safer math functions
void mx_sub_vec_safe(mxs_vector* newvec, const mxs_vector* vec1, const mxs_vector* vec2);
float mx_dist_vec_safe(mxs_vector* vec1, mxs_vector* vec2);

// tools
void LogPrintCallback(void* text);
int MissionNumFromMap(int mapID);
int MissionNumToMap(int mission);
bool GetMissionName(int missionID, char* buff, int buffSize);
const char* DPlayErrorToString(HRESULT hrError);
cPhysModel* PhysModelFromObj(int obj);
bool ObjHasScript(int obj, const char* script);
int ProxyFrobGetInverse(int obj);
int GetNumChainedEvents(sChainedEvent* pEvent);
void PrintEventChain(sChainedEvent* pEvent);
sScreenMode GetScreenMode();
Rect GetScreenRect();
Rect GetCanvasRect();
cAnsiStr GetMissionName(int missionID);
NString TimeString(int time);

extern class InterfaceManager* UI;

long __stdcall ExceptFilter(PEXCEPTION_POINTERS data);

extern Logger Log;
#ifdef _DEBUG
extern std::map<char*, int> PropSendsMap;
extern CCriticalSection g_CoreThread;
//extern std::map<char*, int> PacketSendsMap;
#endif