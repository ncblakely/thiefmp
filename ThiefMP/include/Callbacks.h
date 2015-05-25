#pragma once

// Packet handlers
void HookHandleDamage(int victim, int culprit, int damage);
void HookFrobRequest(int msDown, int sourceObj, int destObj, int srcLoc, int destLoc, int frobber, int flags);
void HookFrobPickup(int frobbedObj, int frobber);
void HookRegSphere(int object, int numSubmodels, DWORD flags, float radius, mxs_vector* location, DWORD facing);
void HookHandleCreateQuestData(char* questName, int questData, int P2, DWORD thisObj);
void HookHandleSetQuestData(char* questName, int questData, DWORD thisObj);
void HookHandleHaltSound(int handle, int object);
void HookHandlePlayerInfo(int playerIndex, char* playerName, char* playerIP, int playerObject, INetManager* netman);
void HookHandleFireProjectile(int projOwner, int projArchetype, DWORD flags, float intensity, mxs_vector* pos, mxs_angvec* ang, mxs_vector* vel);
void HookHandleRequestDestroy(int object);
void HookHandleFinishSynch(INetManager* pNetMan);
void HookSynchReady();
void HookHandleRemap(BYTE P1, uint P2, sSingleRemap* P3);
void HookHandleBeginCreate(int P1, int P2, int P3);
void HookHandleEndCreate(int object);
void HookHandleFrobPickup(int frobber, int object);

ulong __stdcall DarkDamageListener(const sDamageMsg* dmg, void* pData);
unsigned long __stdcall HookDarkCombatDamageListener(const sDamageMsg* dmgMsg, void* pData);

namespace HookAICombat
{
	int __stdcall IsValidTarget(int object);
}

namespace HookAIManager
{
	int __stdcall TransferAI(IAIManager* thisObj, int P1, int P2);
	int __stdcall CreateAI(IAIManager* thisObj, int iIndex,char const * cAIType);
	int __stdcall StartConversation(IAIManager* thisObj, int iID);
	//long __stdcall MakeFullAI(IAIManager* thisObj, int object, const sAINetTransfer* pTranfserInfo);
}

namespace HookAINetManager
{
	long __stdcall MakeProxyAI(IAINetManager* pThis, int aiObject, int playerObject);
}

namespace HookDarkGameSrv
{
	long __stdcall EndMission(DWORD thisObj);
	long __stdcall FadeToBlack(DWORD thisObj, float fadeTime);
}

class HookBowSrv : public IBowSrv
{
public:
	long __stdcall OnStartAttack();
	long __stdcall OnFinishAttack();
};

class HookSimpleDamageModel2 : public cSimpleDamageModel
{
public:
	unsigned long __stdcall OnDamageObject(int victim, int culprit, sDamage* pDmg, sChainedEvent* pEvent, int P4);
};

namespace HookInventory
{
	long __stdcall Select(IInventory* thisObj, int object);
	long __stdcall ClearSelection(IInventory* thisObj, eWhichInvObj invObj);
}

namespace HookAIConversationManager
{
	int __stdcall Start(IAIConversationManager* pAIConv, int convID);
}

namespace HookAIBehaviorSet
{
	void __stdcall EnactProxyJointScanAction(IAIBehaviorSet* pBehaviorSet, IAI* pAI, void* P2);
}

namespace HookContainSys
{
	long __stdcall Add(IContainSys* pContainSys, int objReceiving, int objectID, unsigned int P1, unsigned int P2);
}

namespace HookSimpleDamageModel
{
	long __stdcall DamageObject(DWORD thisObj, int damagedObj, int culpritObj, sDamage* dmg, struct sChainedEvent* chainedEvent, int P3);
}

namespace HookStoredProperty
{
	int __stdcall SendPropertyMsg(sNetMsg_Generic* msg, int objectID, int value, unsigned long type);
	void __stdcall ReceivePropertyMsg(struct sNetMsg_Generic const * msg, unsigned long P1, DWORD P2, int P3);
}

namespace HookNet
{
	int __stdcall Host(INet* thisObj, char * cGameType, char * cSessName);
	int __stdcall Join(INet* thisObj, const char* media, const char* password, const char* ip);
	long __stdcall SimpleCreatePlayer(INet* thisObj, char* player);
}

namespace HookNetManager
{
	void __stdcall SendCreatePlayerNetMsg();
	int __stdcall EnumPlayersCallback(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext);
	int __stdcall InitExistingPlayer(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext);
	int __stdcall _DestroyNetPlayerCallback(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext);
	const char* __stdcall GetPlayerAddress(DWORD thisObj, int playerNum);
	void __stdcall SuspendMessaging(INetManager* pNetMan);
	void __stdcall ResumeMessaging(INetManager* pNetMan);
	void __stdcall Leave(INetManager* pNetMan);
}

namespace HookDarkNetServices
{
	int __stdcall MyAvatarArchetype(DWORD thisObj);
}

