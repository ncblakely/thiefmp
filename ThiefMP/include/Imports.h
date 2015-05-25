#pragma once

#ifdef _DEFINE_OFFSETS

	#include "initguid.h"

	#if (GAME == GAME_THIEF)
	#define DEF_VAR(type, p, a1, a2, a3, a4) type p = (type)a1;
	#define DEF_FUNCPTR(type, calltype, p, args, a1, a2, a3, a4) type (calltype* p)args = (type (calltype*) args)a1;
	#define DEF_OFFSET(type, p, a1, a2, a3, a4) GlobalPointer<type> p(a1);
	#elif (GAME == GAME_SHOCK)
	#define DEF_VAR(type, p, a1, a2, a3, a4) type p = (type)a2;
	#define DEF_FUNCPTR(type, calltype, p, args, a1, a2, a3, a4) type (calltype* p)args = (type (calltype*) args)a2;
	#define DEF_OFFSET(type, p, a1, a2, a3, a4) GlobalPointer<type> p(a2);
	#elif (GAME == GAME_DROMED)
	#define DEF_VAR(type, p, a1, a2, a3, a4) type p = (type)a3;
	#define DEF_FUNCPTR(type, calltype, p, args, a1, a2, a3, a4) type (calltype* p)args = (type (calltype*) args)a3;
	#define DEF_OFFSET(type, p, a1, a2, a3, a4) GlobalPointer<type> p(a3);
	#elif (GAME == GAME_SHOCKED)
	#define DEF_VAR(type, p, a1, a2, a3, a4) type p = (type)a4;
	#define DEF_FUNCPTR(type, calltype, p, args, a1, a2, a3, a4) type (calltype* p)args = (type (calltype*) args)a4;
	#define DEF_OFFSET(type, p, a1, a2, a3, a4) GlobalPointer<type> p(a4);
	#endif

#else

#define DEF_VAR(type, p, a1, a2, a3, a4) extern type p;
#define DEF_FUNCPTR(type, calltype, p, args, a1, a2, a3, a4) extern type (calltype* p)args;
#define DEF_OFFSET(type, p, a1, a2, a3, a4) extern GlobalPointer<type> p;

#endif

#include "Engine\inc\Engine.h"
#include "DarkScript.h"

#ifdef _THIEFBUILD
#include "T2IID.h"
#endif

// Generic pointer template to make the syntax for dereferencing multiple levels of indirection a bit less cumbersome
template <typename T>
class GlobalPointer
{
public:
	GlobalPointer(unsigned int offset) { m_TheOffset = (T**)offset;}

	T* operator->() const { return *m_TheOffset; }
	operator T*() const { return *m_TheOffset; }

protected:
	T** m_TheOffset;
};

template <typename T>
class GlobalOffset
{
public:
	GlobalOffset(unsigned int offset) { m_TheOffset = (T*)offset;}

	operator T() const { return *m_TheOffset; }
	T operator*() const { return *m_TheOffset; }

protected:
	T* m_TheOffset;
};

// imported class function pointers

extern void* (__cdecl* DarkNew)(size_t);

struct sNetPlayer
{
	DPNID playerDpnid;
	cAvatar* pAvatar;
	DWORD x08;
	char playerName[36];
};

struct INetPlayerProperty : public IGenericProperty
{
};

struct NetMsgParams
{
	eNetMsgArgType type;
	DWORD dataSize; // only used for the "Block" parameter and some others, automatic for types with known sizes
	char* paramName;
	int P3;
};

struct sNetMsgDesc
{
	DWORD typeFlags;
	char* name1;
	char* name2;
	char* debugConfigVariable;
	void* handlerFunction;
	NetMsgParams params[kMaxNetMsgParams];
};

struct NetMsgParser
{
	DWORD x00, x04, x08, x0c, x10, x14, x18, x1c;
	class cNetMsg* pMsg;
};


#pragma pack(push, 1)
class cNetMsg
{
public:
	cNetMsg(sNetMsgDesc*, void*);
	virtual ~cNetMsg();

	void* operator new (size_t bytes) { return DarkNew(bytes); }

	virtual void __cdecl Send(int, ...);

	sNetMsgDesc* m_msgDesc;
	int m_paramCount;	// limited to kMaxNetMsgParams
	char m_data[21];
};
#pragma pack(pop)

// Forward declarations
class cDarkPanel;
class cLoading;
class cLoadingSaveGame;
class cGenericAggregate;

class cLoadingSaveGameFns
{
public:
	void (__fastcall* OnLoopMsg)(class cLoadingSaveGame* thisObj, int, int, tLoopMessageData__ *);
};

class cStandardRelationFns
{
public:
	void (__fastcall* SendLinkMsg)(DWORD* thisObj, int, unsigned long P1, long P2, struct sLink* link);
};

class cPlayerMotion
{
	enum eGhostMotion m_motion;
};

class cPlayerSoundList
{
	DWORD P1;
	DWORD P2;
	void* SchemaCallback;
};

#pragma pack(push, 1)

struct sMotorState; // 0x16 bytes
struct sMcMotorState;

class cCreature
{
public:
	DWORD P1;		// 0x0
	DWORD P2;		// 0x4
	int m_objectID;	 // 0x8
	sMotorState* m_motorState;	// 0xC

	//int m_bIsPhysical; // 0x30
	// int m_bIsBallistic; // 0x34
	// int m_CreatureType // 0x38
	// unk 0x3C
	//int m_nJoints; // 0x40

	int GetObjID() { return m_objectID; }
};

struct sCreatureHandle
{
public:
	DWORD P1;
	cCreature* pCreature;
};

class IMotor
{
public:
	cCreature m_creature;
};

class cMotionCoordinator : public IMotionCoordinator, public IMotorResolver
{
public:
	BYTE unk[0x8];
	//IMotor* m_pMotor;		// 8

	// sMcMotorState* m_pMcMotorState; // 24
};

class cGroundActionManeuver
{
public:
	char data[0x3D];
	int motionID;
};

class cMotionSchema
{
public:
	DWORD P1;
	DWORD P2;
	int archetypeID; // MotArchetype
	int motionID; // actual motion number
};

class cMotionPlan; // 0xC bytes?
struct sMcMoveParams;

class cGroundActionManeuverFactory
{
public:
	cMotionPlan* (__stdcall* CreatePlan)(class cMotionSchema const *,struct sMcMotorState const &,struct sMcMoveState const &, const sMcMoveParams& pParams,class IMotor *,class cMotionCoordinator *);
};

class cPlayerManeuverFactory
{
public:
	cMotionPlan* (__stdcall* CreatePlan)(class cMotionSchema const *,struct sMcMotorState const &,struct sMcMoveState const &, const sMcMoveParams& pParams,class IMotor *,class cMotionCoordinator *);
};


class cScriptProp : public IGenericProperty
{
public:
};

struct sScriptProp
{
	char scripts[4][32];
	//bool bDontInherit;
};

class cWindowsApplication
{
public:
	class cWinAppOperations
	{
	public:
		BYTE unk[0x18];
		HWND m_GameWindow;
	};
};

typedef int (__cdecl *LoopClient)(void*,int,tLoopMessageData__);
class cLoopManager
{
	DWORD x00, x04;
	DWORD loopModeFlags; // 0x8 - determines which listeners will be called for the current mode
};

class cGenericLoopClient
{
	virtual ~cGenericLoopClient();

	DWORD x04, x08;
	struct sLoopClientDesc* m_desc; // 0xC
	void* x10; // void* passed to constructor
};

#pragma pack(pop)

// Classes
DEF_OFFSET(IPropertyManager, g_pPropMan, 0x00687128, 0x006C09B8, 0x00807DB0, NULL)
DEF_OFFSET(IReactions, g_pReactions, 0x006C3230, NULL, NULL, NULL)
DEF_OFFSET(INetManager, g_pNetMan, 0x006C3EFC, 0x006F24D0, 0x0082A20C, NULL)
DEF_OFFSET(INet, g_pNet, 0x006C9434, NULL, NULL, NULL)
DEF_OFFSET(IObjectSystem, g_pObjSys, 0x006B8D24, NULL, 0x0083ABB4, NULL)
DEF_OFFSET(IObjectNetworking, g_pObjNet, 0x006C3EF4, NULL, 0x0084A05C, NULL)
DEF_OFFSET(IInputBinder, g_pInputBinder, 0x00687E04, NULL, 0x008092DC, NULL)
DEF_OFFSET(IAIManager, g_pAIMan, 0x006BB634, NULL, 0x0083D5C8, NULL)
DEF_OFFSET(IContainSys, g_pContainSys, 0x006F563C, NULL, 0x00A3F7E8, NULL)
DEF_OFFSET(ITraitManager, g_pTraitMan, 0x0068712C, NULL, 0x00807DB4, NULL)
DEF_OFFSET(IInventory, g_pInventory, 0x006F55CC, NULL, 0x00A3F71C, NULL)
DEF_OFFSET(ILinkManager, g_pLinkMan, 0x00689E64, NULL, 0x0080B954, NULL)
DEF_OFFSET(cPlayerMode, g_pPlayerMode, 0x006900AC, NULL, 0x00811D10, NULL)
DEF_OFFSET(cWindowsApplication::cWinAppOperations, g_pWinAppOps, 0x00716AB0, NULL, 0x00ADF0D8, NULL)
DEF_OFFSET(cPhysModels, g_PhysModels, 0x006C3C50, NULL, 0x00849DC0, NULL)
DEF_OFFSET(IMalloc, g_pMalloc, 0x006F5E00, NULL, NULL, NULL)
DEF_OFFSET(IScriptMan, g_pScriptMan, 0x007204F8, NULL, NULL, NULL)
DEF_OFFSET(IMotionSet, g_pMotionSet, 0x006C52B0, NULL, NULL, NULL)
DEF_VAR(IDamageModel**, g_ppDamageModel, 0x006C3C18, NULL, NULL, NULL)

// Script services
DEF_VAR(IActReactSrv*, g_pActReact, 0x006C3238, NULL, 0x00845268, NULL)
DEF_VAR(INetworkingSrv*, g_pNetSrv, 0x6C3320, NULL, 0x00845350, NULL)
DEF_VAR(IDamageSrv*, g_cDamageSrvScriptService, 0x006C32A8, NULL, 0x008452D8, NULL)
DEF_VAR(IDarkGameSrv*, g_cDarkGameSrvScriptService, 0x006F5820, NULL, 0x00A3F9D0, NULL)
DEF_VAR(ISoundScrSrv*, g_cSoundScrSrvScriptService, 0x006C33E0, NULL, NULL, NULL)

