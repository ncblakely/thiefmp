#pragma once

// goal state
#define GOAL_STATE_INCOMPLETE 0
#define GOAL_STATE_COMPLETE 1
#define GOAL_STATE_INACTIVE 2
#define GOAL_STATE_FAILED 3

// shock overlay rects
#define SHOCKRECT_MAP 0x1A

// awareness filter effect flags
#define AWAREFILT_ABORTCANTSEE 1
#define AWAREFILT_ABORTCANTHEAR 2
#define AWAREFILT_ABORTSTUNNED 4
#define AWAREFILT_ABORTNOTAI 8

// ghost packet flags
#define HBEAT_ROTATION	192
#define HBEAT_AI			128

// sim state flags
#define SIMSTATE_TIMEPASSING 8

// ghost types
// AND ghost type with 3 and compare with flags
#define GHOST_PLAYER 3
#define GHOST_AI 1

// sGhostRemote + 8 flags
#define GHOST_PHYS_HASMOTIONCOORD 0x4
#define GHOST_PHYS_SLEEPING 0x80000

// sfx_parm flags
#define SND_NETAMBIENT 0x8000

// stored property networking flags
#define PROP_NONETWORK 0x1

// frob info flags
#define FINFO_MOVE 0x1
#define FINFO_SCRIPT 0x2
#define FINFO_DELETE 0x4
#define FINFO_IGNORE 0x8
#define FINFO_FOCUSSCRIPT 0x10
#define FINFO_TOOLCURSOR 0x20
#define FINFO_USEAMMO 0x40
#define FINFO_DEFAULT 0x80
#define FINFO_DESELECT 0x100

// macros
#define OBJ_IS_ABSTRACT(obj) (obj < 0)
#define OBJ_IS_CONCRETE(obj) (obj > 0)

// ghost actions
#define GHOST_ACTION_DYING 1
#define GHOST_ACTION_WOUNDED 2
#define GHOST_ACTION_SWINGING 4
#define GHOST_ACTION_FIRING 8
#define GHOST_ACTION_SLEEP 16
#define GHOST_ACTION_REVIVE 32
#define GHOST_ACTION_DEAD 64
#define GHOST_ACTION_SLEEPING 128

// motion flags
#define MFLAG_FOOTFALL 0x1E
#define MFLAG_INTERRUPT 0x40
#define MFLAG_BODYCOLLAPSE 0x8000
#define MFLAG_HITCORPSE 0x10000
#define MFLAG_WEAPONCHARGE 0x20000
#define MFLAG_SEARCH 0x40000
#define MFLAG_WEAPONSWING 0x80000

// mission flags
#define MISSFLAG_SKIP 1
#define MISSFLAG_NOBRIEFING 2
#define MISSFLAG_NOLOADOUT 4
#define MISSFLAG_CUTSCENE 8
#define MISSFLAG_END 16

#define MAX_MISSION_MAPS 32

// assorted game constants
#define kPM_NumModes 8	// total number of player and camera modes
#define kMaxActiveSounds 32
#define kNumWaterBanks 4
#define kNumDamageResults 8
#define kNumSpecialObjParams 2
#define kObjectConcrete 1
#define kMaxPlayerName 33
#define kMaxNetMsgParams 8 // maximum number of params for a net message
#define kMaxFrameLen 4 // physics frames
#define kAICA_Num 6 // max conversation actors
#define MAX_BOOK_PAGES 8

typedef int ObjID;
#define OBJ_NULL 0

// existing ghost motions (sGhostRemote + 56h)
const char kGM_LeanRight = 9;
const char kGM_LeanLeft = 8;
const char kGM_Jump = 6;
const char kGM_Climbing = 3;
const char kGM_Crouch = 1;

DECLARE_HANDLE(tScrTimer);
DECLARE_HANDLE(tScrIter);
DECLARE_HANDLE(PropListenerHandle);
DECLARE_HANDLE(PropListenerData);

struct tLoopMessageData__
{
	DWORD flags;
};


struct Label
{
	char name[16];
};

struct mxs_vector
{
	float x; // 0x0
	float y; // 0x4
	float z; // 0x8
};

struct mxs_angvec
{
	short heading, pitch, bank;
};

