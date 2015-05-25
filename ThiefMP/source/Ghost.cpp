/*************************************************************
* File: Ghost.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "Sound.h"
#include "SaveManager.h"
#include "Gamesys.h"
#include "Network.h"

/* ghost types:
	5 - local player
	6 - remote player
	9 - local AI
	10 - remote AI
*/

bool g_bDisableAIPrediction = false;

void GhostSetTagList(int objectID, const char* tags)
{
	actorTagListProp->Set(objectID, tags);
}

//======================================================================================
// GhostSetLocalAnimation
//
// Sets base animation for a local ghost without modifying the child animation.
//======================================================================================
void GhostSetLocalAnimation(int objectID, short anim)
{
	if (anim)
	{
		sGhostLocal* ghost = _GhostGetLocal(objectID);
		if (ghost)
			ghost->playing.schema_idx = anim;
	}
}

//======================================================================================
// GhostHeartbeatFrequencyMax
//
// Gets update rates for player and AI ghosts. Default is higher than the maximum that Thief 2 normally uses
// (reduces lag).
//======================================================================================
int GhostHeartbeatFrequencyMax(sGhostLocal* ghost, sGhostPos* pos)
{
	return Cfg.GetInt("HeartbeatFrequencyMax");
}

//==============================================================================
// IsNewPacketNeededHook()
//
// Called before IsNewPacketNeeded()
//==============================================================================
int __stdcall IsNewPacketNeededHook(sGhostLocal* ghost, sGhostPos* pos)
{
	int needed = _IsNewPacketNeeded(ghost, pos);
	if (needed)
	{
		// Fix the "sliding" bug for proxy AI by disabling their velocity prediction
		// This happens because g_pAINetSrv->GetTargetVel (called in IsNewPacketNeeded()) returns the velocity the AI
		// "wants" to move at, but not what they are actually moving at.
		if (_ObjIsAI(ghost->obj))
			_mx_zero_vec(pos->pos.vel);

		if (ghost->obj == *_gPlayerObj)
		{
			if (_PhysObjIsMantling(*_gPlayerObj))
				pos->pos.mode = kGM_Climbing;	// broadcast the climbing motion to other players
		//	//if (_PhysObjInWater(*_gPlayerObj))
		//	//	pos->motion = kGM_Swimming; // added to disable gravity calculations for swimming ghosts

		//	//_PhysGetVelocity(*_gPlayerObj, &pos->velocity);
		}

		//if (Debug.FlagSet(DEBUG_SENDS))
		//	__GhostPrintGhostPos(ghost->objectID, pos, ghost->ghostType, "0", -1);
	}

	return needed;
}

//======================================================================================
// GhostStateToString
//======================================================================================
const char* GhostStateToString(int ghostState)
{
	switch (ghostState)
	{
	case 0: return "Normal";
	case GHOST_ACTION_DYING: return "Dying";
	case GHOST_ACTION_WOUNDED: return "Wounded";
	case GHOST_ACTION_SWINGING: return "Swinging";
	case GHOST_ACTION_FIRING: return "Firing";
	case GHOST_ACTION_SLEEP: return "Sleep";
	case GHOST_ACTION_REVIVE: return "Revive";
	case GHOST_ACTION_DEAD: return "Dead";
	case GHOST_ACTION_SLEEPING: return "Sleeping";
	default: return "Unknown";
	}
}

