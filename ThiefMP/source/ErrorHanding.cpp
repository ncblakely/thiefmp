/*************************************************************
* File: ErrorHandling.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Hacks to prevent some bugs in the engine from crashing 
* the game entirely.
*************************************************************/


#include "stdafx.h"
#include "Main.h"

void __stdcall CollisionResolverHook(const tSpringInfo* springInfo, DWORD P2)
{
	try
	{
		CollisionResolver(springInfo, P2);
	}

	catch (...)
	{
		Log.Print("Recovered from collision resolver exception.");

		if (Debug.IsFlagSet(DEBUG_GENERAL))
			MessageMgr::Get()->AddLine(false, "Debug: Spring info array bounds exceeded in ResolveCollision.");
	}
}

int CellBinComputeFuncHook(int objectID, DWORD P2, DWORD P3)
{
	if (g_pObjSys->Exists(objectID))
		return _CellBinComputeFunc(objectID, P2, P3);
	else
		return 0;
}

void ObjectAnglesFuncHook(int object)
{
	Position* pos = _ObjPosGet(object);
	if (!pos)
		return;
	else
		_r3_start_object_angles(&pos->vec, &pos->bank, 6);
}

int __stdcall OnCollideEvent(int collider, int collidee, int P3, float force, class cPhysClsn* pClsn)
{
	if (_ObjIsAI(collider) && !g_pObjNet->ObjHostedHere(collider))
		return CollideEventRaw(collider, collidee, P3, force, pClsn);
	else
		return CollideEvent(collider, collidee, P3, force, pClsn);
}

void LoopSampleCallbackHook(DWORD P1, int handle)
{
	try
	{
		_LoopSampleCallback(P1, handle);
	}

	catch (...)
	{
		const char* str = "Exception in loop sample callback (%x handle %d)";

		Log.Print(str, P1, handle);

		if (Debug.IsFlagSet(DEBUG_GENERAL))
			MessageMgr::Get()->AddLineFormat(true, str, P1, handle);
	}
}

int __fastcall PropSndEnterCBHook(cPropSndInstHigh* pInst, int, const cRoom* pRoom, const cRoomPortal* pPortal, const mxs_vector& vec, float P4)
{
	try
	{
		return cPropSndInstHighFns::EnterCallback(pInst, NULL, pRoom, pPortal, vec, P4);
	}

	catch (...)
	{
		const char* str = "Sound propagation error: %s on %d (%d)";

		Log.Print(str, pInst->sndName, pInst->attachedObject, pInst->sndSchema);

		if (Debug.IsFlagSet(DEBUG_GENERAL))
			MessageMgr::Get()->AddLineFormat(true, str, pInst->sndName, pInst->attachedObject, pInst->sndSchema);
		return 0;
	}
}

void SpeechEndCallbackHook(int objectID, int P2, int sndSchema)
{
	try
	{
		cAIConverse::SpeechEndCallback(objectID, P2, sndSchema);
	}

	catch (...)
	{
		const char* str = "Exception in SpeechEnd: tried to finish playing %s on %d.";

		Log.Print(str, _ObjEditName(sndSchema), objectID);

		if (Debug.IsFlagSet(DEBUG_GENERAL))
			MessageMgr::Get()->AddLineFormat(true, str, _ObjEditName(sndSchema), objectID);
	}
}

//////////////
// Safer replacements for Dark Engine math functions.

void mx_sub_vec_safe(mxs_vector* newVec, const mxs_vector* vec1, const mxs_vector* vec2)
{
	if (vec1 && vec2)
	{
		newVec->x = vec1->x - vec2->x;
		newVec->y = vec1->y - vec2->y;
		newVec->z = vec1->z - vec2->z;
	}
	else
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		if (Debug.IsFlagSet(DEBUG_GENERAL))
			MessageMgr::Get()->AddLineFormat(false, "DEBUG: vec %d is null on vector subtract.", vec1 ? 1 : 2 );
		_mx_zero_vec(*newVec);
	}
}

float mx_dist_vec_safe(mxs_vector* vec1, mxs_vector* vec2)
{
	mxs_vector newVec;

	if (vec1 && vec2)
	{
		newVec.x = vec2->x - vec1->x;
		newVec.y = vec2->y - vec1->y;
		newVec.z = vec2->z - vec1->z;

		return _mx_mag_vec(newVec);
	}
	else
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		if (Debug.IsFlagSet(DEBUG_GENERAL))
			MessageMgr::Get()->AddLineFormat(false, "DEBUG: vec %d is null on vector distance test.", vec1 ? 1: 2);
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

void mx_subeq_vec(mxs_vector* v1, mxs_vector* v2)
{
	v1->x -= v2->x;
	v1->y -= v2->y;
	v1->z -= v2->z;
}

void mx_scale_vec(mxs_vector* v1, mxs_vector* v2, float scale)
{
	v1->x = v2->x * scale;
	v1->y = v2->y * scale;
	v1->z = v2->z * scale;
}

void mx_scaleeq_vec(mxs_vector* v, float scale)
{
	v->x *= scale;
	v->y *= scale;
	v->z *= scale;
}