struct mxs_trans
{
	float data[12];
};

// Size = 0x16, this is wrong according to the struct desc
// + 0C = short
// + 0E = short

struct Position
{
	mxs_vector vec;
	short heading;
	short pitch;
	short bank;
	short cell;
	short hint;
	//mxs_angvec angle;
};

struct Location
{
	mxs_vector vec;
	mxs_angvec ang;
};

enum ePlayerMotion
{
	kMoInvalid = -1,

	kMoNormal,
	kMoStrideLeft,
	kMoStrideRight,

	kMoCrouch,
	kMoCrawlLeft,
	kMoCrawlRight,

	kMoWithBody,
	kMoWithBodyLeft,
	kMoWithBodyRight,

	kMoJumpLand,
	kMoWeaponSwing,
	kMoWeaponSwingCrouch,

	kMoLeanLeft,
	kMoLeanRight,
	kMoLeanForward,

	kMoCrouchLeanLeft,
	kMoCrouchLeanRight,
	kMoCrouchLeanForward,

	kMoDisable = 1000,
	kMoEnable,
	kBigNum = 0x10000,
};

enum eNetworkCategory
{
	CAT_LocalOnly = 0,
	CAT_Hosted = 1,
	CAT_Obsolete = 2,
};

enum eCycleDirection
{
	DIR_Back = -1,
	DIR_Forward = 1,
};

enum ePlayerMode
{
	MODE_Normal  = 0,
	MODE_Crouching = 1,
	MODE_Swimming = 2,
	MODE_Climbing = 3,
	MODE_BodyCarry = 4,
	MODE_Jumping = 6,
	MODE_Dead = 7,
};

enum ePickResult
{
	PICK_Failed = 0,
	PICK_Success = 1,
	PICK_Failed2 = 4,
};

enum eNetMsgArgType
{
	PARAM_Integer = 3,
	PARAM_MyObject = 0x7,
	PARAM_ProxyObject = 0x8,
	PARAM_Object = 0x9,
	PARAM_Object2 = 0xA,
	PARAM_GlobalObject = 0xB,
	PARAM_Vector = 13,
	PARAM_Block = 0xE,
};

enum eArgType
{
	ARG_NONE = 0,
	ARG_BOOL = 1,
	ARG_INT = 2,
	ARG_FLOAT = 3,
	ARG_DOUBLE = 4,
	ARG_STRING = 5,
	ARG_BOOLVAR = 6,
	ARG_INTVAR = 7,
	ARG_STRINGVAR = 8,
	ARG_INTARRAY = 9,
	ARG_FLOATVAR = 10,
	ARG_BOOLTOGGLE = 11,
	ARG_INTTOGGLE = 12,
	ARG_HELP = 13,
};

enum eDamageResult
{
	DR_Collision = 1,
	DR_Damage = 2,
	DR_SlayObject = 4,		// The damaged object is about to be killed
	DR_TerminateObject = 5,
	DR_ResurrectObject,
	DR_DestroyObject = 7,
};

enum _LGadTextBoxEvent
{
};

enum eWhichInvObj
{
	IO_Weapon = 0,
	IO_Item = 1,		// seems to double as junk in this case
};

enum CreatureType
{
	CT_Humanoid,
	CT_PlayerArm,
	CT_PlayerBowArm,
	CT_Burrick,
	CT_Spider,
	CT_BugBeast,
	CT_CrayMan,
	CT_Constantine,
	CT_Apparition,
	CT_Sweel,
	CT_Rope,
	CT_Zombie,
	CT_SmallSpider,
	CT_Frog,
	CT_Cutty,
	CT_Avatar,
	CT_Robot,
	CT_SmallRobot,
	CT_SpiderBot
};

enum eKeyUse
{
	KeyUse_ChangeLockState = 0,
	KeyUse_TestKeyWorks = 3,
};

enum eAIAwareLevel
{
	AWARE_None = 0,
	AWARE_Low = 1,
	AWARE_Moderate = 2,
	AWARE_High = 3,
};

enum eMainMenuButtons
{
	MainMenu_NewGame = 0,
	MainMenu_LoadGame = 1,
	MainMenu_Options = 2,
	MainMenu_Credits = 3,
	MainMenu_Intro = 4,
	MainMenu_Quit = 5,
};