//==============================================================================
// GhostAttachWeapon()
//
// Attaches a weapon to a ghost. Mesh transform values are set according to the weapon type,
// and the appropriate selection sound is played.
//==============================================================================
void GhostAttachWeapon(int ghostObj, char weapType)
{
	sGhostRemote* ghost = _GhostGetRemote(ghostObj);

	if (ghost)
	{
		// if we're equipping the same type of weapon, return
		if (ghost->weap.nWeapon == weapType)
			return;

		// if we already have a weapon, destroy the current one and play the appropriate unequip sound
		if (ghost->weap.weaponObj)
		{
			DoUnequipSound(ghost);

			// probably not necessary anymore
			_ObjSetRenderType(ghost->weap.weaponObj, 1);
			_ObjSetHasRefs(ghost->weap.weaponObj, 0);

			g_pObjSys->Destroy(ghost->weap.weaponObj);

			ghost->weap.weaponObj = 0;
		}

		ghost->weap.nWeapon = weapType;
		if (weapType < 0)
			return;

		int weaponObject;
		int avatarWeaponID;
		WeaponAttachment wa;
		mxs_trans* pTrans = (mxs_trans*)0x66C550; // wrench transform

		// set up the weapon attachment struct and copy the wrench transform data from SS2
		ZeroMemory(&wa, sizeof(WeaponAttachment));
		_mx_copy_trans(&wa.trans, pTrans);

		// adjust the transform data for specific weapons to position them properly
		switch (weapType)
		{
		case CNetMsg_SetWeapon::WeaponType_Sword:
			wa.attachJoint = JOINT_RightWrist;
			avatarWeaponID = Gamesys.Arch.AvatarSword;
			break;
		case CNetMsg_SetWeapon::WeaponType_Blackjack:
			wa.trans.data[5] = 0.0f;
			wa.trans.data[6] = 0.0f;
			wa.trans.data[9] = 0.25f;
			wa.trans.data[10] = -0.38f;
			wa.trans.data[11] = 0.1f;
			wa.attachJoint = JOINT_RightWrist;
			avatarWeaponID = Gamesys.Arch.AvatarBlackjack;
			break;
		case CNetMsg_SetWeapon::WeaponType_Bow:
			ZeroMemory(&wa.trans, sizeof(mxs_trans));
			wa.trans.data[10] = 0.4f;
			wa.trans.data[11] = -0.1f;
			wa.attachJoint = JOINT_LeftWrist;
			avatarWeaponID = Gamesys.Arch.AvatarBow;
			break;
		}

		weaponObject = g_pObjSys->BeginCreate(avatarWeaponID, 1);
		if (weaponObject)
		{
			g_pObjSys->EndCreate(weaponObject);
			_CreatureAttachItem(ghostObj, weaponObject, &wa);

			ghost->weap.weaponObj = weaponObject;
			DoEquipSound(ghost);
		}
		else if (Debug.IsFlagSet(DEBUG_OBJECTS))
			ConPrintF("Failed to attach weapon to %s", _ObjEditName(ghostObj));
	}
}

//==============================================================================
// OnGhostJumpOff()
//
// Called when a ghost has landed from a jump
//==============================================================================
void __stdcall OnGhostJumpOff(int objectID)
{
	cTagSet tag;
	mxs_vector groundpos;
	mxs_vector pos = _ObjPosGet(objectID)->vec;

	ZeroMemory(&tag, sizeof(cTagSet));
	cTagSetFns::FromString(&tag, NULL, "Event Footstep, Landing True");

	int groundtex = FindGroundTextureLoc(&pos, &pos, &groundpos);
	if (groundtex != -1)
	{
		_ESndPlayLoc(&tag, objectID, _GetTextureObj(groundtex), &pos, NULL, NULL);
	}

	cDABaseSrvFns::DoResize((void**)&tag, sizeof(cTagSet), NULL);

	_GhostJumpOff(objectID);
}

void GhostFireListener(int objectID, cCreature* creature, DWORD P3, DWORD P4)
{
	ConPrintF("Fire listener called for %s.", _ObjEditName(objectID));
}

void GhostInstallFireListener(int objectID)
{
	//ConPrintF("Installing fire listen for %s", _ObjEditName(objectID));
	//_AddMotionFlagListener(objectID, MFLAG_WEAPONCHARGE, &GhostFireListener);
}

//char* __stdcall GhostChooseCaptureHook(sGhostRemote* ghost, int P1)
//{
//	if (_IsAPlayer(ghost->objectID))
//	{
//		if (ghost->motion == kGM_Mantling)
//		{
//			return "Mantling";
//		}
//		if (ghost->motion == kGM_Climbing)
//			return "climbing";
//	}
//
//	return _ChooseCapture(ghost, P1);
//}

void _ghost_pos_centerconvert(int objectID, mxs_vector* pos)
{
	float offset;
	gAvatarHeightOffsetProp->Get(objectID, &offset);

	pos->z += offset;
}

