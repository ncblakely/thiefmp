/*************************************************************
* File: Patcher.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Patches data in the game executable.
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "Patcher.h"
#include "Player.h"
#include "SaveManager.h"
#include "Sound.h"
#include "Callbacks.h"
#include "ErrorHandling.h"
#include "Ghost.h"
#include "Client.h"
#include "LoopClients.h"
#include "DarkPanels.h"
#include "DarkStat.h"
#include "NetReactions.h"
#include "DarkOverlay.h"
#include "Difficulty.h"

void __stdcall NewUpdateBowAttack();

void PatchVtable(int addr, ...)
{
	va_list marker;
	va_start(marker, addr);

	void* pfn = va_arg(marker, void*);

	SafeWrite((void*)addr, &pfn, 4);

	va_end(marker);
}

void T2PatchVtables()
{
	// cReactions
	PatchVtable(0x0061D1B4, &HookReactions::OnReact);

	// cBowSrv
	PatchVtable(0x00624320, &HookBowSrv::OnStartAttack);
	PatchVtable(0x00624324, &HookBowSrv::OnFinishAttack);

	// cSimpleDamageModel
	//PatchVtable(0x00608848, HookSimpleDamageModel2::OnDamageObject);
}

PtrPatchList g_MultiplayerPatches[] =
{
	// cObjectNetworking
	{0x00624218, &HookObjectNetworking::ClearTables},
	{0x0062421C, &HookObjectNetworking::ObjRegisterProxy},
	{0x00624220, &HookObjectNetworking::ObjGetProxy},
	{0x00624224, &HookObjectNetworking::ObjDeleteProxy},
	{0x00624228, &HookObjectNetworking::ObjIsProxy},
	{0x0062423C, &HookObjectNetworking::ObjHostInfo},
	{0x00624240, &HookObjectNetworking::ObjTakeOver},
	{0x00624244, &HookObjectNetworking::ObjGiveTo},
	{0x00624248, &HookObjectNetworking::ObjGiveWithoutObjID},
	{0x0062424C, &HookObjectNetworking::StartBeginCreate},
	{0x00624250, &HookObjectNetworking::FinishBeginCreate},
	{0x00624254, &HookObjectNetworking::StartEndCreate},
	{0x00624258, &HookObjectNetworking::FinishEndCreate},
	{0x0062425C, &HookObjectNetworking::StartDestroy},
	{0x00624268, &HookObjectNetworking::NotifyObjRemapped},

	// Throw creature hook
	{0x0055FAA7, &ThrowCreatureHook, PatchType_RedirectCall},

	// cDarkGameSrv
	{0x00625228, &OnReadBook},

	// Difficulty property hooks
	{0x0061367C, &Difficulty_SetAIAwarenessDelay},
	{0x00613684, &Difficulty_GetAIAwarenessDelay},
	//{0x0061634C, &Difficulty_GetAIRatingProperty},

	// Packets
	{0x00651678, &HookFrobRequest},
	{0x00671CF0, &HookFrobPickup},
	{0x0065BD78, &HookRegSphere},
	{0x00639AC8, &HookHandleCreateQuestData},
	{0x00639B20, &HookHandleSetQuestData},
	{0x006528B0, &HookHandleHaltSound},
	{0x0066CC08, &HookHandlePlayerInfo},
	{0x0065F180, &HookHandleFireProjectile},
	{0x0066D980, &HookHandleRequestDestroy},
	{0x0066CB70, &HookHandleFinishSynch},
	{0x00671CF0, &HookHandleFrobPickup},

	// cScriptMan
	{0x0062AA0C, &HookScriptMan::SendMessageA},
	{0x0062A9D8, &HookScriptMan::AddModule},

	// Ghost
	{0x00531A3D, &GhostFrameProcessRemote, PatchType_RedirectCall},

	PATCHARRAY_END
};

PtrPatchList g_StartupPatches[] =
{
#ifdef _DEBUG
	// cObjectSystem

	// cAIBehaviorSet
	{0x00626CC0, &HookAIBehaviorSet::EnactProxyJointScanAction}, // DarkCamera

	// cBaseRelation - DEBUG ONLY!
#endif

#ifndef _RELEASE
	{0x00605724, &HookObjectSystem::Unlock},

	//{0x0061DF9C, cBaseRelation_GetSingleLink},
	//{0x0061DFF0, cBaseRelation_GetSingleLink},
#endif

	// PumpEvents hook
	{0x00628298, &PumpEventsHook},

	//// Register struct desc hook
	//{0x0062AE68, &HookRegisterStructDesc},

	// cDarkGameSrv
	{0x00625188, &HookDarkGameSrv::EndMission},
	{0x0062518C, &HookDarkGameSrv::FadeToBlack},

	// cAIManager
	{0x0060C3F8, &HookAIManager::TransferAI},
	{0x0060C440, &HookAIManager::CreateAI},
	{0x0060C470, &HookAIManager::StartConversation},

	// cAINetManager
	{0x0060C418, &HookAINetManager::MakeProxyAI},

	// cContainSys
	{0x0061846C, &HookContainSys::Add},

	// cInventory
	{0x00624F30, &HookInventory::Select},
	{0x00624F38, &HookInventory::ClearSelection},

	// cSimpleDamageModel
	{0x00608848, &HookSimpleDamageModel::DamageObject},

	// cStoredProperty
	{0x0061E17C, &HookStoredProperty::SendPropertyMsg},
	{0x0061E180, &HookStoredProperty::ReceivePropertyMsg},

	// cNet
	{0x0062C1CC, &HookNet::Host},
	{0x0062C1D0, &HookNet::Join},
	{0x0062C1D4, &HookNet::SimpleCreatePlayer},

	// cNetManager
	{0x00538FB5, &HookNetManager::EnumPlayersCallback},
	{0x005EA764, &HookNetManager::InitExistingPlayer},
	{0x0053619B, &HookNetManager::_DestroyNetPlayerCallback},
	{0x00623F84, &HookNetManager::GetPlayerAddress},
	{0x00623FF4, &HookNetManager::Leave},
		// Don't apply these last two to DromEd
		{0x00623F98, &HookNetManager::SuspendMessaging},
		{0x00623FA0, &HookNetManager::ResumeMessaging},

	// cDarkNetServices
	{0x00624E9C, &HookDarkNetServices::MyAvatarArchetype},

	// cNetMsg
		// Send
	{0x0061F974, HookNetMsg::MarshalArguments},
	{0x00623D54, HookNetMsg::MarshalArguments},
	{0x006241C4, HookNetMsg::MarshalArguments},
		// Receive
	{0x0061F970, HookNetMsg::HandleMsg},
	{0x00623D50, HookNetMsg::HandleMsg},
	{0x006241C0, HookNetMsg::HandleMsg},

	// GhostReceive
	//{0x0066BF18, &HookGhostReceive::ReceiveAIHeartbeat},
	{0x0066BCE8, &HookGhostReceive::ReceiveFullHeartbeat},

	// cLockPickSrv
	{0x00626334, &HookPickLockSrv::StartPicking},
	{0x00626338, &HookPickLockSrv::FinishPicking},

	// Maneuver factories
	{0x00621980, &HookGroundActionManeuverFactory::CreatePlan},
	{0x00621A14, &HookPlayerManeuverFactory::CreatePlan},
	{0x006219C8, &HookGroundLocoManeuverFactory::CreatePlan},

	// cLoading
	{0x00624B00, &HookLoading::OnLoopMsg},
	{0x00624B0C, &HookLoading::DoFileLoad},

	// cLoadingSaveGame
	{0x00624BD4, &HookLoadingSaveGame::OnLoopMsg},

	// File panel hooks
	{0x00625038, &HookDarkFilePanel::SlotFileName},	// cLoadPanel
	{0x0062507C, &HookDarkFilePanel::SlotFileName},	// cSavePanel
	{0x00625084, &HookSaveFilePanel::DoFileOp},			// cSavePanel

	// Menu panel hooks
	{0x00624CE4, &MainMenuBlistHook},
	{0x00624CB8, &SimMenuBlistHook},
	{0x00624AA4, &DebriefBlistHook},

	// cDebugScrSrv
	{0x0061C27C, &HookDebugScrSrv::MPrint},

	/* Loop client hooks */
	{0x56C5EE, &SimulationLoopHook},
	{0x56AAA8, &RenderLoopHook},
	{0x421188, &ObjSysLoopClientHook},
	{0x0044B808, &SimLoopClientHook},
	{0x00413988, &GameModeLoopClientHook},

	/* Sound exception handlers */
	{0x004CE7B3, &LoopSampleCallbackHook},
	{0x0061CCF0, &PropSndEnterCBHook},
	{0x0048718A, &SpeechEndCallbackHook},
	{0x00487293, &SpeechEndCallbackHook},

	/* Player create / player factory hooks */
	{0x55A926, &PlayerCreateHook},

	/* DirectPlay wrapper */
	{0x600384, &FakeCoCreateInstance},

	/* Net simulation callback */
	{0x6C94A4, &NetSimCallback},

	/* Debug string logger */
	{0x600120, &DebugStringHook},

	/* Damage  hooks */
	{0x4BDE00, &DarkDamageListener},
	{0x00556C2F, &HookDarkCombatDamageListener},

	/* Quick save/load hooks */
	{0x673028, &SaveManager::HookQuickSave},
	{0x67303C, &SaveManager::HookQuickLoad},

	/* Ghost callbacks */
	{0x006C940C, &GhostAttachWeapon},
	{0x006C9418, &ShockPlayerActionToSchemaIdx},
	{0x006C9408, &GhostInstallFireListener},