enum eAIPriority
{
	AIPRIORITY_None = 0,
	AIPRIORITY_VeryLow = 1,
	AIPRIORITY_Low,
	AIPRIORITY_Normal,
	AIPRIORITY_High,
	AIPRIORITY_VeryHigh,
	AIPRIORITY_Absolute,
};

enum eSlayResult
{
	SLAY_Normal = 0,
	SLAY_NoEffect = 1,
	SLAY_Terminate = 2,
	SLAY_Destroy = 3,
};

enum eInventoryType
{
	INVTYPE_Junk = 0,
	INVTYPE_Item = 1,
	INVTYPE_Weapon = 2,
};

enum ContainRelType
{
	RELTYPE_Alternate = 0,
	RELTYPE_Hand = 1,
	RELTYPE_Belt = 2,
	RELTYPE_GenericContents = 3,
};

enum eAnimLightProp
{
	ANMLIGHTPROP_FlipBetweenMinMax = 0,
	ANMLIGHTPROP_SlideSmoothly = 1,
	ANMLIGHTPROP_Random = 2,
	ANMLIGHTPROP_MinimumBrightness = 3,
	ANMLIGHTPROP_MaximumBrightness = 4,
	ANMLIGHTPROP_ZeroBrightness = 5,
	ANMLIGHTPROP_SmoothlyBrighten = 6,
	ANMLIGHTPROP_SmoothlyDim = 7,
	ANMLIGHTPROP_RandomButCoherent = 8,
	ANMLIGHTPROP_FlickerMinMax = 9,
};

enum tMotorControllerType
{
	MOTCONTROLLER_GroundLoco = 0,
	MOTCONTROLLER_Combat = 1,
	MOTCONTROLLER_GroundAction = 2,
	MOTCONTROLLER_PlayerArm = 3,
	MOTCONTROLLER_PlayerBow = 4,
	MOTCONTROLLER_Single = 5,
};

enum eMSwordActionType
{
	MSWORDACTION_Swing = 0,
	MSWORDACTION_GeneralBlock = 1,
	MSWORDACTION_DirectedBlock = 2,
};

enum eFrobHandler
{
	FROB_Host = 0,
	FROB_LocalOnly = 1,
	FROB_AllMachines = 2,
};

enum eAxis
{
	AXIS_X = 0,
	AXIS_Y = 1,
	AXIS_Z = 2,
};

enum eDoorPropStatus
{
	DOORPROPSTATUS_Closed = 0,
	DOORPROPSTATUS_Open = 1,
	DOORPROPSTATUS_Closing = 2,
	DOORPROPSTATUS_Opening = 3,
	DOORPROPSTATUS_Halted = 4,
};

enum eTweqHalt
{
	TWEQHALT_DestroyObj = 0,
	TWEQHALT_RemoveProp = 1,
	TWEQHALT_StopTweq = 2,
	TWEQHALT_Continue = 3,
	TWEQHALT_SlayObj = 4,
};

enum eTweqControlType
{
	TWEQTYPE_TweqScale = 0,
	TWEQTYPE_TweqRotate = 1,
	TWEQTYPE_TweqJoints = 2,
	TWEQTYPE_TweqModels = 3,
	TWEQTYPE_TweqDelete = 4,
	TWEQTYPE_TweqEmitter = 5,
	TWEQTYPE_TweqFlicker = 6,
	TWEQTYPE_TweqLock = 7,
	TWEQTYPE_TweqAll = 8,
	TWEQTYPE_TweqNull = 9,
};

enum eTweqControlAction
{
	TWEQACTION_DoDefault = 0,
	TWEQACTION_DoActivate = 1,
	TWEQACTION_DoHalt = 2,
	TWEQACTION_DoReset = 3,
	TWEQACTION_DoContinue = 4,
	TWEQACTION_DoForward = 5,
	TWEQACTION_DoReverse = 6,
};