// Properties
DEF_OFFSET(IGenericProperty, g_pLootProp, 0x006F53A4, NULL, 0x00A3F4F0, NULL)
DEF_OFFSET(IBoolProperty, g_pBloodCauseProp, 0x006F5300, NULL, 0x00A3F430, NULL)
DEF_OFFSET(IStringProperty, g_pBloodTypeProp, 0x006F52FC, NULL, 0x00A3F42C, NULL)
DEF_OFFSET(IGenericProperty, g_pPickCfgProperty, 0x006F5B04, NULL, 0x00A3FCBC, NULL)
DEF_OFFSET(IGenericProperty, g_pPickStateProperty, 0x006F5B14, NULL, 0x00A3FCCC, NULL)
DEF_OFFSET(IGenericProperty, g_pAIVisibilityProperty, 0x006C27F4, NULL, 0x008447DC, NULL)
DEF_OFFSET(IGenericProperty, pFrobInfoProp, 0x006C2CE4, NULL, 0x00844D04, NULL)
DEF_OFFSET(IGenericProperty, g_pESndClassProp, 0x00687138, NULL, 0x00807DC0, NULL)
DEF_OFFSET(IGenericProperty, tweq_models_prop, 0x006C3090, NULL, NULL, NULL)
DEF_OFFSET(IGenericProperty, g_pAIProperty, 0x006C2844, NULL, NULL, NULL)
DEF_OFFSET(IGenericProperty, g_pAIFrustratedProperty, 0x006C2830, NULL, NULL, NULL)
DEF_OFFSET(IIntProperty, gFrobHandlerProp, 0x006C2CEC, NULL, 0x00844D0C, NULL)
DEF_OFFSET(IIntProperty, gStackCountProp, 0x006C2C5C, NULL, 0x00844C7C, NULL)
DEF_OFFSET(IIntProperty, gWeaponExposureProp, 0x006C315C, NULL, 0x0084518C, NULL)
DEF_OFFSET(IStringProperty, g_pPrjSoundProp, 0x006C2FD0, NULL, 0x00845000, NULL)
DEF_OFFSET(IStringProperty, actorTagListProp, 0x006C52C4, NULL, 0x0084B494, NULL)
DEF_OFFSET(IStringProperty, modelnameprop, 0x00689D90, NULL, NULL, NULL)
DEF_OFFSET(IBoolProperty, g_pLocalCopyProp, 0x006C9484, NULL, 0x0084F6A4, NULL)
DEF_OFFSET(IBoolProperty, g_pAINoGhostProperty, 0x006C2808, NULL, 0x008447F0, NULL)
DEF_OFFSET(IBoolProperty, g_pAINoHandoffProperty, 0x006C2804, NULL, 0x008447EC, NULL)
DEF_OFFSET(IBoolProperty, pLockedProp, 0x006C2DF0, NULL, 0x00844E20, NULL)
DEF_OFFSET(IBoolProperty, pBeingTakenProp, 0x006F55EC, NULL, 0x00A3F73C, NULL)
DEF_OFFSET(IBoolProperty, g_pAISeesProjectileProperty, 0x006C299C, NULL, NULL, NULL)
DEF_OFFSET(IBoolProperty, g_pAIIsProxyProperty, 0x006C2800, NULL, NULL, NULL)
DEF_OFFSET(IFloatProperty, gAlphaRenderProp, 0x006B8BAC, NULL, 0x0083A97C, NULL)
DEF_OFFSET(IFloatProperty, gAvatarHeightOffsetProp, 0x006C9480, NULL, 0x0084F6A0, NULL)
DEF_OFFSET(IFloatProperty, g_pPickDistProp, 0x006C2FC0, NULL, 0x00844FEC, NULL)
DEF_OFFSET(IFloatProperty, g_pPickBiasProp, 0x006C2FBC, NULL, 0x00844FF0, NULL)
DEF_OFFSET(IIntProperty, pCreatureProp, 0x006C5358, NULL, 0x0084B52C, NULL)
DEF_OFFSET(IIntProperty, gPropHeartbeat, 0x006C947C, NULL, 0x0084F69C, NULL)
DEF_OFFSET(IIntProperty, g_pPropSpeechVoiceIndex, 0x006B9074, NULL, NULL, NULL)
DEF_OFFSET(IIntProperty, g_pPropSchemaLastSample, 0x006B8D70, NULL, NULL, NULL)
DEF_OFFSET(IGenericProperty, g_pPhysAttrProp, 0x006C3F7C, NULL, 0x0084A0E4, NULL)
DEF_OFFSET(IGenericProperty, g_pPhysTypeProp, 0x006C3F78, NULL, 0x0084A0E0, NULL)
DEF_OFFSET(IGenericProperty, g_pScriptProp, 0x006B8E78, NULL, 0x0083AD10, NULL)
DEF_OFFSET(IGenericProperty, g_pSpeechProp, 0x006B9364, NULL, NULL, NULL)
DEF_OFFSET(ILabelProperty, g_pPropSpeechVoice, 0x006B9068, NULL, NULL, NULL)
DEF_OFFSET(ILabelProperty, gCombineTypeProp, 0x006C2C68, NULL, NULL, NULL)
DEF_OFFSET(IVectorProperty, g_pPhysInitVelProp, 0x006C3F64, NULL, NULL, NULL)
DEF_OFFSET(IBoolProperty, g_pNoDropProperty, 0x006F55E8, NULL, NULL, NULL)
DEF_OFFSET(IAIRatingProperty, g_pAIDefensiveStatProp, 0x006C2874, NULL, NULL, NULL)
DEF_OFFSET(IAIRatingProperty, g_pAIDodginessStatProp, 0x006C2880, NULL, NULL, NULL)

// Property descs
DEF_VAR(sPropertyDesc*, g_ModelNameDesc, 0x00637160, NULL, NULL, NULL)
DEF_VAR(sPropertyDesc*, g_MeshAttachDesc, 0x00637908, NULL, NULL, NULL)
DEF_VAR(sPropertyDesc*, g_PickStateDesc, 0x00674C20, NULL, NULL, NULL)
DEF_VAR(sPropertyDesc*, g_CurWpnDmgDesc, 0x006558B0, NULL, NULL, NULL)
DEF_VAR(sPropertyDesc*, g_InvBeingTakenDesc, 0x00671A48, NULL, NULL, NULL)
DEF_VAR(sPropertyDesc*, g_SelfIllumDesc, 0x0063BD98, NULL, NULL, NULL)
DEF_VAR(sPropertyDesc*, g_AmbientHackedDesc,  0x0062E8A8, NULL, NULL, NULL)
DEF_VAR(sPropertyDesc*, g_StackCountDesc,  0x006501F0, NULL, NULL, NULL)
DEF_VAR(sPropertyDesc*, g_DeathStageDesc, 0x0062F7F0, NULL, NULL, NULL)

 // Dyn arrays
DEF_VAR(cDynArray<int>*, g_SpeechHandles, 0x006B90F4, NULL, NULL, NULL)

// Relations
DEF_OFFSET(IRelation, ppNowPickingRelation, 0x006F5B18, NULL, 0x00A3FCD0, NULL)
DEF_OFFSET(IRelation, ppVoiceLink, 0x006B9374, NULL, NULL, NULL)
DEF_OFFSET(IRelation, g_pPlayerFactoryRelation, 0x006900A4, NULL, NULL, NULL)
DEF_OFFSET(IRelation, ppFrobProxyRel, 0x006C2CE8, NULL, NULL, NULL)
DEF_OFFSET(IRelation, g_pAIProjectileRelation, 0x006C288C, NULL, NULL, NULL)

// Global variables
DEF_VAR(DWORD*, _highlit_obj, 0x00689E58, NULL, 0x0080B948, NULL)
DEF_VAR(DWORD*, _g_PickCurrentObj, 0x006C2E10, NULL, 0x00844E40, NULL)
DEF_VAR(DWORD*, _frobWorldSelectObj, 0x006C2D4C, NULL, 0x00844D6C, NULL)
DEF_VAR(DWORD*, _g_arrowObj, 0x006F5A6C, NULL, 0x00A3FC24, NULL)
DEF_VAR(PlayerCamera**, _gPlayerCam, 0x0069009C, NULL, 0x00811D00, NULL)
DEF_VAR(int*, _gNoMoveKeys, 0x00687E1C, NULL, NULL, NULL)
DEF_VAR(DWORD*, _gPlayerObj, 0x00690098, 0x006C96A8, 0x00811CFC, NULL)
DEF_VAR(DWORD*, pConsoleState, 0x007BED75, NULL, NULL, NULL)
DEF_VAR(DWORD*, pMissFlags, 0x006F53D4, NULL, 0x00A3F520, NULL)
DEF_VAR(int*, pSlowFrame, 0x006B8FA0, NULL, NULL, NULL)
//DEF_VAR(TextboxState*, _CmdTerm, 0x007BEB40, NULL, 0x01044EC0, NULL)
//DEF_VAR(char*, ConsoleHandle, 0x007BEBB9, NULL, 0x01044F39, NULL)
DEF_VAR(char*, pChNil, 0x006F5E2C, NULL, 0x00ABE424, NULL)
DEF_VAR(IRes**, DarkMessageFont, 0x006F5420, NULL, 0x00A3F570, NULL)
DEF_VAR(int*, mission_map, 0x00671214, NULL, 0x007DD554, NULL)
DEF_VAR(_TagFileTag*, GhostRemTag, 0x0066BC40, NULL, 0x00740F68, NULL)
DEF_VAR(char*, cmdterm_text, 0x007BEC71, NULL, 0x01044FF1, NULL)
DEF_VAR(int*, net_cap_physics, 0x006C3DA4, NULL, 0x00849F0C, NULL)
DEF_VAR(char*, g_gamesysName, 0x00686EB0, NULL, NULL, NULL)
DEF_VAR(float*, g_ModeBaseSpeeds, 0x00639678, NULL, NULL, NULL)
DEF_VAR(BOOL*, g_AllPlayersFinishedSynch, 0x006C94A0, NULL, NULL, NULL)
DEF_VAR(int*, g_NumPlayersNotInGameMode, 0x006C9490, NULL, NULL, NULL)
DEF_VAR(BOOL*, g_bSpeechDatabaseLoaded, 0x006B9354, NULL, NULL, NULL)
DEF_VAR(cSpeechDomain*, g_Domain, 0x006B9078, NULL, NULL, NULL)
DEF_VAR(sSchemaPlay*, g_pLastSchemaPlay, 0x006B8D1C, NULL, NULL, NULL)
DEF_VAR(SchemaPlayList*, playingSchemas, 0x006B8D18, NULL, NULL, NULL)
DEF_VAR(char**, g_AIActTypeNames, 0x0064067C, NULL, NULL, NULL)
DEF_VAR(char**, g_ppszAISpeed, 0x006410D0, NULL, NULL, NULL)
DEF_VAR(float**, g_AISpeeds, 0x006413F8, NULL, NULL, NULL)
DEF_VAR(DWORD*, dword_6F55D4, 0x6F55D4, NULL, NULL, NULL)
DEF_VAR(DWORD*, dword_6F55DC, 0x6F55DC, NULL, NULL, NULL)
DEF_VAR(DWORD*, dword_671B8C, 0x671B8C, NULL, NULL, NULL)
DEF_VAR(const sDarkPanelDesc*, g_SynchPanelDesc, 0x0066D830, NULL, NULL, NULL)
DEF_OFFSET(ISearchPath, _gContextPath, 0x006B8BEC, NULL, NULL, NULL)
DEF_VAR(char**, g_LoopMsgNames, NULL, NULL, 0x007F2E90, NULL)
DEF_VAR(guiStyle*, g_DefaultPanelStyle, 0x0066BA80, NULL, NULL, NULL)
DEF_VAR(sAIAwareDelay*, g_pAIDefAwareDelay, 0x0064A918, NULL, NULL, NULL)
DEF_VAR(BOOL*, enable_inv, 0x00671DB8, NULL, NULL, NULL)

// drkwbow
DEF_VAR(int*, g_TotalBowFrames, 0x006F59A0, NULL, NULL, NULL)
DEF_VAR(long*, g_BowEquippedTime, 0x006F599C, NULL, NULL, NULL)
DEF_VAR(int*, g_BowAttackStarted, 0x00673CEC, NULL, NULL, NULL)
DEF_VAR(BOOL*, g_BowIsForceFinished, 0x00673CF0, NULL, NULL, NULL)
DEF_VAR(float*, g_BowWobble, 0x006F5A20, NULL, NULL, NULL)
DEF_VAR(int*, g_BowPutAwayTime, 0x006F5940, NULL, NULL, NULL)
DEF_VAR(BOOL*, arrowNocked, 0x00673CD8, NULL, NULL, NULL)