//  I don't think any of these cause crashes anymore so only hook in release mode for extra safety
#ifdef _RELEASE
	/* Crash fixes */
	{0x0040CB21, &CellBinComputeFuncHook},
	{0x0063BBD4, &ObjectAnglesFuncHook},
	{0x0063BBF0, &ObjectAnglesFuncHook},

	/* Object rendering safety */
	{0x44222F, &ObjectIsVisible},
#endif

	/* Function pointers */
	{0x5FDB50, &CClient::OnPlayerCreate},
	{0x5FDB54, &LaunchProjectileHook},
	{0x5FDB58, &EquipAIWeaponHook},
	{0x5FDB68, &GhostHeartbeatFrequencyMax},
	{0x5FDB6C, &CClient::OnJump},
	{0x5FDB70, &OnGenerateSound},
	{0x5FDB74, &EquipBowHook},
	{0x5FDB78, &UnEquipBowHook},
	{0x5FDB7C, &OnIsValidTarget},
	{0x5FDB80, &OnGenerateSoundVec},
	{0x5FDB84, &OnGenerateSoundObj},
	{0x5FDB8C, &SaveManager::HookSaveRemoteGhosts},
	{0x5FDB90, &SaveManager::HookLoadRemoteGhosts},
	//{0x5FDB94, &GhostFrameProcessRemote},
	{0x5FDB98, &CollisionResolverHook},
	{0x5FDB9C, &IsNewPacketNeededHook},
	{0x5FDBA0, &CClient::StartGameModeCallback},
	{0x5FDBA4, &OnGhostJumpOff},
	{0x5FDBA8, &OnCollideEvent},
	{0x5FDBAC, &DarkStatIntSet},
	//{0x5FDBB0, &DarkStatIntAdd},
	{0x5FDBB4, &OnSpeechHalt},
	{0x5FDBB8, &MocapRunMe},
	{0x5FDBBC, &SchemaIDPlay},
	{0x5FDBC4, &MPPlayerCreate},
	{0x5FDBC8, &HookNetManager::SendCreatePlayerNetMsg},
	{0x5FDBD0, &NewUpdateBowAttack},
	{0x5FDBD4, &GhostFrameProcessLocal},

	{0x5FDD00, &mx_sub_vec_safe},
	{0x5FDD04, &mx_dist_vec_safe},

	PATCHARRAY_END
};

