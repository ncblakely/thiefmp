/*************************************************************
* File: Motions.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/


#include "stdafx.h"
#include "Main.h"

cGroundActionManeuverFactory GroundActionManeuverFactory;
cPlayerManeuverFactory PlayerManeuverFactory;

namespace HookGroundActionManeuverFactory
{

//======================================================================================
// Name: CreatePlan
//
// Desc: Hooks requests to create ground action motion plans. GroundActionManeuverFactory::CreatePlan then calls
// one of four different GroundActionManeuver constructors, only one of which was made to send motion updates to 
// other players by LGS. This resulted in missing animations for proxy AI, and has been fixed here.
//
// The call to GhostSendMoCap in the one constructor that this was implemented in is disabled at startup, since it is
// no longer necessary.
//======================================================================================
cMotionPlan* __stdcall CreatePlan(const cMotionSchema* pSchema, struct sMcMotorState &pMotorState, struct sMcMoveState &pMoveState, sMcMoveParams& pParams, class IMotor* pMotor, class cMotionCoordinator *pMotCoord)
{
	cMotionPlan* pPlan;
	int schema;

	if (!pMotor)
	{
		const char* msg = "ERROR: invalid motor passed to ground action maneuver factory.";
		Log.Print(msg);
		MessageMgr::Get()->AddLine(true, msg);

		return NULL;
	}

	
	pPlan = GroundActionManeuverFactory.CreatePlan(pSchema, pMotorState, pMoveState, pParams, pMotor, pMotCoord);

	// Send information about the new animation to other players if the object is a local ghost
	if (pPlan && _IsLocalGhost(pMotor->m_creature.GetObjID()))
	{
		// Extract the schema ID from the newly created motion plan
		__asm
		{
			mov eax, [pPlan]
			mov eax, [eax+0x4]
			mov eax, [eax+0x3A]
			mov [schema], eax
		}

		//if (Debug.FlagSet(DEBUG_SENDS))
		//	ConPrintF("Sending mocap for %s (%d:%d)", _ObjEditName(pMotor->m_creature.GetObjID()), pSchema->motionID, schema);
		_GhostSendMoCap(pMotor->m_creature.GetObjID(), pSchema->motionID, schema, 0);
	}

	return pPlan;
}

} // HookGroundActionManeuver

namespace HookPlayerManeuverFactory
{

cMotionPlan* __stdcall CreatePlan(const cMotionSchema* pSchema, struct sMcMotorState &pMotorState, struct sMcMoveState &pMoveState, sMcMoveParams& pParams, class IMotor* pMotor, class cMotionCoordinator *pMotCoord)
{
	if (!pMotor)
	{
		const char* msg = "ERROR: invalid motor passed to player maneuver factory.";
		Log.Print(msg);
		MessageMgr::Get()->AddLine(true, msg);

		return NULL;
	}
	else
		return PlayerManeuverFactory.CreatePlan(pSchema, pMotorState, pMoveState, pParams, pMotor, pMotCoord);
}

} // HookPlayerManeuverFactory

namespace HookGroundLocoManeuverFactory
{

cMotionPlan* __stdcall CreatePlan(const cMotionSchema* pSchema, struct sMcMotorState &pMotorState, struct sMcMoveState &pMoveState, sMcMoveParams& pParams, class IMotor* pMotor, class cMotionCoordinator *pMotCoord)
{
	if (!pMotor)
	{
		const char* msg = "ERROR: invalid motor passed to ground loco maneuver factory.";
		Log.Print(msg);
		MessageMgr::Get()->AddLine(true, msg);

		return NULL;
	}
	else
		return cGroundLocoManeuverFactory::CreatePlan(pSchema, pMotorState, pMoveState, pParams, pMotor, pMotCoord);
}

}	// HookPlayerManeuverFactory

namespace HookMotionCoordinator
{

}