// Struct descs
DEF_VAR(const sStructDesc*, pSD_eAIPriority, 0x641148, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_tMetaPropertyPriority, 0x65AE90, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_ContainRelType, 0x650180, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAmplifyParam, 0x659AB0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sWeakPointParam, 0x659CE8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sStimSourceData, 0x65A478, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sStimSourceDesc, 0x65A598, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_eSlayResult, 0x62F988, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_PropagateSourceScale, 0x63DC18, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sFlinder, 0x63DDA8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sReceptron, 0x65A1C0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_tSensorCount, 0x659F30, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_ReceptronID, 0x65A290, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sStimulateParam, 0x65A7B0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_eInventoryType, 0x6718F8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_BOOL, 0x65AA50, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_int, 0x65AAC0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_float, 0x65AB30, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_Label, 0x65ABA0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_Vector, 0x65AC10, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_CollType, 0x65B3A8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sLightProp, 0x635E28, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_ColorInfo, 0x635F18, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAnimLightProp, 0x6362A0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sExtraLightProp, 0x6301A0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_Position, 0x637EB8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMotActorTagList, 0x65F6D0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_tMotorControllerType, 0x65F770, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_eMSwordActionType, 0x65F878, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMotPhysLimits, 0x65F980, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMGaitSkillData, 0x65FF40, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMPlayerLimbOffsets, 0x65FA60, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sScriptProp, 0x63D710, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sStimMsgParam, 0x657D18, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_TrapFlags, 0x635310, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sHeatDiskCluster, 0x635068, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sSpark, 0x63E960, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMeshAttach, 0x6378D8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sFrobInfo, 0x651300, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_FrobProxyInfo, 0x651410, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_eFrobHandler, 0x6514C0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sRotDoorProp, 0x650940, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTransDoorProp, 0x650C60, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTweqSimpleConfig, 0x6541F0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTweqVectorConfig, 0x654220, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTweqJointsConfig, 0x654250, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTweqLockConfig, 0x6542E0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTweqModelsConfig, 0x654280, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTweqEmitterConfig, 0x6542B0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTweqSimpleState, 0x654310, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTweqLockState, 0x6543A0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTweqVectorState, 0x654340, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTweqJointsState, 0x654370, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTweqControl, 0x6543D0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sWeaponOffset, 0x6556F8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sKeyInfo, 0x651D20, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sInvRenderType, 0x651AC0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sSuspiciousInfo, 0x63EB78, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sQVarParam, 0x6524B8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sBashParamsProp, 0x650FF0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sBashVars, 0x650EA8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sCollParam, 0x658B38, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sPermeate, 0x658F48, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sPropAddParam, 0x6590E0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sPropRemParam, 0x6591D8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sObjCreateParam, 0x6594A0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sObjMoveParam, 0x659768, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sDamageParam, 0x62FBD0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sSpoofDamageParam, 0x62FCC8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sSetModelParam, 0x672C90, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAware, 0x672EA8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sCreatureAttachInfo, 0x6602B8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sCreatureHandle, 0x660508, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sCreaturePose, 0x6606B0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAdvPickTransCfg, 0x676218, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAdvPickSoundCfg, 0x676278, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAdvPickStateCfg, 0x676248, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_PickSrc, 0x6762A8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sPickCfg, 0x6762D8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sPickState, 0x676308, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sBeltLink, 0x676AA0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAltLink, 0x676C40, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sBreathConfig, 0x66FA88, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sCombatVars, 0x66FD50, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cBaseGunDescs, 0x656420, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIGunDesc, 0x656600, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sGunState, 0x6574E0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sProjectileData, 0x657B80, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sProjectile, 0x657A30, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sFlashData, 0x655BC8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sLootProp, 0x6749E0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMissionData, 0x6716C0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMapSourceData, 0x66F340, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAutomapProperty, 0x66F450, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_tQVarVal, 0x639CB0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sVoiceData, 0x673C18, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_tDarkStat, 0x6739B8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIProp, 0x64A0A8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIEfficiency, 0x64A370, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIAlertness, 0x64A7C0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIFrustrated, 0x64A6D0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIAlertCap, 0x64A8E8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIVisibility, 0x64AC40, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_eAIMode, 0x64ACF8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_eAITeam, 0x64A4C8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_VisionJoint, 0x64C660, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIVisionDesc, 0x64BA98, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIVisibilityMods, 0x64C118, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIVisibilityControl, 0x64C478, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIAwareCapacitor, 0x64C598, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIAlertSenseMults, 0x64BF38, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIAwareDelay, 0x64AAA0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cAIDeviceParams, 0x64D1E8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cAITurretParams, 0x64D348, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cAICameraParams, 0x64D468, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_ActorID, 0x649E38, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_eAIRating, 0x64D668, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_eAIInvestKind, 0x649870, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_eAINonHostility, 0x649930, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cAINonCombatDmgResp, 0x649A30, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIProjectileRel, 0x64DD20, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIRangedCombatProp, 0x64E1F0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAISoundType, 0x64EA18, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sVantagePtProp, 0x64DDD0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sCoverPtProp, 0x64DEF0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAngleLimitProp, 0x64DFD0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIPathOptions, 0x6494D0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIGamesysPathOptions, 0x649578, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAICreatureSizes, 0x640968, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAICombatTiming, 0x6442D0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIRangedRanges, 0x64E370, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cAIRangedWound, 0x64E4B0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIRangedApplicabilities, 0x6437C0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIRangedFleeParams, 0x6439B8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIRangedShootParams, 0x643C88, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIFleeConditions, 0x6464E0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAINoFleeLink, 0x646660, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIFleeDest, 0x6465F0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIWatchPoint, 0x649318, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIDefendPoint, 0x646398, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIIdleDirs, 0x646DF8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIIdleOrigin, 0x646ED8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAICameraLinkData, 0x6445C8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIAcuitySets, 0x64FA10, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAISoundTweaks, 0x64F518, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sHearingStats, 0x64F308, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIBroadcastSettings, 0x641E58, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIFollowLink, 0x646A28, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAISignalResponse, 0x6477B8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIThreatResponse, 0x6487B8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIAlertResponse, 0x6450A8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIBodyResponse, 0x6457D8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAISuspiciousResponse, 0x648010, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIObjectAvoid, 0x6405C0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIInformResponse, 0x640E28, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAISenseCombatResponse, 0x645F68, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIFrozen, 0x646B08, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sFreezeReactionParam, 0x658E00, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAIAwareness, 0x64EEB8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAISuspiciousLink, 0x642A90, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMissionSongParams, 0x63E1C0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_AmbientSound, 0x62E878, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAcousticsProperty, 0x658470, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAmbientProperty, 0x658550, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cPhysTypeProp, 0x65DF60, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cPhysAttrProp, 0x65DF90, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cPhysStateProp, 0x65E020, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cPhysControlProp, 0x65E050, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cPhysDimsProp, 0x65E080, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cMovingTerrainProp, 0x65E470, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_tCanAttachType, 0x65E848, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sPhysExplodeProp, 0x65C8B0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sPhysRopeProp, 0x65C7C0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sPhysPPlateProp, 0x65C688, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sTerrainPath, 0x65E310, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sPhysAttachData, 0x65CA10, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sSchemaLoopParams, 0x63CD38, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sSchemaPlayParams, 0x63CBD8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sSpeech, 0x63E358, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sESndTagList, 0x6302D0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sEnvSoundReactionParam, 0x658D08, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_tRendType, 0x63BE48, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sJointPos, 0x63C280, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_ParticleGroup, 0x638CB0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_ParticleLaunchInfo, 0x638FE0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sParticleAttachLinkData, 0x639180, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_FrameAnimationState, 0x630C78, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_FrameAnimationConfig, 0x630C48, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_FlashbombRender, 0x63C868, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cBitmapWorldspaceProperty, 0x63BFD0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_tBitmapAnimationType, 0x63C078, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sAnimTexProp, 0x63ECF0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sSkyMode, 0x639E08, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMissionRenderParams, 0x6370A0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sRGBA, 0x634728, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sWaterBanks, 0x634B88, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMissionSkyObj, 0x63A4B8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMissionStarObj, 0x63A698, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMissionCelestialObj, 0x63AA98, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMissionCloudObj, 0x63BA18, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_sMissionDistantObj, 0x63B078, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_eNetworkCategory, 0x66D418, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cWeatherProperty, 0x63F3D8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cMeshTexProperty, 0x636CD0, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cFaceProperty, 0x6305C8, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cFaceStateProperty, 0x630740, NULL, NULL, NULL)
DEF_VAR(const sStructDesc*, pSD_cCoronaProperty, 0x62F5A8, NULL, NULL, NULL)

// Struct field descs
DEF_VAR(const sFieldDesc*, pFD_MissionFlags, 0x006713A8, NULL, NULL, NULL)

// Structs
DEF_OFFSET(grd_canvas, _grd_canvas, 0x007CF9A4, NULL, 0x01056784, NULL)
DEF_VAR(cHashSet*, _gGhostRemotes, 0x006C93D0, NULL, NULL, NULL)
DEF_VAR(sFrobActivate*, g_currFrob, 0x006F5590, NULL, NULL, NULL)

//======================================================================================
// Function pointer definitions.
//======================================================================================
DEF_FUNCPTR(void*, __stdcall, _AppGetAggregated, (const GUID&), 0x00576930, 0x0059D4B0, 0x00610D10, NULL)
DEF_FUNCPTR(void*, __cdecl, DarkNew, (size_t), 0x005EC92D, 0x006127A0, 0x0069A0DD, NULL)
DEF_FUNCPTR(void,, DarkFree, (void*), 0x005FE010, NULL, 0x006AC010, NULL)
DEF_FUNCPTR(void,, dark_delete, (void*), 0x005EC320, NULL, NULL, NULL)
DEF_FUNCPTR(const char*,, dark_strdup, (const char*), 0x005F8A48, NULL, NULL, NULL)
DEF_FUNCPTR(enum eWinPumpResult, __stdcall, PumpEvents, (DWORD*, int, enum  eWinPumpDuration), 0x0057E350, 0x005A55C0, 0x0061A810, NULL)
DEF_FUNCPTR(void,, _quit_game, (), 0x004137D0, NULL, 0x00418A10, NULL)
DEF_FUNCPTR(cAnsiStr,, FetchUIString, (const char* fileName, const char* placeholderStr, const char* resFile), 0x005306A0, NULL, 0x00585D80, NULL)
DEF_FUNCPTR(IDarkDataSource*,, FetchUIImage, (const char*, const char* resFile), 0x00530680, NULL, NULL, NULL)
DEF_FUNCPTR(int,, FetchUIRects, (const char* rectsBin, cDynArray<Rect> & rects, const char* resFile), 0x005307A0, NULL, NULL, NULL)
DEF_FUNCPTR(const char*,, _extension_p, (const char* str), 0x00401790, NULL, NULL, NULL)
DEF_FUNCPTR(sMissionData*,, _GetMissionData, (), 0x0055E320, NULL, NULL, NULL)
DEF_FUNCPTR(const char*, __stdcall, LoopGetMessageName, (int), NULL, NULL, 0x00659A90, NULL)
DEF_FUNCPTR(void,, UpdateLootTotals, (int invLoot, sLoot* pNewValues), 0x0055CA20, NULL, NULL, NULL)

// ai
DEF_FUNCPTR(int,, _AIGetStun, (int objectID), 0x004714F0, NULL, NULL, NULL)
DEF_FUNCPTR(int, __stdcall, AwarenessFilter, (sReactionEvent*, const sReactionParam*, DWORD), 0x0056A570, NULL, NULL, NULL)

// automap
DEF_FUNCPTR(sAutomapProperty*,, AutomapGetCurrentRoom, (), 0x00553580, NULL, NULL, NULL)
DEF_FUNCPTR(void,, AutomapHighlightArea, (IRes*, Rect*, int), 0x00553D90, NULL, NULL, NULL)

// bow
DEF_FUNCPTR(int,, _IsBowEquipped, (), 0x0056E3F0, NULL, NULL, NULL)
//DEF_FUNCPTR(void,, _ForceFinishBowAttack, (int), 0x0056E2B0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, WobbleBow, (long), 0x0056E450, NULL, NULL, NULL)
DEF_FUNCPTR(void,, UpdateBowTime, (int, int), 0x0056E150, NULL, NULL, NULL)
DEF_FUNCPTR(BOOL,, _RenockBow, (long), 0x0056E680, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ClearPlayerArmFilter, (), 0x00434A70, NULL, NULL, NULL)
DEF_FUNCPTR(void,, ZoomClear, (), 0x0056DFE0, NULL, NULL, NULL)
DEF_FUNCPTR(int,, DoBowAttack, (), 0x0056E780, NULL, NULL, NULL)
DEF_FUNCPTR(void,, PutTheBowAway, (int), 0x0056E280, NULL, NULL, NULL)
DEF_FUNCPTR(void,, sub_56EB50, (), 0x0056EB50, NULL, NULL, NULL)

// creature
DEF_FUNCPTR(void,, _CreatureAttachItem, (int creatureObj, int weaponObj, WeaponAttachment *), 0x005276D0, NULL, 0x0057A700, NULL)
DEF_FUNCPTR(void,, _CreatureDetachItem, (int creatureObj, int weaponObj),  0x00527730, NULL, 0x0057A760, NULL)
DEF_FUNCPTR(void,, _CreatureAttachWeapon, (int creatureObj, int weaponObj, WeaponAttachment *), 0x00527580, NULL, 0x0057A5B0, NULL)
DEF_FUNCPTR(cCreature*,, _CreatureFromObj, (int objectID), 0x00528A30, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _ObjGetCreatureType, (int objectID, int& creatureType), 0x0052DA30, NULL, 0x00582A70, NULL)
DEF_FUNCPTR(void,, _ObjSetCreatureType, (int objectID, int creatureType), 0x0052DA00, NULL, 0x00582A10, NULL)
DEF_FUNCPTR(sCreatureHandle*,, _CreatureHandle, (int handleID), 0x0052DAB0, NULL, NULL, NULL)
DEF_FUNCPTR(IMotor*,, CreatureGetMotorInterface, (int objectID), 0x005283A0, NULL, NULL, NULL)


// dark
DEF_FUNCPTR(void,, _dark_init_game, (), 0x0055A680, NULL, 0x005F1950, NULL)
DEF_FUNCPTR(void,, _dark_term_game, (), 0x0055AB90, NULL, 0x005F1EA0, NULL)
DEF_FUNCPTR(void,, _DarkStatIntAdd, (const char* statName, int amount), 0x0056D330, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _DarkStatIntGet, (char *, int), 0x0056D290, NULL, 0x00606900, NULL)
DEF_FUNCPTR(void,, _DarkAutomapSetLocationVisited, (int page, int location), 0x00553470, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _DarkAutomapGetLocationVisited, (int page, int location), 0x00553430, NULL, NULL, NULL)

// gr
DEF_FUNCPTR(grs_bitmap*,, _gr_alloc_bitmap, (int, int, int, int), 0x0057EB50, NULL, 0x0061BBC0, NULL)
DEF_FUNCPTR(void,, _gr_make_canvas, (grs_bitmap*, grd_canvas*), 0x0057EFE0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _gr_push_canvas, (grd_canvas*), 0x0057EDA0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _gr_pop_canvas, (), 0x0057EDE0, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _gr_make_screen_fcolor, (int hexColor), 0x005810E0, NULL, NULL, NULL)

typedef void (*_UIRegionHandler)(struct _ui_event*, _Region*, void*);