enum eJointType
{
	JOINT_None = 0,
	JOINT_Head = 1,
	JOINT_Neck = 2,
	JOINT_Abdomen = 3,
	JOINT_Butt = 4,
	JOINT_LeftShoulder = 5,
	JOINT_RightShoulder = 6,
	JOINT_LeftElbow = 7,
	JOINT_RightElbow = 8,
	JOINT_LeftWrist = 9,
	JOINT_RightWrist = 10,
	JOINT_LeftFingers = 11,
	JOINT_RightFingers = 12,
	JOINT_LeftHip = 13,
	JOINT_RightHip = 14,
	JOINT_LeftKnee = 15,
	JOINT_RightKnee = 16,
	JOINT_LeftAnkle = 17,
	JOINT_RightAnkle = 18,
	JOINT_LeftToe = 19,
	JOINT_RightToe = 20,
	JOINT_Tail = 21,
};

enum eInvRenderType
{
	INVRENDER_Default = 0,
	INVRENDER_AlternateModel = 1,
	INVRENDER_AlternateBitmap = 2,
};

enum eQVarOperation
{
	QVAROP_SetTo = 0,
	QVAROP_Add = 1,
	QVAROP_MultiplyBy = 2,
	QVAROP_DivideBy = 3,
	QVAROP_BitwiseOr = 4,
	QVAROP_BitwiseAnd = 5,
	QVAROP_BitwiseXOR = 6,
};

enum eCollisionResult
{
	COLLRESULT_Bounce = 0,
	COLLRESULT_Stick = 1,
	COLLRESULT_Slay = 2,
};

enum eCreatureHandleType
{
	CRETHANDLE_Humanoid = 0,
	CRETHANDLE_PlayerArm = 1,
	CRETHANDLE_PlayerBowArm = 2,
	CRETHANDLE_Burrick = 3,
	CRETHANDLE_Spider = 4,
	CRETHANDLE_BugBeast = 5,
	CRETHANDLE_Crayman = 6,
	CRETHANDLE_Constantine = 7,
	CRETHANDLE_Apparition = 8,
	CRETHANDLE_Sweel = 9,
	CRETHANDLE_Rope = 10,
	CRETHANDLE_Zombie = 11,
	CRETHANDLE_SmallSpider = 12,
	CRETHANDLE_Frog = 13,
	CRETHANDLE_Cutty = 14,
	CRETHANDLE_Avatar = 15,
	CRETHANDLE_Robot = 16,
	CRETHANDLE_SmallRobot = 17,
	CRETHANDLE_SpiderBot = 18,
};

enum eCreaturePoseType
{
	CRETPOSE_Tags = 0,
	CRETPOSE_MotionName = 1,
};

enum eAIAlertLevel
{
	ALERTLEVEL_None = 0,
	ALERTLEVEL_Low = 1,
	ALERTLEVEL_Moderate = 2,
	ALERTLEVEL_High = 3,
};

enum eAIMode
{
	AIMODE_Asleep = 0,
	AIMODE_SuperEfficient = 1,
	AIMODE_Efficient = 2,
	AIMODE_Normal = 3,
	AIMODE_Combat = 4,
	AIMODE_Dead = 5,
};

enum eAISpeed
{
	AISPEED_Stopped = 0,
	AISPEED_VerySlow = 1,
	AISPEED_Slow = 2,
	AISPEED_Normal = 3,
	AISPEED_Fast = 4,
	AISPEED_VeryFast = 5,
};

enum eAIGoalType
{
	AIGOAL_IdleNearObjectOrLocation = 0,
	AIGOAL_GotoObjectOrLocation = 1,
	AIGOAL_FollowObject = 2,
	AIGOAL_InvestigateObjectOrLocation = 3,
	AIGOAL_Conversation = 4,
	AIGOAL_CustomGoal = 5,
	AIGOAL_DefendObjectOrLocation = 6,
	AIGOAL_AttackObject = 7,
	AIGOAL_FleeFromObjectOrLocation = 8,
};

enum eAITeam
{
	AITEAM_Good = 0,
	AITEAM_Neutral = 1,
	AITEAM_Bad1 = 2,
	AITEAM_Bad2 = 3,
	AITEAM_Bad3 = 4,
	AITEAM_Bad4 = 5,
	AITEAM_Bad5 = 6,
};

enum eAIRating
{
	AIRATING_Null = 0,
	AIRATING_WellBelowAverage = 1,
	AIRATING_BelowAverage = 2,
	AIRATING_Average = 3,
	AIRATING_AboveAverage = 4,
	AIRATING_WellAboveAverage = 5,
};