int GhostAllowedToBuildModels(sGhostRemote* pGhost)
{
	int creatureType;
	if (!_ObjGetCreatureType(pGhost->obj, creatureType))
		return TRUE;
	else
		return (CreatureGetMotorInterface(pGhost->obj) != 0);
}

void UpdateGhostPrediction(sGhostRemote* pGhost, float frameTime)
{
	int object = pGhost->obj;
	int motion = pGhost->info.pred.pos.mode;
	DWORD gravRemote;

	if (motion == kGM_Climbing)
		gravRemote = 0;
	else
		gravRemote = __GhostGravRemote(object, pGhost->info.last.pos.pos, pGhost->cfg.flags);

	// update ghost heading from second pos?
	__GhostApproxPhys(object, pGhost->info.last.pos.pos, &pGhost->info.pred, frameTime, gravRemote);

	if (!__is_zero_vec(pGhost->info.pred.pos.vel))
	{
		__GhostBleedVelocity(object, &pGhost->info.pred, pGhost->cfg.flags, pGhost->info.last.time, frameTime);
	}
}

bool GhostPerFrameModeUpdate(sGhostRemote* pGhost)
{
	if (pGhost->info.last.pos.mode == 0xD && _GhostAllowedToSleep(pGhost))
	{
		pGhost->cfg.flags |= GHOST_PHYS_SLEEPING;

		if (Debug.IsFlagSet(DEBUG_GHOSTS))
			ConPrintF("%s: going to sleep.", _ObjEditName(pGhost->obj));

		__GhostPhysSleep(pGhost);

		return true;
	}

	return false;
}

void MocapRunMe(sGhostRemote* pGhost, char* tag)
{
	IMotionPlan* pPlan;

	if (tag[0])
	{
		pPlan = _BuildTagBasedPlan(pGhost, tag);
		//const char* tagList;
		//if (!actorTagListProp->Get(pGhost->objectID, &tagList))
		//	tagList = "Player";

		//pGhost->anim.schema = DarkPlayerActionToSchemaIdx(tag, tagList);
		//pGhost->anim.motion = -1;
	}
	else
	{
		pPlan = _BuildSchemaOffsetPlan(pGhost, pGhost->critter.cur_mocap.schema_idx, pGhost->critter.cur_mocap.motion_num);
	}

	if (pPlan)
	{
		pGhost->critter.pMotCoord->SetCurrentManeuver(pPlan->PopFirstManeuver());

		dark_delete(pPlan);
	}
}

static float NextUpdate = 0.0f;
static float UpdateFrame = 0;

bool IsUpdateNeeded(int objectID)
{
	if (!_ObjIsAI(objectID) && objectID != *_gPlayerObj)
		return false;

	if (IsSimTimePassing())
	{
		if (g_pObjNet->ObjHostedHere(objectID))
		{
			float time = g_DeltaFrame.GetElapsedTime();

			if (g_DeltaFrame.GetNumFrames() == UpdateFrame)
				return true;

			if (time > NextUpdate)
			{
				NextUpdate = time + 0.1f;
				UpdateFrame = (float)g_DeltaFrame.GetNumFrames();
				//DbgPrint("Updating, time %f next %f", time, NextUpdate);
				return true;
			}
		}
	}

	return false;
}