// gui // ui
DEF_FUNCPTR(int,, _guiScreenColor, (int hexColor), 0x005A0980, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _GUIsetup, (GUIsetupStruct*, const Rect*, int, int), 0x005A05A0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _GUIdone, (GUIsetupStruct*), 0x005A0670, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _uiGameLoadStyle, (const char* panelPrefix, guiStyle* pStyle, const char* resFile), 0x00458640, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _uiGameUnloadStyle, (guiStyle* pStyle), 0x00458760, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _DarkMessage, (const char*), 0x0055E660, NULL, 0x005F6200, NULL)
DEF_FUNCPTR(void,, _gr_font_string, (void* fontResource, const char* fontString, int xpos, int ypos), 0x0058CD50, NULL, 0x0062A570, NULL)
DEF_FUNCPTR(void,, _gr_font_string_wrap, (void* fontResource, const char* fontString, short width), 0x0058D040, NULL, 0x0062A860, NULL)
DEF_FUNCPTR(void,, _gr_font_string_size, (void* fontResource, const char* fontString, short* stringWidth, short* stringHeight), 0x0058CF70, NULL, 0x0062A790, NULL)
DEF_FUNCPTR(void,, _gr_font_string_unwrap, (const char*), 0x0058D120, NULL, NULL, NULL)
DEF_FUNCPTR(short,, _gr_font_string_height, (void* fontResource, const char* str), 0x0058CF40, NULL, NULL, NULL)
DEF_FUNCPTR(short,, _gr_font_string_width, (void* fontResource, const char* str), 0x0058CED0, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _guiStyleGetColor, (guiStyle* style, int), 0x005A09E0, NULL, 0x00643070, NULL)
DEF_FUNCPTR(void,, _guiStyleCleanupFont, (guiStyle*, int index), 0x005A0A90, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _guiStyleSetupFont, (guiStyle*, int index), 0x005A0A30, NULL, 0x006430C0, NULL)
/* uiInstallRegionHandler:
/* int - unknown
/* int - unknown flag
/* _UIRegionHandler - region processing func
/* pData - extra data (passed to region handler)
/* int - unknown
*/
DEF_FUNCPTR(void,, _uiInstallRegionHandler, (int, int, _UIRegionHandler, void* pData, int), 0x0059D1E0, NULL, NULL, NULL)
DEF_FUNCPTR(guiStyle*,, _GetCurrentStyle, (), 0x005A0870, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SetCurrentStyle, (guiStyle* pStyle), 0x005A0860, NULL, NULL, NULL) // do not use in game mode
DEF_FUNCPTR(IRes*,, _guiStyleGetFont, (int, int), 0x005A0A10, NULL, NULL, NULL)

// mouse
DEF_FUNCPTR(short,, _mouse_get_xy, (short* x, short* y), 0x00597090, NULL, NULL, NULL)
DEF_FUNCPTR(short,, _mouse_put_xy, (short x, short y), 0x00597110, NULL, NULL, NULL)

// pal
DEF_FUNCPTR(BYTE,, _palmgr_alloc_pal, (void*), 0x0042AC60, NULL, 0x00436920, NULL)

// property create
DEF_FUNCPTR(IStringProperty*,, _CreateStringProperty, (const sPropertyDesc* pDesc, int), 0x004EBD10, NULL, 0x0052DD60, NULL)

// loop 
DEF_FUNCPTR(int, __stdcall, DarkSimulationLoop, (DWORD P1, DWORD flags, tLoopMessageData__* pMsgData), 0x56C600, NULL, 0x605A50, NULL) 
DEF_FUNCPTR(int, __stdcall, ObjectSystemLoopClient, (DWORD P1, DWORD flags, tLoopMessageData__* pMsgData), 0x004211A0, NULL, NULL, NULL)
DEF_FUNCPTR(int, __stdcall, GameModeLoopClient, (DWORD P1, DWORD flags, tLoopMessageData__* pMsgData), 0x004139A0, NULL, NULL, NULL)
DEF_FUNCPTR(int, __stdcall, DarkRenderingLoopClient, (DWORD, DWORD, tLoopMessageData__* pMsgData), 0x0056AAC0, NULL, 0x00603DF0, NULL)
DEF_FUNCPTR(int, __stdcall, SimLoopClient, (DWORD, DWORD, tLoopMessageData__* pMsgData), 0x0044B820, NULL, NULL, NULL)

// input binder
DEF_FUNCPTR(void,, _InstallIBHandler, (long contextLong, int, int contextInt), 0x00415760, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _RemoveIBHandler, (), 0x004157D0, NULL, NULL, NULL)

// mode switch
DEF_FUNCPTR(void,, _PushMovieOrBookMode, (const char* name), 0x0055E150, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SwitchToNewGameMode, (int), 0x0055CEA0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SwitchToMainMenuMode, (int), 0x0055CE20, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SwitchToOptionsMode, (int), 0x00561D50, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SwitchToLoadGameMode, (int), 0x0056B950, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SwitchToSaveGameMode, (int), 0x0056BCC0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SwitchToObjectivesMode, (int), 0x00558E00, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SwitchToDarkAutomapMode, (int), 0x00554930, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SwitchToSimMenuMode, (int), 0x0055CE60, NULL, NULL, NULL)

// player
DEF_FUNCPTR(void,, _PlayerCreate, (void), 0x00432810, NULL, 0x0043FBB0, NULL)
DEF_FUNCPTR(void,, _PlayerDestroy, (void), 0x004327A0, NULL, NULL, NULL)
DEF_FUNCPTR(ePlayerMode,, _GetPlayerMode, (void),  0x004330D0, NULL, 0x00440640, NULL)
DEF_FUNCPTR(void,, _SetPlayerMode, (int),  0x004330E0, NULL, 0x00440650, NULL)
DEF_FUNCPTR(int,, _IsAPlayer, (int objectID), 0x00432700, NULL, 0x0043FAA0, NULL)
DEF_FUNCPTR(int,, _PlayerMotionGetActive, (), 0x0051DB30, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _PlayerMotionActivate, (ePlayerMotion mot), 0x0051DB10, NULL, NULL, NULL)
DEF_FUNCPTR(float,, _GetTransSpeedScale, (), 0x00433140, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _AddSpeedScale, (const char*, float, float), 0x00433100, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _PlayerFinishAction, (), 0x00432060, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SetLeaningState, (int state), 0x004331D0, NULL, NULL, NULL)

// object
DEF_FUNCPTR(int,, _ObjGetMaxHitPoints, (int object, int* hitPoints), 0x004194D0, NULL, 0x0041F230, NULL)
DEF_FUNCPTR(int,, _ObjSetHitPoints, (int object, int hitPoints), 0x00419480, NULL, 0x0041F1E0, NULL)
DEF_FUNCPTR(int,, _ObjGetHitPoints, (int object, int* hitPoints), 0x00419430, NULL, 0x0041F190, NULL)
DEF_FUNCPTR(char*,, _ObjEditName, (int object), 0x00429270, 0x00424540, 0x00434DB0, NULL)
DEF_FUNCPTR(Position*,, _ObjPosGet, (int object), 0x00422460, NULL, 0x0042D350, NULL)
DEF_FUNCPTR(void,, _ObjPosCopyUpdate, (int object, Position* pos), 0x00422750, NULL, NULL, NULL)
DEF_FUNCPTR(mxs_vector*,, _ObjPosGetLocVector, (int object), 0x004224A0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ObjPosSetLocation, (int objectID, Position*), 0x00422670, NULL, 0x0042D6C0, NULL)
DEF_FUNCPTR(int,, _ObjRenderType, (int object), 0x00442970, NULL, 0x00452AD0, NULL)
DEF_FUNCPTR(void,, _ObjSetRenderType, (int object, int renderType), 0x004429A0, NULL, 0x00452B20, NULL)
DEF_FUNCPTR(void,, _ObjSetHasRefs, (int object, int hasRefs), 0x00442950, NULL, 0x00452AA0, NULL)
DEF_FUNCPTR(int,, _ObjGetModelName, (int object, char* buffer), 0x0041EA20, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ObjSetModelName, (int object, char* modelName), 0x0041EA70, NULL, 0x00426E20, NULL)
DEF_FUNCPTR(void,, _ObjGetESndClass, (int object, void** pEsnd), 0x0040DE30, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ObjSetScale, (int object, mxs_vector* vec), 0x00444E20, NULL, 0x004554D0, NULL)
DEF_FUNCPTR(void,, _ObjGetWeaponType, (int object, int* type), 0x0040C090, NULL, 0x0040F060, NULL)
DEF_FUNCPTR(int,, _ObjIsAI, (int object), 0x004680D0, NULL, 0x004827E0, NULL)
DEF_FUNCPTR(int,, _GetTextureObj, (int), 0x00457870, NULL, NULL, NULL)
DEF_FUNCPTR(struct sFrobInfo*,, _ObjFrobResult, (int), 0x004AF280, NULL, NULL, NULL)
DEF_FUNCPTR(BYTE,, _ObjFrobResultForLoc, (int actionType, int objectID), 0x004AF2B0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ObjSetSelfLocked, (int object, int lockState), 0x004B2EA0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ObjTranslate, (int object, mxs_vector* vec), 0x004225E0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ObjRotate, (int object, mxs_angvec* ang), 0x00422570, NULL, NULL, NULL)

// rendobj
DEF_FUNCPTR(int,, _rendobj_object_is_visible, (int object), 0x00441B90, NULL, 0x0044EBD0, NULL)

// mx
DEF_FUNCPTR(void,, _mx_zero_vec, (mxs_vector &), 0x00577D10, NULL, 0x00612A90, NULL)
DEF_FUNCPTR(void,, _mx_copy_trans, (mxs_trans* oldtrans, mxs_trans* newtrans), 0x00579100, NULL, 0x00613F00, NULL)
DEF_FUNCPTR(float,, _mx_mag_vec, (mxs_vector &), 0x00578020, NULL, 0x00612E00, NULL)
DEF_FUNCPTR(float,, _mx_mag2_vec, (mxs_vector*), 0x00577FF0, NULL, NULL, NULL)
DEF_FUNCPTR(float,, _mx_dist_vec, (mxs_vector*, mxs_vector*), 0x00577FB0, NULL, 0x00612D90, NULL)

// quicksave/load
DEF_FUNCPTR(void,, _DarkQuickSaveGame, (void), 0x0056B560, NULL, 0x00604920, NULL)
DEF_FUNCPTR(void,, _DarkQuickLoadGame, (void), 0x0056B600, NULL, 0x006049C0, NULL)
DEF_FUNCPTR(HRESULT,, _DarkSaveGameFile, (char* desc, char* path), 0x0056AFD0, NULL, 0x00604320, NULL)
DEF_FUNCPTR(HRESULT,, _DarkQuickLoadGameFile, (char*), 0x0056B670, NULL, 0x00604A30, NULL)

// sim
DEF_FUNCPTR(void,, _SimStatePause, (void), 0x0044C650, NULL, 0x0045F070, NULL)
DEF_FUNCPTR(void,, _SimStateUnpause, (void), 0x0044C690, NULL, 0x0045F0B0, NULL)
DEF_FUNCPTR(int,, _SimStateCheckFlags, (DWORD flags), 0x0044C610, NULL, 0x0045F030, NULL)
DEF_FUNCPTR(void,, _SimStateSetFlags, (DWORD flags, BOOL clear), 0x0044C620, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _GetSimTime, (), 0x0044B770, NULL, 0x0045DE90, NULL)
DEF_FUNCPTR(int,, _GetSimFrameTime, (), 0x0044B780, NULL, 0x0045DEA0, NULL)

// mission
DEF_FUNCPTR(void,, _DarkLootPrepMission, (void), 0x0055C770, NULL, 0x005F3FF0, NULL)
DEF_FUNCPTR(void,, _DarkPrepLevelForDifficulty, (void), 0x0052F550, NULL, 0x00584AC0, NULL)
DEF_FUNCPTR(void,, _dark_end_gamemode, (void), 0x0055A450, NULL, 0x005F15C0, NULL)
DEF_FUNCPTR(void,, _dark_start_gamemode, (DWORD), 0x0055A2D0, NULL, 0x005F13F0, NULL)
DEF_FUNCPTR(void,, _win_mission, (), 0x0055A4D0, NULL, NULL, NULL)

// sound
DEF_FUNCPTR(int,, _GenerateSound, (char*, sfx_parm*), 0x004CC3F0, NULL, 0x0050A360, NULL)
DEF_FUNCPTR(int,, _GenerateSoundObj, (int objectID,int schemaID,const char* soundName,float atten,struct sfx_parm *,int P3,int P4), 0x004CC380, NULL, 0x0050A2F0, NULL)
DEF_FUNCPTR(int,, GenerateSoundVec, (mxs_vector* vec, int P1, int P2, char* soundName, float attenuation, sfx_parm* parms, int P3, int P4), 0x4CC3B0, NULL, 0x0050A320, NULL) 
DEF_FUNCPTR(void,, _SoundNetGenerateSoundVec, (int handle, mxs_vector* vec, int archetype, char* soundName, float atten, sfx_parm* parms), 0x004B5DB0, NULL, 0x004EDFE0, NULL)
DEF_FUNCPTR(void,, _SoundNetGenerateSoundObj, (int handle, int object, int schemaID, char* soundName, float atten, sfx_parm* parms), 0x004B5C20, NULL, 0x004EDD90, NULL)
DEF_FUNCPTR(void,, _SoundNetHalt, (int handle), 0x004B60E0, NULL, 0x004EE3D0, NULL)
DEF_FUNCPTR(void,, _SchemaPlayHalt, (sSchemaPlay*), 0x00445620, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _SchemaPlayObj, (const char* schemaName, int object, int P3), 0x00445E50, NULL, 0x00456E40, NULL)
DEF_FUNCPTR(sSchemaLoopParams*,, _SchemaLoopParamsGet, (int schemaID), 0x00446620, NULL, 0x00457F70, NULL)
DEF_FUNCPTR(float,, _SchemaAttFacGet, (int schemaID),  0x00446710, NULL, 0x00458060, NULL)
DEF_FUNCPTR(sSchemaPlay*,, _SchemaIDPlay, (int schemaID, sSchemaCallParams* params, void* P3),  0x00445D20, NULL, 0x00456C80, NULL)
DEF_FUNCPTR(void,, SchemaParamsSetup, (int, struct sfx_parm &, DWORD), 0x004458B0, NULL, 0x00456530, NULL)
DEF_FUNCPTR(const char*,, _SchemaSampleGet, (int schemaID, int P1), 0x00446FE0, NULL, 0x00458A90, NULL)
DEF_FUNCPTR(DWORD,, _SchemaGetIDFromHandle, (int handle), 0x00446060, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SFXReset, (void), 0x00403910, NULL, 0x004048A0, NULL)
DEF_FUNCPTR(int,, _SFX_Play_Raw, (int P1, int P2, char*), 0x00404A00, NULL, 0x00405AD0, NULL)
DEF_FUNCPTR(void,, _SFX_Frame, (int, int), 0x00403CC0, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _ESndPlayLoc, (class cTagSet * ,int,int,struct mxs_vector const *,struct sSchemaCallParams*, void *), 0x0040D890, NULL, 0x00410D70, NULL)
DEF_FUNCPTR(int,, _ESndPlay, (class cTagSet * ,int,int,struct sSchemaCallParams*, void *), 0x0040D200, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _LoopSampleCallback, (DWORD P1, DWORD P2), 0x004CE950, NULL, NULL, NULL)
DEF_FUNCPTR(int,, SchemaChooseSample, (int, int), 0x00445710, NULL, NULL, NULL)
DEF_FUNCPTR(sSchemaPlay*,, SchemaPlayAdd, (int schemaID, const sSchemaCallParams*), 0x004454B0, NULL, NULL, NULL)
DEF_FUNCPTR(int,, SchemaSamplePlayAndSetupNext, (int, int, sSchemaPlay*, sSchemaLoopParams*, void*), 0x00445AA0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SoundHaltObj, (int), 0x004CC430, NULL, NULL, NULL)