namespace HookNetMsg
{
	void __fastcall HandleMsg(cNetMsg* pNetMsg, int, struct sNetMsg_Generic const * msg, unsigned long P1, int P2);
	int __fastcall MarshalArguments(cNetMsg* pNetMsg, int, char * msgData, int* P1, unsigned int* P2);
}

namespace HookGhostReceive
{
	void ReceiveAIHeartbeat(int objectID, int seqID, sGhostHeartbeat* data);
	void ReceiveFullHeartbeat(int objectID, int seqID, int relObj, sGhostHeartbeat* hbData, sGhostMoCap* mcData);
	void ReceiveRotHeartbeat(int objectID, int seqID, sGhostHeartbeat* data);
}

namespace HookObjectNetworking
{
	void __stdcall ClearTables(IObjectNetworking* thisObj);
	void __stdcall ObjRegisterProxy(IObjectNetworking* thisObj, int ownerID, short shortObj, int object);
	int __stdcall ObjGetProxy(IObjectNetworking* thisObj, int P1, short P2);
	void __stdcall ObjDeleteProxy(IObjectNetworking* thisObj, int P1);
	int __stdcall ObjIsProxy(IObjectNetworking* thisObj, int object);
	void __stdcall ObjHostInfo(IObjectNetworking* thisObj, int P1, int* P2, short* P3);
	void __stdcall ObjTakeOver(IObjectNetworking* thisObj, int object);
	void __stdcall ObjGiveTo(IObjectNetworking* thisObj, int P1, int P2, int P3);
	void __stdcall ObjGiveWithoutObjID(IObjectNetworking* thisObj, int P1, int P2);
	void __stdcall StartBeginCreate(IObjectNetworking* pThis, int creatorObjOrArch, int objectID);
	void __stdcall FinishBeginCreate(IObjectNetworking* thisObj, int objectID);
	void __stdcall StartEndCreate(IObjectNetworking* thisObj, int P1);
	void __stdcall FinishEndCreate(IObjectNetworking* pThis, int objectID);
	void __stdcall StartDestroy(IObjectNetworking* thisObj, int objectID);
	void __stdcall NotifyObjRemapped(IObjectNetworking* thisObj, int mapFrom, int mapTo);
}

namespace HookObjectSystem
{
	long __stdcall Unlock(IObjectSystem* pObjSys);
}

namespace HookPickLockSrv
{
	int __stdcall StartPicking(DWORD thisObj, int playerObject, int lockpickObject, int lockedObject);
	int __stdcall FinishPicking(DWORD thisObj, int lockpickObject);
}

namespace HookContainSrv
{
	long __stdcall Add(DWORD, class object obj1, class object obj2, int P1, int P2);
}

namespace HookGroundActionManeuverFactory
{
	cMotionPlan* __stdcall CreatePlan(const cMotionSchema* pSchema, struct sMcMotorState &pMotorState, struct sMcMoveState &pMoveState, sMcMoveParams& pParams, class IMotor* pMotor, class cMotionCoordinator *pMotCoord);
}

namespace HookPlayerManeuverFactory
{
	cMotionPlan* __stdcall CreatePlan(const cMotionSchema* pSchema, struct sMcMotorState &pMotorState, struct sMcMoveState &pMoveState, sMcMoveParams& pParams, class IMotor* pMotor, class cMotionCoordinator *pMotCoord);
}

namespace HookGroundLocoManeuverFactory
{
	cMotionPlan* __stdcall CreatePlan(const cMotionSchema* pSchema, struct sMcMotorState &pMotorState, struct sMcMoveState &pMoveState, sMcMoveParams& pParams, class IMotor* pMotor, class cMotionCoordinator *pMotCoord);
}

namespace HookLoading
{
	void __fastcall OnLoopMsg(cLoading* thisObj, int, int P1, tLoopMessageData__* loopMsg);
	void __fastcall DoFileLoad(cLoading* thisObj, int, ITagFile* pTag);
}

namespace HookLoadingSaveGame
{
	void __fastcall OnLoopMsg(cLoadingSaveGame* thisObj, int, int flags, tLoopMessageData__* loopMsg);
}

namespace HookDarkFilePanel
{
	const char* __stdcall SlotFileName(int slot); // Used by both cSaveFilePanel and cLoadFilePanel
}

namespace HookSaveFilePanel
{
	void __fastcall DoFileOp(DWORD* thisObj, int, int slot);
}

namespace HookDebugScrSrv
{
	long __stdcall MPrint(DWORD* thisObj, cScrStr& str1, cScrStr& str2, cScrStr& str3, cScrStr& str4, cScrStr& str5, cScrStr& str6, cScrStr& str7, cScrStr& str8);
}

namespace HookLoopDispatch
{
	long __stdcall SendMessage(int flags, tLoopMessageData__ *, int);
}

namespace HookScriptMan
{
	long __stdcall AddModule(DWORD thisObj, const char* modName);
	long __stdcall SendMessageA(DWORD thisObj, sScrMsg* pMsg, sMultiParm* pParms);
}