// fix me: why isn't the new packet needed hook working for ai pred?
void GhostFrameProcessLocal(sGhostLocal* pGhost, float frameTime)
{
	//sGhostPos pos;

	if (pGhost->state & 0x20)
	{
		pGhost->cfg.flags &= ~0x80000;
	}

	if (_PhysObjHasPhysics(pGhost->obj) && !(pGhost->cfg.flags & 0x180000))
	{
		__GhostApproxPhys(pGhost->obj, pGhost->info.last.pos.pos, &pGhost->info.pred, frameTime, (pGhost->info.last.pos.flags & 0x40));

		if (!__is_zero_vec(pGhost->info.pred.pos.vel))
		{
			__GhostBleedVelocity(pGhost->obj, &pGhost->info.pred, pGhost->cfg.flags, pGhost->info.last.time, frameTime);
		}

		//IsNewPacketNeededHook(pGhost, &pos);
		//if (_IsNewPacketNeeded(pGhost, &pos))
		//	_GhostSendHeartbeat(pGhost, &pos);
		{
			// check to see whether we need to send out a position update for the local player
			if (IsUpdateNeeded(pGhost->obj))
			{
				ObjMsg_PlayerFullUpdate msg;
				cPhysModel* pPhysMod = PhysModelFromObj(pGhost->obj);

				msg.posX = (short)pPhysMod->m_location.x;
				msg.posY = (short)pPhysMod->m_location.y;
				msg.posZ = (short)pPhysMod->m_location.z;
				msg.velX = (short)pPhysMod->m_velocity.x;
				msg.velY = (short)pPhysMod->m_velocity.y;
				msg.velZ = (short)pPhysMod->m_velocity.z;
				msg.ang = pPhysMod->m_angle;

				msg.flags = 0;
				//if (pGhost->anim.schema)
				//	msg.flags |= UpdateFlagAnimation;

				//msg.anim = pGhost->anim;

				//DbgPrint("xc %0.2f xr %0.2f yc %0.2f yr %0.2f zc %0.2f zr %0.2f ang %d %d %d", (float)msg.posX, pPhysMod->m_location.x, (float)msg.posY, pPhysMod->m_location.y, (float)msg.posZ, pPhysMod->m_location.z, msg.ang.heading, msg.ang.pitch, msg.ang.bank);

				msg.motion = 0;
				if (pGhost->obj == *_gPlayerObj)
				{
					switch (_PlayerMotionGetActive())
					{
					case kMoLeanRight:
						msg.motion = kGM_LeanRight;
						break;
					case kMoLeanLeft:
						msg.motion = kGM_LeanLeft;
					default:
						msg.motion = _GetPlayerMode();
					}
				}

				ObjectSendData(pGhost->obj, OD_PlayerFullUpdate, &msg, sizeof(ObjMsg_PlayerFullUpdate), NULL);
			}
		}
		pGhost->last_fr = _GetSimTime();
	}
}

//==============================================================================
// _GhostConfigureRemoteModels()
//
// Same as original function in thief2.exe for now
//==============================================================================
void _GhostConfigureRemoteModels(sGhostRemote* pGhost)
{
	int obj = pGhost->obj;

	_PhysSetGravity(obj, 0.0f);
	_PhysSetBaseFriction(obj, 2.0f);
	__GhostSetupInitialPhysRemote(pGhost);

	if ((pGhost->cfg.flags & 0x3) == 0x3)
	{
		//DbgPrint("set flag for %d", obj);
		//cPhysModel* pModel = PhysModelFromObj(obj);
		//pModel->m_elasticity = 0;
		//pModel->m_mass = 180.0f;
		//_PhysSetFlag(pGhost->objectID, 0x480880, 1);

		_PhysSetFlag(pGhost->obj, 0x1000000, 1);
	}
}

void GhostFrameProcessRemote(sGhostRemote* pGhost, float frameTime)
{
	int object = pGhost->obj;

	if (!_PhysObjHasPhysics(object) || pGhost->cfg.flags & 0x180000)
		return;

	if (pGhost->cfg.flags & 0x8)
	{
		if (!GhostAllowedToBuildModels(pGhost))
			return;
		else
		{
			__GhostBuildMotionCoord(pGhost);
			_GhostConfigureRemoteModels(pGhost);
			//__GhostConfigureRemoteModels(pGhost);

			PackedDword* flags = (PackedDword*)&pGhost->cfg.flags;
			flags->b1 &= 0xF7;
		}
	}

	if (!(pGhost->cfg.flags & 0x10000))
	{
		UpdateGhostPrediction(pGhost, frameTime);
		_ReAimGhostPhysics(pGhost, frameTime);
		_AimGhostHead(pGhost);
		GhostPerFrameModeUpdate(pGhost);
	}

	if (!(pGhost->cfg.flags & 0x4))
	{
		_MocapEval(pGhost, frameTime);
	}
}