// time
DEF_FUNCPTR(ulong, __stdcall, tm_get_millisec_unrecorded, (), 0x0059AB30, NULL, NULL, NULL)

typedef void (*_SpeechStartCallback)(int object, const Label* pLabel, int schemaID);

// speech
DEF_FUNCPTR(int,, SpeechGetVoice, (int), 0x004501C0, NULL, NULL, NULL)
DEF_FUNCPTR(BOOL,, _ObjGetSpeechVoiceIndex, (int, int*), 0x0044F550, NULL, NULL, NULL)
DEF_FUNCPTR(void,, SpeechInstallStartCallback, (int index, _SpeechStartCallback), 0x0044FCC0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, SpeechEndCallback, (int, int, void*), 0x0044FF00, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _SpeechHalt, (int object), 0x0044FF80, NULL, NULL, NULL)
DEF_FUNCPTR(sSpeech*,, _SpeechPropCreateAndGet, (int object), 0x0044F220, NULL, NULL, NULL)

// loading
DEF_FUNCPTR(int,, _GetNextMission, (void), 0x0055E090, NULL, 0x005F5C10, NULL)
DEF_FUNCPTR(void,, _SetNextMission, (int), 0x0055E0A0, NULL, 0x005F5C20, NULL)
DEF_FUNCPTR(void,, _MissionLoopReset, (int), 0x0055E0F0, NULL, 0x005F5C70, NULL)
DEF_FUNCPTR(void,, _UnwindToMissionLoop, (void), 0x0055E0C0, NULL, 0x005F5C40, NULL)
DEF_FUNCPTR(void,, _SwitchToLoadingMode, (int), 0x00558F10, NULL, 0x005EFB90, NULL)
DEF_FUNCPTR(void,, _SwitchToNetSynchMode, (int), 0x0053F0B0, NULL, 0x0059D400, NULL)
DEF_FUNCPTR(void,, SwitchToGameMode, (int), 0x0053F1B0, NULL, NULL, NULL)
DEF_FUNCPTR(ITagFile*,, BufTagFileOpen, (const char*, enum TagFileOpenMode), 0x00406640, NULL, NULL, NULL)
DEF_FUNCPTR(long,, DarkLoadGameInternal, (ITagFile*), 0x0056B000, NULL, NULL, NULL)

// inv
DEF_FUNCPTR(void,, _InvUIRefreshObj, (int obj), 0x00560C00, NULL, NULL, NULL)

// db
DEF_FUNCPTR(DWORD,, _dbMergeLoadTagFile, (ITagFile* tag, DWORD P2), 0x0040ACA0, NULL, NULL, 0x0040D310)
DEF_FUNCPTR(DWORD,, _dbCurrentFilenum, (), 0x0040B160, NULL, NULL, NULL)

// commands
DEF_FUNCPTR(void,, _CommandExecute, (char const*), 0x00408920, NULL, 0x0040A840, NULL)
DEF_FUNCPTR(void,, _CommandRegister, (Command*, int nCmds, int P2), 0x00408760, NULL, 0x0040A660, NULL)
DEF_FUNCPTR(Command*,, _CommandFind, (const char* name, int length), 0x004087E0, NULL, NULL, NULL)

// motion
DEF_FUNCPTR(void,, _AddMotionFlagListener, (int object, unsigned long flags, void* pfn), 0x0051F8B0, NULL, NULL, NULL)

// projectile
DEF_FUNCPTR(int,, _GetArrowArchetype, (), 0x0056EB60, NULL, 0x00608490, NULL)
DEF_FUNCPTR(void,, _launchProjectile, (int launcherObject, int projArchetype, float velocity, DWORD flags, DWORD P1, DWORD P2, DWORD P3),  0x004B4400, NULL, 0x004EC530, NULL) 

// phys
DEF_FUNCPTR(void,, _PhysSetFlag, (int objectID, DWORD flags, DWORD P3), 0x0051A1E0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _PhysSetVelocity, (int objectID, mxs_vector *), 0x00518C80, NULL, 0x00568830, NULL)
DEF_FUNCPTR(void,, _PhysSetRotationalVelocity, (int objectID, mxs_vector *), 0x00518D60, NULL, 0x00568990, NULL)
DEF_FUNCPTR(void,, _PhysSetGravity, (int objectID, float grav), 0x0051AA40, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _PhysSetBaseFriction, (int objectID, float friction), 0x0051BE30, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _PhysGetVelocity, (int objectID, mxs_vector* velocity), 0x00518D20, NULL, 0x00568930, NULL)
DEF_FUNCPTR(float,, _PhysGetGravity, (int objectID), 0x0051AAC0, NULL, NULL, NULL)
DEF_FUNCPTR(float,, _PhysGetDensity, (int objectID), 0x0051AC10, NULL, 0x0056AE30, NULL)
DEF_FUNCPTR(float,, _PhysGetMass, (int objectID), 0x0051ABA0, NULL, 0x0056ACD0, NULL)
DEF_FUNCPTR(BOOL,, _PhysGetFriction, (int objectID, float* friction), 0x0051B9C0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _PhysStopControlLocation, (int objectID), 0x00519310, NULL, NULL, NULL)
DEF_FUNCPTR(void, __stdcall, PhysRopeListener, (struct sPropertyListenMsg *,struct PropListenerData__ *), 0x0050F6B0, NULL, 0x0055C4E0, NULL)
DEF_FUNCPTR(int,, _PhysObjHasPhysics, (int objectID), 0x00519920, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _PhysGetClimbingObj, (int objectID), 0x0051AD60, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _PhysObjIsMantling, (int object), 0x004F2860, NULL, 0x00535C50, NULL)
DEF_FUNCPTR(int,, _PhysObjInWater, (int object), 0x0051A370, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _PhysNetDiscardBorrow, (int object), 0x0050E3C0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _PhysNetBroadcastObjPosByObj, (int object), 0x0050E8A0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _PhysNetForceContainedMsgs, (int enabled), 0x0050DE10, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _PhysDeregisterModel, (int object), 0x0051B650, NULL, NULL, NULL)
DEF_FUNCPTR(BOOL,, ThrowObjectInternal, (int thrower, int thrown, float force), 0x0055F9C0, NULL, NULL, NULL)

// pick
DEF_FUNCPTR(int,, _DoPickOperation, (int linkData, int linkSrc, int linkDest, ulong frameTime), 0x00570CF0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, PickLockDoStop, (int P1), 0x00571000, NULL, NULL, NULL)

// network
DEF_FUNCPTR(eNetworkCategory,, _NetworkCategory, (int), 0x0053EB60, NULL, 0x0059CE70, NULL)
DEF_FUNCPTR(void,, _SetNetworkCategory, (int objectID, int networkType), 0x0053EB90, NULL, 0x0059CEB0, NULL)

// weapon
DEF_FUNCPTR(int,, _EquipAIWeapon, (int objectID, DWORD P1, DWORD P2), 0x004BD690, NULL, 0x004F6390, NULL)
DEF_FUNCPTR(void,, _EquipWeapon, (int playerObj, int weaponObj, int type), 0x0056F7C0, NULL, 0x00609380, NULL)
DEF_FUNCPTR(void,, _EquipBow, (), 0x0056E310, NULL, 0x00607C30, NULL)
DEF_FUNCPTR(void,, _UnEquipWeapon, (int playerObj, int weaponObj), 0x0056F8A0, NULL, 0x006094A0, NULL)
DEF_FUNCPTR(void,, _SetWeapon, (int objectID, int arrowArch, int P1), 0x004BD320, NULL, 0x004F5F60, NULL)
DEF_FUNCPTR(void,, _UnSetWeapon, (int objectID), 0x004BD4A0, NULL, 0x004F6120, NULL)
DEF_FUNCPTR(int,, _GetWeaponObjID, (int objectID), 0x004BD510, NULL, 0x004F61B0, NULL)
DEF_FUNCPTR(void,, _StartWeaponAttack, (int playerObject, int weaponObject), 0x0056FD10, NULL, 0x00609910, NULL)
DEF_FUNCPTR(void,, _FinishWeaponAction, (int playerObject, int weaponObject), 0x0056FE30, NULL, 0x00609A30, NULL)
DEF_FUNCPTR(void,, _StartBowAttack, (), 0x0056E7F0, NULL, 0x00608110, NULL)
DEF_FUNCPTR(void,, _FinishBowAttack, (), 0x0056E840, NULL, 0x00608160, NULL)
DEF_FUNCPTR(int,, _has_projectile_type, (int object), 0x004B4D50, NULL, 0x004ECE90, NULL)
DEF_FUNCPTR(void,, _SetCurrentArrow, (int object), 0x0056EB80, NULL, 0x006084D0, NULL)
DEF_FUNCPTR(int,, _IsWeapon, (int object), 0x004BD260, NULL, 0x004F5EA0, NULL)
DEF_FUNCPTR(void,, _UpdateWeaponAttack, (), 0x0056FBC0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _WeaponEvent, (WeaponEvent event, int object, int, int), 0x004BBDF0, NULL, NULL, NULL)

// interface rendering
DEF_FUNCPTR(void,, _ScrnForceUpdate, (void), 0x00448560, NULL, 0x0045A780, NULL)
DEF_FUNCPTR(void,, _ScrnForceUpdateRect, (Rect*), 0x004485A0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ScrnClear, (void), 0x004483B0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ScrnLockDrawCanvas, (void), 0x00447F50, NULL, 0x0045A0E0, NULL)
DEF_FUNCPTR(void,, _ScrnModeGet, (sScreenMode* mode), 0x004487D0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ScrnModeSet, (int, sScreenMode* mode, int), 0x004488D0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ScrnNewBlackPalette, (), 0x00448710, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ScrnBlacken, (), 0x00448450, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ScrnUnlockDrawCanvas, (void), 0x00447F90, NULL, 0x0045A120, NULL)
DEF_FUNCPTR(void,, _region_set_invisible, (void*, int invisible), 0x0059CF60, NULL, 0x0063BE50, NULL)
DEF_FUNCPTR(void,, _region_expose, (struct _Region*, Rect*), 0x0059CB80, NULL, 0x0063BA70, NULL)
DEF_FUNCPTR(void,, _TextGadgUpdate, (void*), 0x005A0350, NULL, 0x0063F2F0, NULL)
DEF_FUNCPTR(void,, _TextGadgFocus, (void*), 0x005A0380, NULL, 0x0063F320, NULL)
DEF_FUNCPTR(void,, _TextGadgUnfocus, (void*), 0x005A03D0, NULL, 0x0063F370, NULL)
DEF_FUNCPTR(struct _LGadRoot*,, _LGadCurrentRoot, (), 0x0059EA80, NULL, 0x0063F710, NULL) // this seems to return a _LGadRoot*, see dromed.005FDF71 - was previously Region
DEF_FUNCPTR(void,, _LGadSetupSubRoot, (TextboxState* TextBoxDataBuffer, _LGadRoot* root, int P1, int P2,int width, int height), 0x0059EB00, NULL, 0x0063F790, NULL)
DEF_FUNCPTR(void,, _LGadCreateTextboxDesc, (void*, void*, LGadTextboxCreateParams*), 0x005A0460, NULL, 0x006429C0, NULL)
DEF_FUNCPTR(void,, _LGadDestroyRoot, (_LGadRoot*), 0x0059EA90, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _LGadDestroyTextBox, (void*), 0x005A0540, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _compose_cursor, (int), 0x00448510, NULL, NULL, NULL)