enum eAIInvestKind
{
	INVESTKIND_Normal = 0,
	INVESTKIND_DontInvestigate = 1,
};

enum eAINonHostility
{
	AINONHOSTILITY_Never = 0,
	AINONHOSTILITY_AlwaysToPlayer = 1,
	AINONHOSTILITY_ToPlayerUntilDamaged = 2,
	AINONHOSTILITY_ToPlayerUntilThreatened = 3,
	AINONHOSTILITY_UntilDamaged = 4,
	AINONHOSTILITY_UntilThreatened = 5,
	AINONHOSTILITY_Always = 6,
};

enum eProjectileConstraint
{
	PROJCONSTRAIT_None = 0,
	PROJCONSTRAIT_Others = 1,
	PROJCONSTRAIT_Misses = 2, // not implemented
};

enum eProjectileTargetMethod
{
	PROJMETHOD_StraightLine = 0,
	PROJMETHOD_Arcing = 1,
	PROJMETHOD_Reflecting = 2,
	PROJMETHOD_Overhead = 3,
};

// These are used for several things - real names unknown
enum eAILevel1
{
	AILEVEL1_VeryLow,
	AILEVEL1_Low = 1,
	AILEVEL1_Moderate = 2,
	AILEVEL1_High = 3,
	AILEVEL1_VeryHigh = 4,
};

enum eAILevel2
{
	AILEVEL2_None = 0,
	AILEVEL2_Minimum = 1,
	AILEVEL2_VeryLow = 2,
	AILEVEL2_Low = 3,
	AILEVEL2_Normal = 4,
	AILEVEL2_High = 5,
	AILEVEL2_VeryHigh = 6,
};

//

enum eAIFireWhileMoving
{
	AIFIREMOVING_Never = 0,
	AIFIREMOVING_VeryRarely = 1,
	AIFIREMOVING_Rarely = 2,
	AIFIREMOVING_Sometimes = 3,
	AIFIREMOVING_Often = 4,
	AIFIREMOVING_VeryOften = 5,
};

enum eAISoundType
{
	AISOUND_None = 0,
	AISOUND_Inform = 1,
	AISOUND_MinorAnomaly = 2,
	AISOUND_MajorAnomaly = 3,
	AISOUND_NonCombatHigh = 4,
	AISOUND_CombatHigh = 5,
};

enum eAIFleeCondition
{
	FLEECONDITION_Never = 0,
	FLEECONDITION_OnLowAlert = 1,
	FLEECONDITION_OnModerateAlert = 2,
	FLEECONDITION_OnHighAlert = 3,
	FLEECONDITION_OnLowHitpoints = 4,
	FLEECONDITION_OnThreat = 5,
	FLEECONDITION_OnFrustration = 6,
};

enum eAIWatchKind
{
	WATCHKIND_PlayerIntrusion = 0,
	WATCHKIND_SelfEntry = 1,
};

enum eAIWatchpointLineRequirement
{
	WATCHPOINTLINEREQ_None = 0,
	WATCHPOINTLINEREQ_LineOfSight = 1,
	WATCHPOINTLINEREQ_Raycast = 2,
};

enum eAIWatchpointLinkKill
{
	WATCHPOINTLINKKILL_DontKill = 0,
	WATCHPOINTLINKKILL_AfterTrigger = 1,
	WATCHPOINTLINKKILL_AfterCompletion = 2,
};

enum eAIDefendPointReturnSpeed
{
	DEFENDRETURNSPEED_Stopped = 0,
	DEFENDRETURNSPEED_VerySlow = 1,
	DEFENDRETURNSPEED_Slow = 2,
	DEFENDRETURNSPEED_Normal = 3,
	DEFENDRETURNSPEED_Fast = 4,
	DEFENDRETURNSPEED_VeryFast = 5,
};

