; External functions
cTagDBInputTag_ctor dd 0040D800h
cNetMsg__cNetMsg dd 0053CE40h
cDarkPanel__cDarkPanel dd 005302F0h
operator_new dd 005EC92Dh

CClientMsgBundle__dtor dd 00536C50h

CClientMsgBundleList__GetPlayer dd 00536C30h
cDList__Remove dd 0053C5A0h
cDABaseSrvFns__DoResize dd 005794C0h
cDABase__Append dd 0040DDD0h	; cDABase<cTagDBInputTag,4,cDARawSrvFns<cTagDBInputTag>>::Append(cTagDBInputTag const &)
cTagSet__FromString dd 0040A4E0h
_AppGetAggregated dd 00576930h
GhostGetLocal dd 00532240h
_GhostRemRemote dd 00531DD0h
GetSimTime dd 0044B770h
CClientMsgBundleList__KillPlayer dd 00538240h
_mx_scale_addeq_vec dd 00577EC0h
_mx_add_vec dd 00577D60h
_mx_scale_vec dd 00577E20h
_mx_sub_vec dd 00577DC0h
_mx_copy_vec dd 00577D40h
_mx_zero_vec dd 00577D10h
_PhysRaycast dd 0051C360h
_PhysSetModLocation dd 005186D0h
_PhysSetSubModLocation dd 005188A0h
_PhysStopControlLocation dd 00519310h
_PhysStopControlRotation dd 00519390h
_PhysStopControlRotationalVelocity dd 005191E0h
_PhysStopControlVelocity dd 00519000h
cPhysModel__CheckAngleLimits dd 00509300h
_SetPlayerMode dd 004330E0h
_ObjGetHitPoints dd 00419430h
_ObjSetHitPoints dd 00419480h
_ObjGetMaxHitPoints dd 004194D0h
_ObjSetMaxHitPoints dd 00419520h
_ObjSetHasRefs dd 00442950h
_ObjHasRefs dd 00442920h
_ObjEditName dd 00429270h
_EditGetObjNamed dd 004293B0h
_ObjPosGet dd 00422460h
_PhysObjInWater dd 0051A370h
_PhysGetDensity dd 0051AC10h
_PhysGetMass dd 0051ABA0h
_PhysNetDiscardBorrow dd 0050E3C0h
_PhysNetSetSleep dd 0050E2C0h
_PhysDeregisterModel dd 0051B650h
_LGadSetupSubRoot dd 0059EB00h
_LGadCreateTextBoxDesc dd 005A0460h
_LGadCurrentRoot dd 0059EA80h
__ftol dd 005EC3ACh
__is_zero_vec dd 00533D50h
_mprintf dd 005765E0h
_config_get_raw dd 005933B0h
CAnsiStr_ctor dd 00579AC0h
CAnsiStr__ChNil dd 006F5E2Ch
CAnsiStr__FreeStr dd 0057A2F0h
cHashSetBase__FindIndex dd 0040A110h
_GhostNotify dd 00532050h
_IsAPlayer dd 00432700h
_GetSimTime dd 0044B770h
_sprintf dd 005EC81Ch
_mprint dd 00576470h

; External variables
cNetManager__gm_ObjNet equ 006C942Ch
cNetManager__gm_ObjSys equ 006C9428h
cNetManager__gm_bSynchronizing equ 006C9448h
cNetManager__gm_bNetworkGame equ 006C9444h
cNetManager__gm_bNetworkingReady equ 006C9440h

_kGravityDir dd 006C3D70h
IID_IObjectSystem dd 0061E5C8h
IID_ITraitManager dd 0061F518h
IID_INetManager dd 00601130h
_IID_IContainSys equ 00618420h
_IID_ITraitManager dd 0061F518h
_IID_IAIManager equ 00600B60h
_IID_IQuestData equ 00606710h
_IID_INetManager equ 00601130h
_IID_IObjectNetworking equ 0061E5D8h
_IID_IObjectSystem equ 0061E5C8h
_IID_IStimulator equ 0061D2C0h
_IID_IStimSensors equ 0061D220h
_IID_IStimuli equ 0061D270h
g_pPlayerMode dd 006900ACh
_gPlayerObj equ 00690098h
g_pInputBinder equ 00687E04h
_g_PhysModels equ 006C3C50h
_CmdTerm equ 007BEB40h
ConsoleHandle equ 007BEBB9h
CmdTermText equ 007BEC71h


pFrobPickupObjMsg equ 006F55F0h

; Strings
;aHitSGivingUpOn db 'Hit %s: giving up on %g %g %g gonna do %g %g %g',0Ah,0
;aHeyRaycasterGa db 'Hey! raycaster gave us zero vec',0Ah,0
;aHideinterfac_2  db 'HideInterface',0
NetWait db "Waiting for players...", 0
NetPause db "Game paused.", 0
NetJoined db "All players joined.", 0
NetResume db "Game resumed.", 0
aCancel_0       db 'cancel',0
aNext           db 'next',0
aPrev           db 'prev',0
aIntrface_6     db 'intrface',0
aNetplay        db 'main',0								; should be netplay!
aNoname         db 'NoName',0 
aSeqD           db ' seq %d',0
aFull           db 'Full',0
aDtz            db 'Dtz',0

; Variables
g_pShockMultiUI dd 0

; Data structures
MultiStagingButtons db	  1		; DATA XREF: cShockMultiplayUI::cShockMultiplayUI(void)+20o
					; _SwitchToShockMultiplayUIMode+Do
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
off_6A4BC8		dd offset aPrev		; "prev"
		dd offset aCancel_0	; "cancel"
		dd offset aNext		; "next"
		db    0
		db    0
		db    0
		db    0
		
cShockMultiplayUI__gDesc dd offset aNetplay; "netplay"
		db    3
		db    0
		db    0
		db    0
		db    5
		db    0
		db    0
		db    0
		db    3
		db    0
		db    0
		db    0
dd offset off_6A4BC8
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    1
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0
		db    0

GamePaused dd 0

__real@4@4008fa00000000000000 dd 1.0e3
__real@8@3fff8000000000000000 dq 1.0
__real@8@4001d555555555555800 dq 6.666666666666667