// camera
DEF_FUNCPTR(int,, _CameraIsRemote, (DWORD cam), 0x004073F0, NULL, 0x004090D0, NULL)
DEF_FUNCPTR(int,, _CameraGetMode, (DWORD cam), 0x004073D0, NULL, 0x00408ED0, NULL)
DEF_FUNCPTR(void,, _CameraRemote, (PlayerCamera*, int attachObj), 0x00406EC0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _CameraView, (PlayerCamera*, int attachObj), 0x00406E50, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _CameraAttach, (PlayerCamera*, int attachObj), 0x00406DF0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, AttachPlayerCamera, (), 0x00432940, NULL, NULL, NULL)

// cam
DEF_FUNCPTR(void,, CamApplyAlpha, (float alpha, int), 0x00438F60, NULL, NULL, NULL)

// config
DEF_FUNCPTR(bool,, _config_get_value, (const char* name, int, void* value, int* valuesRead), 0x00593130, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _config_get_raw, (char *,char *,size_t), 0x005933B0, NULL, 0x006319E0, NULL)
DEF_FUNCPTR(int,, _config_get_single_value, (char* key, int P1, int* buffer), 0x00593370, NULL, 0x006319A0, NULL)
DEF_FUNCPTR(void,, _config_set_int, (const char* name, int val), 0x005936F0, NULL, 0x00631D20, NULL)
DEF_FUNCPTR(void,, _config_load, (const char* file), 0x00593820, NULL, 0x00631E70, NULL)

// gd
DEF_FUNCPTR(void,, _gd_bitmap, (void*, int, int), 0x005803C0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _gd_clear, (int), 0x005805C0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _gd_rect, (int left, int top, int right, int bottom), 0x00580540, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _gd_box, (int left, int top, int right, int bottom), 0x005804A0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _gd_vline, (int x, int ystart, int yend), 0x00581340, NULL, NULL, NULL)

// cell
DEF_FUNCPTR(int,, _CellBinComputeFunc, (int objectID, DWORD, DWORD), 0x0040CA50, NULL, 0x0040FA60, NULL)

// headmove
DEF_FUNCPTR(void,, _headmoveCheck, (), 0x00419070, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _headmoveSetRelPosX, (int), 0x00419010, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _headmoveSetRelPosY, (int), 0x00419020, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _headmoveTouch, (), 0x00419060, NULL, NULL, NULL)

// rand
DEF_FUNCPTR(int,, _RandRange, (int min, int max), 0x004011D0, NULL, NULL, NULL)

// ghost
DEF_FUNCPTR(void,, _GhostNotify, (int, int), 0x00532050, NULL, 0x00588240, NULL)
DEF_FUNCPTR(void,, _GhostChangeRemoteConfig, (int obj, int, int), 0x00532090, NULL, NULL, NULL) 
DEF_FUNCPTR(void,, _GhostRemRemote, (int obj), 0x00531DD0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _GhostAddRemote, (int obj, float, int), 0x00531C60, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _GhostAddLocal, (int obj, float, int), 0x00531B80, NULL, NULL, NULL)
DEF_FUNCPTR(sGhostRemote*,, _GhostGetRemote, (int object), 0x005321D0, NULL, 0x005885A0, NULL)
DEF_FUNCPTR(sGhostLocal*,, _GhostGetLocal, (int object), 0x00532240, NULL, 0x00588610, NULL)
DEF_FUNCPTR(void,, _GhostSendMoCap, (int objectID, short schema, short motion, BOOL bIndexBased), 0x00534E80, NULL, 0x0058D200, NULL)
DEF_FUNCPTR(void,, _GhostSendHeartbeat, (sGhostLocal*, sGhostPos*), 0x00534EC0, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _GetGhostType, (int objectID), 0x00532390, NULL, 0x00588760, NULL)
DEF_FUNCPTR(void,, _GhostRecvPacket, (int objectID,int seqID,int relobj,sGhostHeartbeat* hbData,sGhostMoCap* mcData), 0x00533FA0, NULL, 0x0058B100, NULL)
DEF_FUNCPTR(int,, _IsLocalGhost, (int objectID), 0x005322B0, NULL, 0x00588680, NULL)
DEF_FUNCPTR(int,, _IsRemoteGhost, (int objectID), 0x005322B0, NULL, 0x005886F0, NULL)
DEF_FUNCPTR(char*,, _ChooseCapture, (sGhostRemote*, int), 0x00534960, NULL, 0x0058C670, NULL)
DEF_FUNCPTR(int,, _IsNewPacketNeeded, (sGhostLocal* ghost, sGhostPos* pos), 0x005353A0, NULL, 0x0058DB20, NULL)
DEF_FUNCPTR(void,, _GhostJumpOff, (int objectID), 0x00532D30, NULL, NULL, NULL)
DEF_FUNCPTR(HRESULT,, LoadRemoteGhosts, (TagFileRead* write, unsigned long P2), 0x005325E0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, __GhostBuildMotionCoord, (sGhostRemote*), 0x00533250, NULL, NULL, NULL)
DEF_FUNCPTR(void,, __GhostConfigureRemoteModels, (sGhostRemote*), 0x00533200, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _ReAimGhostPhysics, (sGhostRemote*, float frameTime), 0x005342D0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _AimGhostHead, (sGhostRemote*), 0x00534500, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _UpdateGhostPrediction, (sGhostRemote*, float frameTime), 0x00534170, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _GhostAllowedToBuildModels, (sGhostRemote*), 0x005332D0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _GhostPerFrameModeUpdate, (sGhostRemote*), 0x00533EA0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _MocapEval, (sGhostRemote*, float frameTime), 0x00534C80, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _MocapRunMe, (sGhostRemote* pGhost, char* P2), 0x005347C0, NULL, NULL, NULL)
DEF_FUNCPTR(DWORD,, __GhostGravRemote, (int objectID, mxs_vector& position, DWORD physFlags), 0x00532D50, NULL, NULL, NULL)
DEF_FUNCPTR(void,, __GhostApproxPhys, (int objectID, mxs_vector& position, sGhostPos* ghostPos, float frameTime, DWORD gravRemote), 0x00533B30, NULL, NULL, NULL)
DEF_FUNCPTR(int,, __is_zero_vec, (mxs_vector& vec), 0x00533D50, NULL, NULL, NULL)
DEF_FUNCPTR(void,, __GhostBleedVelocity, (int objectID, sGhostPos* ghostPos, DWORD physFlags, DWORD unk, float frameTime), 0x00533A20, NULL, NULL, NULL)
DEF_FUNCPTR(void,, _AnalyzeVelocity, (struct sGhostRemote *, enum  eGhostMotionSpeed *, int *), 0x00534820, NULL, NULL, NULL)
DEF_FUNCPTR(int,, _GhostAllowedToSleep, (sGhostRemote*), 0x00533D70, NULL, NULL, NULL)
DEF_FUNCPTR(void,, __GhostPhysSleep, (sGhostRemote*), 0x005331B0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, __GhostSetupInitialPhysRemote, (sGhostRemote*), 0x005335B0, NULL, NULL, NULL)

// ghost anim
DEF_FUNCPTR(IMotionPlan*,, _BuildTagBasedPlan, (sGhostRemote*, char*), 0x005345D0, NULL, NULL, NULL)
DEF_FUNCPTR(IMotionPlan*,, _BuildSchemaOffsetPlan, (sGhostRemote*, int, int), 0x005346D0, NULL, NULL, NULL)

// frob
DEF_FUNCPTR(void,, _FrobExecute, (sFrobActivate*), 0x004AFFA0, NULL, 0x004E7DF0, NULL)
DEF_FUNCPTR(void,, _FrobInstantExecute, (sFrobActivate*), 0x004B03C0, NULL, NULL, NULL)
DEF_FUNCPTR(eFrobHandler,, _FrobHandler, (int objectID), 0x004AF2D0, NULL, 0x004E7110, NULL)

// damage
DEF_FUNCPTR(long, __stdcall, _net_damage_filter, (int,int,struct sDamage *,void *), 0x005569F0, NULL, 0x005ED4C0, NULL)
DEF_FUNCPTR(int,, _GetRealCulprit, (int object), 0x0044C570, NULL, 0x0045EF90, NULL)

// packet handlers
DEF_FUNCPTR(void,, HandleFrobRequest, (DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD), 0x004AFA70, NULL, 0x004E78A0, NULL)
DEF_FUNCPTR(void,, HandleRegSphere, (int, int, DWORD, float, mxs_vector*, DWORD), 0x0050DFC0, NULL, 0x0055AD50, NULL)
DEF_FUNCPTR(void,, HandleDamage, (int victim, int culprit, int damage), 0x005569A0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, HandleCreateQuestData, (char* questName, int questData, int P2, DWORD thisObj), 0x004360C0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, HandleSetQuestData, (char* questName, int questData, DWORD thisObj), 0x004360E0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, HandleHaltSound, (int handle, int object), 0x004B5F20, NULL, NULL, NULL)
DEF_FUNCPTR(void,, HandleFrobPickup, (DWORD, DWORD), 0x0055FC00, NULL, NULL, NULL)
DEF_FUNCPTR(void,, HandlePlayerInfo, (int playerIndex, char* playerName, char* playerIP, int playerObject, INetManager* netman), 0x0053B760, NULL, NULL, NULL)
DEF_FUNCPTR(void,, HandleGhostFullHB, (int objectID, int seqID, int relobj, sGhostHeartbeat* heartBeat, int P1), 0x005328B0, NULL, NULL, NULL)
DEF_FUNCPTR(void,, HandleSimpleSoundVec, (int handle, mxs_vector* vec, int schema, int sampleNumber, sfx_parm* parms, int senderObj), 0x004B5D70, NULL, 0x004EDFA0, NULL)
DEF_FUNCPTR(void,, HandleSoundVec, (int handle, mxs_vector* vec, int schema, int sampleNumber, sfx_parm* parms, int senderObj), 0x004B5CA0, NULL, 0x004EDEB0, NULL)

// command handlers
DEF_FUNCPTR(void,, HandleUseWeapon, (int startOrFinish), 0x0055F170, NULL, NULL, NULL)
DEF_FUNCPTR(void,, HandleDropItem, (), 0x0055F900, NULL, NULL, NULL)

// r3
DEF_FUNCPTR(void,, _r3_start_object_angles, (mxs_vector* vec, short* ang, int), 0x005AADA0, NULL, NULL, NULL)

// kb
DEF_FUNCPTR(_kbs_event,, _kb_next, (), 0x005FCFA0, NULL, NULL, NULL)
DEF_FUNCPTR(short,, _kb_cook_real, (_kbs_event& event, short& cooked, BYTE& P3, int zero), 0x00597C80, NULL, NULL, NULL)

// collision
DEF_FUNCPTR(int,,  CollideEvent, (int,int,int,float,class cPhysClsn *), 0x004F1EE0, NULL, NULL, NULL)
DEF_FUNCPTR(int,,  CollideEventRaw, (int,int,int,float,class cPhysClsn *), 0x004F1D10, NULL, NULL, NULL)

// loot
DEF_FUNCPTR(int,, _count_all_loot, (int), 0x0055C860, NULL, NULL, NULL)

typedef unsigned long (__stdcall* ReactionHandler)(sReactionEvent* pEvent, const sReactionParam* pParam, void*);

// reaction handlers
DEF_FUNCPTR(unsigned long, __stdcall, SlayReactionHandler, (sReactionEvent* pEvent, const sReactionParam* pParam, void*), 0x0040C340, NULL, NULL, NULL)
DEF_FUNCPTR(unsigned long, __stdcall, KnockoutReactionHandler, (sReactionEvent* pEvent, const sReactionParam* pParam, void*), 0x0056A3A0, NULL, NULL, NULL)
DEF_FUNCPTR(unsigned long, __stdcall, AddMetaPropReactionHandler, (sReactionEvent* pEvent, const sReactionParam* pParam, void*), 0x004D5120, NULL, NULL, NULL)
DEF_FUNCPTR(unsigned long, __stdcall, FrobObjectReactionHandler, (sReactionEvent* pEvent, const sReactionParam* pParam, void*), 0x0056A330, NULL, NULL, NULL)



