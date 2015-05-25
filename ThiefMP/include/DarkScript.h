#include "Interface.h"

// custom virtual script methods are thiscall

interface IEngineRootScript : public IUnknown
{
	STDMETHOD_(const char*, GetName)(void) { }; // 0xC
	STDMETHOD_(long, ReceiveMessage)(sScrMsg *, struct sMultiParm *, enum eScrTraceAction) { }; // 0x10
	virtual void OnDisconnect(DWORD) { }; // 0x14
	virtual void unk2() { }; // 0x18
	virtual void OnMessage(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x1C
	virtual void OnBeginScript(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x20
	virtual void OnEndScript(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x24
	virtual void OnTimer(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x28
	virtual void RegisterListeners() { }; // 0x2C
	virtual void OnTweqComplete(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x30
	virtual void OnNowLocked(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x34
	virtual void OnNowUnlocked(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x38
	virtual void OnSoundDone(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x3C
	virtual void OnSchemaDone(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x40
	virtual void OnQuestChange(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x44
	virtual void OnSim(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x48
	virtual void OnMotionStart(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x4C
	virtual void OnMotionEnd(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x50
	virtual void OnMotionFlagReached(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x54
	virtual void OnCreate(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x58
	virtual void OnDestroy(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x5C
	virtual void OnFrobToolBegin(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x60
	virtual void OnFrobToolEnd(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x64
	virtual void OnFrobWorldBegin(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x68
	virtual void OnFrobWorldEnd(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x6C
	virtual void OnFrobInvBegin(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x70
	virtual void OnFrobInvEnd(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x74
	virtual void OnWorldFocus(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x78
	virtual void OnWorldDeFocus(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x7C
	virtual void OnWorldSelect(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x80
	virtual void OnWorldDeSelect(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x84
	virtual void OnInvFocus(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x88
	virtual void OnInvDeFocus(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x8C
	virtual void OnInvSelect(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x90
	virtual void OnInvDeSelect(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x94
	virtual void OnCombine(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x98
	virtual void OnContainer(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0x9C
	virtual void OnContained(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xA0

	DWORD x04;
	int m_scriptObjID; // 0x8
};

interface IRootScript: public IEngineRootScript
{
	virtual void OnDamage(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xA4
	virtual void OnSlain(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xA8
	virtual void OnReportMessage(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xAC
	virtual void OnDifficulty(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xB0
	virtual void OnPickStateChange(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xB4
	virtual void OnDarkGameModeChange(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xB8
};

interface IDoor : public IEngineRootScript
{
	virtual void OnDoorOpen(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xBC
	virtual void OnDoorClose(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xC0
	virtual void OnDoorOpening(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xC4
	virtual void OnDoorClosing(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xC8
	virtual void OnDoorHalt(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xCC
};

interface IPhysObjScript: public IRootScript
{
	virtual void OnPhysFellAsleep(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xBC
	virtual void OnPhysWokeUp(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xC0
	virtual void OnPhysMadePhysical(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xC4
	virtual void OnPhysMadeNonPhysical(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xC8
	virtual void OnPhysCollision(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xCC
	virtual void OnPhysContactCreate(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xD0	
	virtual void OnPhysContactDestroy(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xD4
	virtual void OnPhysEnter(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xD8
	virtual void OnPhysExit(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xDC
};

interface IAIScript: public IRootScript
{
	virtual void OnSignalAI(sScrMsg* pMsg, cMultiParm* pMultiParm) { } // 0xBC
	virtual void OnPatrolPoint(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xC0
	virtual void OnAIModeChange(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xC4
	virtual void OnAlertness(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xC8
	virtual void OnHighAlert(sScrMsg* pMsg, cMultiParm* pMultiParm) { } // 0xCC
	virtual void OnObjActResult(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xD0
};

class MovingTerrainScript: public IRootScript
{
public:
	virtual void OnMovingTerrainWaypoint(sScrMsg* pMsg, cMultiParm* pMultiParm) = 0; // 0xBC
};

class StdElevatorScript: public MovingTerrainScript
{
public:
	virtual void OnCall(int object) = 0; // 0xC0
	virtual void elevator_unk1() = 0; // 0xC4
	virtual void elevator_unk2() = 0; // 0xC8
	virtual void OnActivate(true_bool activate) = 0; // 0xCC
};

class StopAtWaypointsScript: public MovingTerrainScript
{
public:
};

class CollisionStickScript: public IPhysObjScript
{
public:
	virtual true_bool ObjectsHaveCanAttach(true_bool b) { }; // 0xE0
	virtual true_bool ObjSupportsAttach(int collidedWith) { }; // 0xE4
	virtual float GetScriptTiming() { }; // 0xE8
};

class CameraAlertScript: public IAIScript
{
public:
	virtual void f1(sScrMsg* pMsg, cMultiParm* pMultiParm) { } // 0xD4
	virtual void f2(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xD8
	virtual void f3(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xDC
	virtual void f4(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xE0
	virtual void f5(sScrMsg* pMsg, cMultiParm* pMultiParm) { } // 0xE4
	virtual void f6(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xE8
	virtual void f7(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xEC
	virtual void f8(sScrMsg* pMsg, cMultiParm* pMultiParm) { }; // 0xF0
	virtual void f9(sScrMsg* pMsg, cMultiParm* pMultiParm) { } // 0xF4
	virtual void SetModelName(int awareness) { }; // 0xF8
};