enum eAIBroadcast
{
	AIBROADCAST_None = 0,
	AIBROADCAST_Sleeping = 1,
	AIBROADCAST_AlertZero = 2,
	AIBROADCAST_AlertOne = 3,
	AIBROADCAST_AlertTwo = 4,
	AIBROADCAST_AlertThree = 5,
	AIBROADCAST_AlertToOne = 6,
	AIBROADCAST_AlertToTwo = 7,
	AIBROADCAST_AlertToThree = 8,
	AIBROADCAST_SpotPlayer = 9,
	AIBROADCAST_AlertDownFromOne = 10,
	AIBROADCAST_LostContact = 11,
	AIBROADCAST_Charge = 12,
	AIBROADCAST_Shoot = 13,
	AIBROADCAST_Flee = 14,
	AIBROADCAST_Friend = 15,
	AIBROADCAST_Alarm = 16,
	AIBROADCAST_Attack = 17,
	AIBROADCAST_AttackSuccess = 18,
	AIBROADCAST_BlockSuccess = 19,
	AIBROADCAST_DetectBlock = 20,
	AIBROADCAST_Blocked = 21,
	AIBROADCAST_HitNoDamage = 22,
	AIBROADCAST_HitHighDamage = 23,
	AIBROADCAST_HitLowDamage = 24,
	AIBROADCAST_HitAmbush = 25,
	AIBROADCAST_DieLoud = 26,
	AIBROADCAST_DieSoft = 27,
	AIBROADCAST_FoundBody = 28,
	AIBROADCAST_SomethingMising = 29,
	AIBROADCAST_SecurityBreach = 30,
	AIBROADCAST_SmallAnomaly = 31,
	AIBROADCAST_LargeAnomaly = 32,
	AIBROADCAST_FoundRobot = 33,
	AIBROADCAST_SawIntruder = 34,
	AIBROADCAST_SawBody = 35,
	AIBROADCAST_SawMissing = 36,
	AIBROADCAST_SawSomethingUnspecific = 37,
	AIBROADCAST_SawRecentDeadRobot = 38,
	AIBROADCAST_OutOfReach = 39,
	AIBROADCAST_Null = 40, // missing string in struct desc?
};

enum eAIBroadcastType
{
	AIBROADCASTTYPE_Normal = 0,
	AIBROADCASTTYPE_NoBroadcast = 1,
	AIBROADCASTTYPE_CustomConcept = 2,
};

enum eAIResponse
{
	AIRESPONSE_Nothing = 0, // ()
	AIRESPONSE_ScriptMessage = 1, // (Message,MsgData1,MsgData2)
	AIRESPONSE_PlaySoundOrMotion = 2, // (Sound Concept,Sound Tags,Motion Tags)
	AIRESPONSE_Alert = 3, // unimplemented
	AIRESPONSE_BecomeHostile = 4, // ()
	AIRESPONSE_EnableInvestigate = 5, // ()
	AIRESPONSE_GotoObject = 6, // (Object,Speed,Motion Tags)
	AIRESPONSE_FrobObject = 7, // (Target Object,With Object)
	AIRESPONSE_Wait = 8, // (Time(ms),Motion Tags)
	AIRESPONSE_Mprint = 9, // (text)
	AIRESPONSE_AddOrRemoveMetaProperty = 10, // (Add/Remove,Mprop name,Target Objects)
	AIRESPONSE_AddLink = 11, // (Flavor,Destination(s),Source(s))
	AIRESPONSE_RemoveLink = 12, // (Flavor,Destination(s),Source(s))
	AIRESPONSE_Face = 13, // (Angle,Object(-1 for angle relative to self))
	AIRESPONSE_Signal = 14, // (Signal,Target(s),Max Distance)
	AIRESPONSE_DestScriptMessage = 15, // (Message,MsgData1,MsgData2)
};

enum ePhysTypeProp
{
	PHYSTYPE_OBB = 0,
	PHYSTYPE_Sphere = 1,
	PHYSTYPE_SphereHat = 2,
	PHYSTYPE_None = 3,
};

enum tRendType
{
	RENDTYPE_Normal = 0,
	RENDTYPE_NotRendered = 1,
	RENDTYPE_Unlit = 2,
	RENDTYPE_EditorOnly = 3,
};

enum eParticleRenderType
{
	PARTICLERENDER_SingleColoredPixels = 0,
	PARTICLERENDER_MultiColoredPixels = 1,
	PARTICLERENDER_SingleColoredSquares = 2,
	PARTICLERENDER_MultiColoredSquares = 3,
	PARTICLERENDER_SingleColoredDisk = 4,
	PARTICLERENDER_ScaledBitmap = 5,
};