// Unnamed functions
DEF_FUNCPTR(void,, OldPlayerCreateHook, (int, int), 0x0055A940, NULL, 0x005F1C50, NULL)
DEF_FUNCPTR(int,, PlayerFactoryHook, (), 0x0055A9B0, NULL, 0x005F1CC0, NULL)
DEF_FUNCPTR(void,, FrobItem, (int), 0x0055EF10, NULL, 0x005F6C30, NULL)
DEF_FUNCPTR(void,, DarkReleaseBlood, (sDamageMsg*), 0x00557290, NULL, 0x005EDD80, NULL)
DEF_FUNCPTR(void,, darkSendFrobPickup, (int frobbedObj, int frobber), 0x00560600, NULL, NULL, NULL)
DEF_FUNCPTR(void,, SetTotalLootQuestData, (int P1), 0x0055C780, NULL, 0x005F4000, NULL)
DEF_FUNCPTR(ulong, __stdcall, DarkCombatDamageListener, (const sDamageMsg*, void*), 0x00556E60, NULL, 0x005ED950, NULL)
DEF_FUNCPTR(int,, darkInvTakeObjFromWorld, (int frobbedObj, int frobber), 0x005604E0, NULL, 0x005F8210, NULL)
DEF_FUNCPTR(void,, SendProjectilePacket, (int launcherObject, int projArchetype, DWORD flags, float intensity, mxs_vector* position, mxs_angvec* angle, mxs_vector* velocity), 0x0051E2F0, NULL, 0x0056F9A0, NULL) 
DEF_FUNCPTR(void,, HandleFireProjectile, (int projOwner, int projArchetype, DWORD flags, float intensity, mxs_vector* pos, mxs_angvec* ang, mxs_vector* vel), 0x0051DC10, NULL, 0x0056F2C0, NULL)
DEF_FUNCPTR(int,, PlaySoundObj, (const char* schemaName, int object, int P3) , 0x005708C0, NULL, 0x0060A4E0, NULL)
DEF_FUNCPTR(int,, CreateProjectile, (int projOwner, int projArchetype, DWORD flags, float intensity, mxs_vector* pos, mxs_angvec* ang, mxs_vector* vel), 0x0051DC80, NULL, 0x0056F330, NULL)
DEF_FUNCPTR(void,, CollisionResolver, (const tSpringInfo* springInfo, DWORD P2), 0x005016C0, NULL, 0x0054A430, NULL)
DEF_FUNCPTR(int,, PlaySchemaPlayer, (char const*), 0x0056E990, NULL, 0x006082C0, NULL)
DEF_FUNCPTR(void,, RemoveRelationByID, (int object, short relationID), 0x004155E0, NULL, 0x0041B220, NULL)
DEF_FUNCPTR(int,, FindGroundTextureLoc, (mxs_vector* creaturePos, mxs_vector* objectPos, mxs_vector* groundPos), 0x0044CB50, NULL, 0x0045F570, NULL)
DEF_FUNCPTR(int,, sub_560650, (int frobbedObj, int frobber), 0x00560650, NULL, NULL, NULL)
DEF_FUNCPTR(BOOL,, sub_56E0C0, (DWORD*), 0x0056E0C0, NULL, NULL, NULL)
DEF_FUNCPTR(BOOL,, unnamed_4AFF10, (void* pLinkData, int, int), 0x004AFF10, NULL, NULL, NULL)
DEF_FUNCPTR(void,, GameModeEnterUnpause, (), 0x0053EE20, NULL, NULL, NULL)

// Goal
DEF_FUNCPTR(int,, GetGoalState, (IQuestData* qd, int index), 0x00558750, NULL, NULL, NULL) // was unnamed
DEF_FUNCPTR(cAnsiStr,, GoalFiction, (int index), 0x0055AD90, NULL, NULL, NULL)
DEF_FUNCPTR(cAnsiStr,, GoalDescription, (int index), 0x0055AD30, NULL, NULL, NULL)
DEF_FUNCPTR(IDataSource*,, GoalStatus, (int index), 0x0055ADF0, NULL, NULL, NULL)

// cStoredProperty
DEF_FUNCPTR(BOOL, __stdcall,  cStoredProperty_Set, (IProperty* property, int object, void* data), 0x00456140, NULL, NULL, NULL)
DEF_FUNCPTR(BOOL, __stdcall,  cStoredProperty_GetInt, (IProperty* property, int object, int* data), 0x004A0580, NULL, NULL, NULL)
DEF_FUNCPTR(BOOL, __stdcall,  cStoredProperty_GetVoid, (IProperty* property, int object, void** data), 0x004A0580, NULL, NULL, NULL)
DEF_FUNCPTR(int, __stdcall, cStoredProperty_SendPropertyMsg, (sNetMsg_Generic* msg, int objectID, DWORD P1, unsigned long P2), 0x004E4DA0, NULL, 0x00526370, NULL)
DEF_FUNCPTR(void, __stdcall, cStoredProperty_ReceivePropertyMsg, (struct sNetMsg_Generic const * msg, unsigned long P1, DWORD P2, int P3), 0x004E4EF0, NULL, 0x00526660, NULL)
DEF_FUNCPTR(void, __fastcall, cStoredProperty_OnListenMsg, (DWORD thisObj, int, unsigned long, int, DWORD), 0x004E5D50, NULL, NULL, NULL)

namespace cTagSetFns
{
	DEF_FUNCPTR(int, __fastcall, FromString, (cTagSet*, int, const char*), 0x0040A4E0, NULL, 0x0040C870, NULL)
}

namespace cObjectNetworkingFns
{
	DEF_FUNCPTR(void, __fastcall, HandleRemap, (IObjectNetworking* p, int, BYTE, uint, sSingleRemap*), 0x00541A20, 0x0052A080, NULL, NULL)
	DEF_FUNCPTR(void, __fastcall, HandleEndCreate, (IObjectNetworking* p, int, int object), 0x0053FF10, NULL, NULL, NULL)
	DEF_FUNCPTR(int, __fastcall, HandleBeginCreate, (IObjectNetworking* p, int, int exemplar, int localObj, int senderObj), 0x0053FDB0, NULL, NULL, NULL)
	DEF_FUNCPTR(void, __fastcall, ClearProxyEntry, (IObjectNetworking* p, int, int P1), 0x00541280, NULL, NULL, NULL)
}

namespace cHashSetBaseFns
{
	DEF_FUNCPTR(tHashSetNode__*, __fastcall, ScanNext, (cHashSet*, int, tHashSetHandle*), 0x00579720, NULL, NULL, NULL)
	DEF_FUNCPTR(tHashSetHandle, __fastcall, FindIndex, (cHashSet*, int, tHashSetKey__*), 0x0040A110, NULL, NULL, NULL)
}

namespace cDarkPanelFns
{
	DEF_FUNCPTR(void, __fastcall, _ctor, (cDarkPanel*, int, const sDarkPanelDesc* pDesc, const char* resPath), 0x005302F0, NULL, NULL, NULL)
	DEF_FUNCPTR(void, __fastcall, _dtor, (cDarkPanel*, int), 0x005303E0, NULL, NULL, NULL)
	DEF_FUNCPTR(void, __fastcall, InitUI, (cDarkPanel*, int), 0x0052FD70, NULL, NULL, NULL)
	DEF_FUNCPTR(void, __fastcall, TermUI, (cDarkPanel*, int), 0x00530180, NULL, NULL, NULL)
}

namespace cInventory
{
	DEF_FUNCPTR(long, __stdcall, Select, (IInventory*, int object), 0x00569190, NULL, 0x00601150, NULL)
	DEF_FUNCPTR(long, __stdcall, ClearSelection, (IInventory*, eWhichInvObj), 0x00569290, NULL, 0x00601250, NULL)
}

namespace cObjectSystem
{
	DEF_FUNCPTR(int, __stdcall, IsObjSavePartition, (IObjectSystem*, int, unsigned long), 0x00428B70, NULL, 0x004345D0, NULL)
	DEF_FUNCPTR(long, __stdcall, Unlock, (IObjectSystem*), 0x00427710, NULL, 0x00432FF0, NULL)
}

namespace cObjectNetworking
{
	DEF_FUNCPTR(void, __stdcall, ClearTables, (IObjectNetworking* pThis), 0x00540A40, NULL, 0x0x0059F0F0, NULL)
	DEF_FUNCPTR(void, __stdcall, ObjRegisterProxy, (IObjectNetworking* thisObj, int ownerID, short shortObj, int object), 0x005410D0, NULL, 0x0059F7D0, NULL)
	DEF_FUNCPTR(int, __stdcall, ObjGetProxy, (IObjectNetworking* thisObj, int P1, short P2), 0x00541110, NULL, 0x0059F830, NULL)
	DEF_FUNCPTR(int, __stdcall, ObjIsProxy, (IObjectNetworking* thisObj, int object), 0x00541460, NULL, 0x0059FCC0, NULL)
	DEF_FUNCPTR(void, __stdcall, ObjDeleteProxy, (IObjectNetworking* thisObj, int P1), 0x00541250, NULL, 0x0059FA60, NULL)
	DEF_FUNCPTR(void, __stdcall, NotifyObjRemapped, (IObjectNetworking* pThis, int mapFrom, int mapTo), 0x00541680, 0x00529CB0, NULL, NULL)
	DEF_FUNCPTR(void, __stdcall, StartBeginCreate, (IObjectNetworking* pThis, int P1, int P2), 0x0053FE00, NULL, 0x0059E340, NULL)
	DEF_FUNCPTR(void, __stdcall, FinishBeginCreate, (IObjectNetworking* thisObj, int objectID), 0x0053FEC0, NULL, 0x0059E400, NULL)
	DEF_FUNCPTR(void, __stdcall, FinishEndCreate, (IObjectNetworking* pThis, int objectID), 0x0053FF80, NULL, 0x0059E4C0, NULL)
	DEF_FUNCPTR(void, __stdcall, ObjGiveTo, (IObjectNetworking* thisObj, int P1, int P2, int P3), 0x00540E00, NULL, 0x0059F4B0, NULL)
	DEF_FUNCPTR(void,  __stdcall, ObjTakeOver, (IObjectNetworking* thisObj, int objectID), 0x00540DC0, NULL, 0x0059F470, NULL)
	DEF_FUNCPTR(void, __stdcall, ObjGiveWithoutObjID, (IObjectNetworking* thisObj, int P1, int P2), 0x00540220, NULL, 0x0059E8D0, NULL)
	DEF_FUNCPTR(void, __stdcall, ObjHostInfo, (IObjectNetworking* thisObj, int P1, int* P2, short* P3), 0x005415D0, NULL, 0x0059FE30, NULL)
	DEF_FUNCPTR(void, __stdcall, StartDestroy, (IObjectNetworking* thisObj, int objectID), 0x00540080, NULL, 0x0059E610, NULL)
	DEF_FUNCPTR(void, __stdcall, StartEndCreate, (IObjectNetworking* thisObj, int objectID), 0x0053FF60, NULL, 0x0059E4A0, NULL)
}

namespace cContainSys
{
	/* Returns 0 on success, 1 on failure */
	DEF_FUNCPTR(long, __stdcall, Add, (IContainSys* pContainSys, int,int,int,unsigned int), 0x004A88F0, NULL, 0x004DF680, NULL)
}

namespace cGroundLocoManeuverFactory
{
	DEF_FUNCPTR(cMotionPlan*, __stdcall, CreatePlan, (const cMotionSchema* pSchema, struct sMcMotorState &pMotorState, struct sMcMoveState &pMoveState, sMcMoveParams& pParams, class IMotor* pMotor, class cMotionCoordinator *pMotCoord), 0x005208E0, NULL, NULL, NULL)
}

namespace cDarkGameSrv
{
	DEF_FUNCPTR(long, __stdcall, EndMission, (DWORD thisObj), 0x0056C850, NULL, 0x00605CC0, NULL)
	DEF_FUNCPTR(long, __stdcall, FadeToBlack, (DWORD thisObj, float fadeTime), 0x0056C860, NULL, 0x00605CF0, NULL)
}

namespace cDarkUISrv
{
	DEF_FUNCPTR(long, __stdcall, ReadBook, (DWORD pThis, const char*, const char*), 0x0056D710, NULL, NULL, NULL)
}

namespace cNetManager
{
	DEF_FUNCPTR(long, __stdcall, Init, (INetManager*), 0x00538480, 0x005209B0, 0x005957E0, NULL)
	DEF_FUNCPTR(const char*, __stdcall, GetPlayerAddress, (INetManager*, int), 0x00538B80, 0x005210E0, 0x00696D80, NULL)
	DEF_FUNCPTR(int, __stdcall, EnumPlayersCallback, (DPID, DWORD, LPCDPNAME, DWORD, LPVOID), 0x00539140, 0x005216B0, 0x00596CF0, NULL)
	DEF_FUNCPTR(int, __stdcall, InitExistingPlayer, (DPID, DWORD, LPCDPNAME, DWORD, LPVOID), 0x005EA780, 0x00610370, 0x006976F0, NULL)
	DEF_FUNCPTR(int, __stdcall, _DestroyNetPlayerCallback, (DPID, DWORD, LPCDPNAME, DWORD, LPVOID), 0x00536480, 0x0051E940, 0x005948A0, NULL)
	DEF_FUNCPTR(void, __stdcall, Leave, (INetManager*), 0x00539260, 0x005217C0, 0x00596E50, NULL)

	DEF_VAR(DPID*, gm_PlayerDPID, 0x0066CCB0, 0x00693740, 0x00743468, NULL)
	DEF_VAR(BOOL*, gm_bSynchronizing, 0x006C9448, 0x006F6C08, 0x0084F668, NULL)
	DEF_VAR(DWORD*, gm_Net, 0x006C9434, 0x006F6BF4, 0x0084F654, NULL)
	DEF_VAR(DWORD*, gm_TheNetManager, 0x006C9420, 0x006F6BE0, 0x0084F640, NULL)
	DEF_VAR(DWORD*, gm_DefaultHostPlayer, 0x006C9438, NULL, NULL, NULL)

	DEF_OFFSET(INetPlayerProperty, gm_NetPlayerProp, 0x006C9424, NULL, NULL, NULL)
}

namespace cNet
{
	DEF_FUNCPTR(int, __stdcall, Host, (INet* thisObj, char *,char *), 0x005EA100, 0x0060FCF0, 0x006969B0, NULL);
	DEF_FUNCPTR(long, __stdcall, SimpleCreatePlayer, (INet* thisObj, char*), 0x005EA540, 0x00610130, 0x00696F00, NULL);
	DEF_FUNCPTR(int, __stdcall, Join, (INet* thisObj, const char *,const char *,const char *), 0x005EA260, 0x0060FE50, 0x00696B80, NULL)
}