RawPatchList g_StartupRawPatches[] =
{
	///* TEST allow joins in progress */
#ifdef _DEBUG
	{0x0053669F, 0x909090909090, 6},
#endif

	/* Fix missing AI animations for clients */
	{0x5346DD, 0x9090909090, 5},
	{0x520E21, 0x0, 1},

	/* Increase maximum players to 8 */
	{0x5383A4, 0x08, 1},

	/* Ghost motion capture */
	{0x623D30, 0x3FE6666666666666, 8}, // set min walk anim threshold to 0.7
	{0x623D2E, 0x59,	 1}, // set min run anim threshold to 100.0

	/* Ghost update rate */
	{0x535B80, 0x90C3005FDB6825FF, 8}, // hook ghost update rate

	/* Shorten send/receive spew for console output */
	{0x53E7E9, 0x90000002F7E9, 	6},	// receive
	{0x53E369, 0x90000003B6E9, 6},	// send

	///* Ghost capture hook */
	//{0x534DA3, 0x9090D88B005F, 6},
	//{0x534D9F, 0xDB0C15FF, 4},

	/* Safe math functions */
	{0x577DC0, 0x9090005FDD0025FF, 8},		// mx_sub_vec_safe
	{0x577FB0, 0x90005FDD0425FF, 7},		// mx_dist_vec_safe

	/* Is valid target hook */
	{0x47B2B6, 0x5FDB7C15, 4},

	/* Sound generation hooks */
	{0x445C79, 0x90005FDB7015FF55, 8}, // GenerateSound

	/* Prevent hitting the jump key while dead from ending the mission in MP games */
	{0x552B60, 0x9090005FDB6C25FF, 8},

	/* Log mprintf debug strings */
	{0x576619, 0xEB,	 1},

	/* Fix duplicate player creation in MP */
	{0x55DA34, 0x9090005FDB5015FF, 8},
	{0x55DA3C, 0x9090, 2},

	/* Fix maximum hit points in MP */
	{0x537FC5, 0x9090909090, 5},
	{0x537FDB, 0x9090909090, 5},

	/* Fix inventory cycling in MP */
	{0x5691BD, 0xEB, 1},

	/* Fix ground action maneuver animations for clients */
	{0x5202CD, 0xEB, 1},

	/* Disable net ambient flag */
	{0x4CCFA1, 0xEB, 1},

	/* Call cQuestData::doSet instead of Set for TOTAL_LOOT quest data */
	{0x55C836, 0x50, 1},

	/* Ignore no_network config line */
	{0x538627, 0x32, 1},
	{0x53846C, 0x32, 1},

	/* Remove all dependencies on DirectPlay 4 */
	{0x5EA108, 0x00000096E9, 5}, // Skip useless DirectPlay4 lobby initialization code in cNet::Host


	PATCHARRAY_END
};