enum eParticleAnimation
{
	PARTICLEANIM_LaunchedOneShot = 0,
	PARTICLEANIM_LaunchedContinually = 1,
	PARTICLEANIM_RotatingHollowSphere = 2,
	PARTICLEANIM_RotatingSphere = 3,
	PARTICLEANIM_RotatingHollowCube = 4,
	PARTICLEANIM_RotatingCube = 5,
	PARTICLEANIM_RotatingDisk = 6,
	PARTICLEANIM_RotatingCircle = 7,
	PARTICLEANIM_RotatingPyramid = 8,
};

enum eParticleGroupMotion
{
	PARTICLEGROUPMOTION_Immobile = 0,
	PARTICLEGROUPMOTION_MoveWithVelocity = 1,
	PARTICLEGROUPMOTION_MoveWithVelAndGravity = 2,
	PARTICLEGROUPMOTION_TrailingObject = 3,
	PARTICLEGROUPMOTION_AttachedToObject = 4,
	PARTICLEGROUPMOTION_FireAndForget = 5,
};

enum eParticleLaunchType
{
	PARTICLELAUNCHTYPE_BoundingBox = 0,
	PARTICLELAUNCHTYPE_Sphere = 1,
	PARTICLELAUNCHTYPE_Cylinder = 2,
};

enum eParticleAttachLinkType
{
	PARTICLEATTACHLINK_Object = 0,
	PARTICLEATTACHLINK_Vhot = 1,
	PARTICLEATTACHLINK_Joint = 2,
	PARTICLEATTACHLINK_Submodel = 3,
};

enum eSkyMode
{
	SKYMODE_Textures = 0,
	SKYMODE_Stars = 1,
};

enum eSkyColorMethod
{
	SKYCOLORMETHOD_Sum = 0,
	SKYCOLORMETHOD_Interpolate = 1,
};

enum eFaceState
{
	FACESTATE_Neutral = 0,
	FACESTATE_Smile = 1,
	FACESTATE_Wince = 2,
	FACESTATE_Surprise = 3,
	FACESTATE_Stunned = 4,
};

enum eBookButtonRects
{
	BOOK_NextPage = 1,
	BOOK_PrevPage = 0,
};

enum eImageResType
{
	kImgRes_Bmp = 0,
	kImgRes_Cel = 1,
	kImgRes_Gif = 2,
	kImgRes_Pcx = 3,
	kImgRes_Tga = 4,
};

enum eLoopMsgType
{
	kMsgNull = 0x1,
	kMsgStart = 0x2,
	kMsgEnd = 0x4,
	kMsgExit = 0x8,
	kMsgStartBlock = 0x10,
	kMsgBlocked = 0x20,
	kMsgEndBlock = 0x40,
	kMsgBeginFrame = 0x80,
	kMsgNormalFrame = 0x100,
	kMsgPauseFrame = 0x200,
	kMsgEndFrame = 0x400,
	kMsgFrameReserved1 = 0x800,
	kMsgFrameReserved2 = 0x1000,
	kMsgEnterMode = 0x2000,
	kMsgSuspendMode = 0x4000,
	kMsgResumeMode = 0x8000,
	kMsgExitMode = 0x10000,
	kMsgMinorModeChange = 0x20000,
	kMsgModeReserved2 = 0x40000,
	kMsgModeReserved3 = 0x80000,
	kMsgLoad = 0x100000,
	kMsgSave = 0x200000,
	kMsgUserReserved1 = 0x400000,
	kMsgUserReserved2 = 0x800000,
	kMsgUserReserved3 = 0x1000000,
	kMsgApp1 = 0x2000000,
	kMsgApp2 = 0x4000000,
	kMsgApp3 = 0x8000000,
	kMsgApp4 = 0x10000000,
	kMsgApp5 = 0x20000000,
	kMsgApp6 = 0x40000000,
	kMsgApp7 = 0x80000000,
	kMsgApp8 = 0x0,
};

enum WeaponEvent
{
	WE_BowForceFinished = 4,
};