namespace cPropSndInstHighFns
{
	DEF_FUNCPTR(int, __fastcall, EnterCallback, (cPropSndInstHigh* pThis, int, class cRoom const *,class cRoomPortal const *,struct mxs_vector const &,float), 0x004D02E0, NULL, NULL, NULL)
}

namespace cAIConverse
{
	DEF_FUNCPTR(void,, SpeechEndCallback, (int P1, int P2, int sndSchema), 0x00487320, NULL, NULL, NULL)
}

namespace cAIBehaviorSet
{
	DEF_FUNCPTR(void, __stdcall, EnactProxyJointScanAction, (IAIBehaviorSet* pBehaviorSet, IAI* pAI, void* P2), 0x00463040, NULL, NULL, NULL)
}

namespace cDebrief
{
	DEF_FUNCPTR(void, __fastcall, OnButtonList, (cDarkPanel* pPanel, int, unsigned short P1, int selection), 0x00558B20, NULL, NULL, NULL)
}

namespace cPickLockSrv
{
	DEF_FUNCPTR(int, __stdcall, StartPicking, (DWORD thisObj, int, int, int), 0x00572AA0, NULL, NULL, NULL)
	DEF_FUNCPTR(int, __stdcall, FinishPicking, (DWORD thisObj, int), 0x00572AC0, NULL, NULL, NULL)
}

struct _lgMouseEvent;
namespace cDefaultInputDevicesSink
{
	DEF_FUNCPTR(long, __stdcall, OnMouse, (DWORD thisObj, _lgMouseEvent* pEvent), 0x00598160, NULL, NULL, NULL)
}

namespace cPhysModelFns
{
	DEF_FUNCPTR(const mxs_vector&, __fastcall, GetVelocity, (cPhysModel*, int, int P1), 0x004F2790, NULL, NULL, NULL)
	DEF_FUNCPTR(int, __fastcall, IsLocationControlled, (cPhysModel*, int), 0x00507060, NULL, NULL, NULL)
}

namespace cLoadingFns
{
	DEF_FUNCPTR(void, __fastcall, OnLoopMsg, (cLoading* pLoading, int, int, tLoopMessageData__*), 0x00559340, NULL, NULL, NULL)
	DEF_FUNCPTR(void, __fastcall, DoFileLoad, (cLoading* pLoading, int, ITagFile* pTag), 0x00559260, NULL, NULL, NULL)
}

namespace cAnsiStrFns
{
	DEF_FUNCPTR(void, __fastcall, AllocBuffer, (cAnsiStr*, int, int size), 0x00579BA0, NULL, NULL, NULL)
	DEF_FUNCPTR(void, __fastcall, Append, (cAnsiStr*, int, int length, const char*), 0x00579E60, NULL, NULL, NULL)
}

namespace cPlayerModeFns
{
	DEF_FUNCPTR(void, __fastcall, InstallOnDeathCallback, (cPlayerMode*, int, void*), 0x00433D10, NULL, 0x00441370, NULL)
}

namespace cNetMsgFns
{
	DEF_FUNCPTR(void, __fastcall, SendSpew, (cNetMsg* pNetMsg, int, char* msgData), 0x0053E320, 0x00526810, 0x0059C630, NULL)
	DEF_FUNCPTR(void, __fastcall, ReceiveSpew, (cNetMsg* pNetMsg, int, int), 0x0053E7A0, 0x00526C40, 0x0059CAB0, NULL)
	DEF_FUNCPTR(void, __fastcall, HandleMsg, (cNetMsg* pNetMsg, int, struct sNetMsg_Generic const *,unsigned long,int), 0x0053D870, 0x00525D90, 0x0059B8E0, NULL)
	DEF_FUNCPTR(int, __fastcall, MarshalArguments, (cNetMsg* pNetMsg, int, char * msgData, int* P1, unsigned int* P2), 0x0053D110, 0x005258A0, 0x0059AF40, NULL)
}

// cNetManager
DEF_FUNCPTR(int, __stdcall, cNetManager_LoopFunc, (void* pNetMan, eLoopMsgType msg, tLoopMessageData__* pMsgData), 0x00535E00, NULL, NULL, NULL)
DEF_FUNCPTR(void, __fastcall, cNetManager_SynchReady, (INetManager* netman, int), 0x00537DD0, NULL, NULL, NULL)

namespace cBowSrv
{
	DEF_FUNCPTR(long, __stdcall, StartAttack, (IBowSrv*), 0x005504E0, NULL, 0x005E6860, NULL)
	DEF_FUNCPTR(long, __stdcall, FinishAttack, (IBowSrv*), 0x00550500, NULL, 0x005E6880, NULL)
}

namespace cAIManager
{
	DEF_FUNCPTR(int, __stdcall, TransferAI, (IAIManager* thisObj, int P1, int P2), 0x0046CCE0, NULL, 0x00489090, NULL)
	DEF_FUNCPTR(int, __stdcall, CreateAI, (IAIManager* thisObj, int,char const *), 0x00468820, NULL, 0x00482F90, NULL)
	DEF_FUNCPTR(int, __stdcall, StartConversation, (IAIManager* thisObj, int iID), 0x00468E80, NULL, 0x004838A0, NULL)
	DEF_FUNCPTR(long, __stdcall, MakeFullAI, (IAIManager* thisObj, int P1, const sAINetTransfer* pTransferInfo), 0x0046C690, NULL, NULL, NULL)
}

namespace cAICombat
{
	DEF_FUNCPTR(int, __stdcall, IsValidTarget, (int object), 0x0047B100, NULL, 0x00498FE0, NULL)
}

namespace cGameTools
{
	DEF_FUNCPTR(long, __stdcall, TeleportObject, (DWORD, int,struct mxs_vector const *,struct mxs_angvec const *,int), 0x00415420, NULL, 0x0041B060, NULL)
}

namespace cSaveFilePanelFns
{
	DEF_FUNCPTR(void, __fastcall, DoFileOp, (DWORD* thisObj, int, int), 0x0056BA20, NULL, 0x00604E70, NULL)
}

namespace cAISensesFns
{
	DEF_FUNCPTR(long, __fastcall, GetAwarenessLink, (IAISenses* p, int, int object, sAIAwareness**), 0x004A3150, NULL, NULL, NULL)
	DEF_FUNCPTR(void, __fastcall, SetAwareness, (IAISenses* p, int, int object, long handle, const sAIAwareness*), 0x004A3D70, NULL, NULL, NULL)
}

//namespace cGroundActionManeuverFactory
//{
//	DEF_FUNCPTR(cMotionPlan*, __stdcall, CreatePlan, (const cMotionSchema* pSchema, struct sMcMotorState &pMotorState, struct sMcMoveState &pMoveState, sMcMoveParams& pParams, class IMotor* pMotor, class cMotionCoordinator *pMotCoord), 0x00520070, NULL, 0x00571AC0, NULL)
//}
//
//namespace cPlayerManeuverFactory
//{
//	DEF_FUNCPTR(cMotionPlan*, __stdcall, CreatePlan, (const cMotionSchema* pSchema, struct sMcMotorState &pMotorState, struct sMcMoveState &pMoveState, sMcMoveParams& pParams, class IMotor* pMotor, class cMotionCoordinator *pMotCoord), 0x005215D0, NULL, NULL, NULL)
//}

namespace cReactions
{
	DEF_FUNCPTR(long, __stdcall, React, (IReactions* pReactions, unsigned long,struct sReactionEvent *,struct sReactionParam const *), 0x004D65A0, NULL, 0x00516700, NULL)
}

namespace cScriptMan
{
	DEF_FUNCPTR(long, __stdcall, AddModule, (DWORD thisObj, const char*), 0x005C6E60, NULL, 0x00671730, NULL)
	DEF_FUNCPTR(long, __stdcall, SendMessageA, (DWORD thisObj, sScrMsg* pMsg, sMultiParm* pParms), 0x005C7BC0, NULL, NULL, NULL)
}

namespace cKeySys
{
	DEF_FUNCPTR(int, __stdcall, TryToUseKey, (DWORD thisObj, int keyObject, int lockedObject, eKeyUse use), 0x004B20A0, NULL, NULL, NULL)
}

namespace cBaseDamageModelFns
{
	DEF_FUNCPTR(unsigned long, __fastcall, SendMessage, (cBaseDamageModel*, int, const sDamageMsg*), 0x0040B710, NULL, NULL, NULL)
}

namespace cSimpleDamageModelFns
{
	DEF_FUNCPTR(unsigned long, __stdcall, DamageObject, (DWORD pThis, int, int, sDamage*, sChainedEvent*, int), 0x0044BC80, NULL, NULL, NULL)
}

namespace cNameMapFns
{
	DEF_FUNCPTR(int, __fastcall, IDFromName, (cNameMap*, int, const Label*), 0x0044F010, NULL, NULL, NULL)
	DEF_FUNCPTR(Label*, __fastcall, NameFromID, (cNameMap*, int, int ID), 0x0041FEF0, NULL, NULL, NULL)
}

namespace sSimMsgFns
{
	DEF_FUNCPTR(sSimMsg*, __fastcall, Constructor, (sSimMsg*, int, int objectTo, BOOL bEntering), 0x0044B360, NULL, NULL, NULL)
	DEF_FUNCPTR(void, __fastcall, Destructor, (sSimMsg*, int), 0x0044B2D0, NULL, NULL, NULL)
}

namespace sScrMsgFns
{
	DEF_FUNCPTR(sScrMsg*, __fastcall, Constructor, (sScrMsg*, int), 0x004357C0, NULL, NULL, NULL)
	DEF_FUNCPTR(void, __fastcall, Destructor, (sScrMsg*, int), 0x0044B2D0, NULL, NULL, NULL)
}

// Size = 0x78
class cPanelMode
{
};

class cModalButtonPanel
{
public:
	class cClient
	{
	public:
		DWORD x00, x04, x08, x0c;
		DWORD LGadBox; // 0x10
	};
};

class cPanelLoopClient
{
public:
	DWORD x00, x04;
	IPanelMode* m_PanelMode; // 0x8
};

struct sPanelModeDesc
{
	DWORD x00, x04;
};

class cDarkPanel /* : public cModalButtonPanel */
{
public:
	cDarkPanel(const sDarkPanelDesc* pDesc, const char* resPath) { cDarkPanelFns::_ctor(this, NULL, pDesc, resPath); }
	~cDarkPanel() { cDarkPanelFns::_dtor(this, NULL); }
	//void *operator new(size_t size) { return DarkNew(size); }

	virtual void InitUI() { return cDarkPanelFns::InitUI(this, NULL); }
	virtual void TermUI() { return cDarkPanelFns::TermUI(this, NULL); }
	virtual int NumButtons() { return m_PanelDesc.num_buttons; }
	virtual cDynArray<Rect> & Rects() { return m_Rects; }
	virtual cDynArray<_DrawElement> & Elems() { return m_Elements; }
	virtual IDarkDataSource * CursorImage(void) { return FetchUIImage("cursor", "intrface"); }
	virtual IDarkDataSource * CursorPalette(void);
	virtual void OnButtonList(unsigned short,int) = 0; // 0x1C
	virtual void OnLoopMsg(int, tLoopMessageData__ *); // 0x20
	virtual void RedrawDisplay() { return; } // 0x24
	virtual void OnEscapeKey() { m_panelMode->Exit(); } // 0x28

	IPanelMode* m_panelMode; // 0x4
	DWORD x08;
	cModalButtonPanel::cClient* m_pClient; // 0xc
	sDarkPanelDesc m_PanelDesc; // 0x10 - 0x30
	cDynArray<Rect> m_Rects; // 0x34 - 0x38
	cDynArray<_DrawElement> m_Elements; // 0x3C - 0x40
	DWORD x44, x48;
	IRes* m_pFont;
	DWORD x50, x54;
	cAnsiStr x58;
	cAnsiStr m_resPath; // 0x64 - 0x6C
	guiStyle m_Style;
protected:
};

// Size = 0x1BC
class cAutomap : public cDarkPanel
{
public:
	int m_currentPage; // 0xC0 - initialized to qvar MAP_MIN_PAGE
	DWORD xc4, xc8, xcc, xd0;
	IQuestData* m_pQuestData; // 0xD4
};

class cBook : public cDarkPanel
{
public:
	int m_currentPage; // 0xC0
	int m_minPage; // 0xC4
	int m_maxPage; // 0xC8
	cDynArray<Rect> m_TextRect; // 0xCC - 0xD0 - size of this array (at 0xD0) is considered page count for drawing purposes
	DWORD xd4, xd8, xdc, xe0, xe4;
	IRes* m_pBookImages[4]; // 0xE8
};

class cLoading : public cDarkPanel
{
public:
	BYTE unk[0xA4];
	ITagFile* m_pTagFile; // 0x164
	BYTE unk2[0x4C];
	IRes* m_pBitmap; // 0x1B4
	BYTE unk3[0x50];
	int m_loadState; // 0x208
	DWORD x20c;
	DWORD x210;
	int m_progressDialRate;	// 0x214
	DWORD x218;
};

class cLoadingSaveGame: public cLoading
{
};

// externals
extern cStandardRelationFns StandardRelation;
extern cGroundActionManeuverFactory GroundActionManeuverFactory;
extern cPlayerManeuverFactory PlayerManeuverFactory;
extern cLoadingSaveGameFns LoadingSaveGame;