RawPatchList g_MultiplayerRawPatches[] =
{
	// Fix repeating flee sounds
	{0x488C50, 0x9090, 2},

	/* Framerate independence */
	{0x0044B863, 0x909009EBC18B, 6},

	PATCHARRAY_END
};

void InstallCallbacks()
{
	InstallCallback(0x0056E8F5, 0x5FDB54, CBTYPE_Call, 0, 4); // LaunchProjectileHook
	InstallCallback(0x0051F3D9, 0x5FDB58, CBTYPE_Call, 0, 2); // EquipAIWeaponHook
	InstallCallback(0x0056E340, 0x5FDB74, CBTYPE_Call, 0, 2); // EquipBowHook
	InstallCallback(0x0056E3D6, 0x5FDB78, CBTYPE_Call, 0, 2); // UnequipBowHook
	InstallCallback(0x00445C69, 0x5FDB80, CBTYPE_Call, 0, 2); // GenerateSoundVec
	InstallCallback(0x00445C40, 0x5FDB84, CBTYPE_Call, 0, 2); // GenerateSoundObj
	InstallCallback(0x00532790, 0x5FDB8C, CBTYPE_Jump, 0, 2); // HookSaveRemoteGhosts
	InstallCallback(0x005325D2, 0x5FDB90, CBTYPE_Call, 0, 2); // HookLoadRemoteGhosts
	//InstallCallback(0x00534DF0, 0x5FDB94, CBTYPE_Jump, 0, 0); // GhostFrameProcessRemote
	InstallCallback(0x00500A82, 0x5FDB98, CBTYPE_Call, 0, 2); // CollisionResolverHook
	InstallCallback(0x0053536D, 0x5FDB9C, CBTYPE_Call, 0, 2); // IsNewPacketNeededHook
	InstallCallback(0x0056C6FC, 0x5FDBA0, CBTYPE_Call, 0, 2); // StartGameModeCallback
	InstallCallback(0x00533F49, 0x5FDBA4, CBTYPE_Call, 0, 2); // OnGhostJumpOff
	InstallCallback(0x004FD325, 0x5FDBA8, CBTYPE_Call, 0, 2); // CollideEvent
	InstallCallback(0x004FD5E1, 0x5FDBA8, CBTYPE_Call, 0, 2); // CollideEvent
	InstallCallback(0x0056D350, 0x5FDBAC, CBTYPE_Jump, 0, 2); // DarkStatIntSet
	//InstallCallback(0x0056D230, 0x5FDBB0, CBTYPE_Jump, 0, 2); // DarkStatIntAdd
	InstallCallback(0x0044FF80, 0x5FDBB4, CBTYPE_Jump, 0, 2); // OnSpeechHalt
	InstallCallback(0x005347C0, 0x5FDBB8, CBTYPE_Jump, 0, 2); // MocapRunMe
	InstallCallback(0x00445D20, 0x5FDBBC, CBTYPE_Jump, 0, 2); // SchemaIDPlay
	InstallCallback(0x00432810, 0x5FDBC4, CBTYPE_Jump, 0, 2); // MPPlayerCreate
	//InstallCallback(0x0053A309, 0x5FDBC8, CBTYPE_Call, 0, 1); // SendCreatePlayerMsg fix this, can't pick up items on lotp when enabled as client?
	////InstallCallback(0x0056C656, 0x5FDBD0, CBTYPE_Call, 0, 4); // NewUpdateBowAttack

#ifdef NEW_NETCODE
	InstallCallback(0x005352D0, 0x5FDBD4, CBTYPE_Jump, 0, 0); // GhostFrameProcessLocal
#endif
}

void PatcherInit()
{
	MemPatcher::WriteRaw(g_StartupRawPatches, NULL, NULL);
	MemPatcher::WritePtr(g_StartupPatches);
	InstallCallbacks();
	T2PatchVtables();
}

void PatcherTerm()
{
}