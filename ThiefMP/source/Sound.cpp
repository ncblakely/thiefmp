/*************************************************************
* File: Sound.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "Sound.h"
#include "SpeechNet.h"
#include "Gamesys.h"

//======================================================================================
// Name: SchemaPlayAtObj
//
// Desc: Plays given schema ID on an object.
//======================================================================================
void SchemaPlayAtObj(int schemaID, int objectID)
{
	sfx_parm sfxparms;
	const char* soundName = _SchemaSampleGet(schemaID, 0);

	SchemaParamsSetup(schemaID, sfxparms, 0);
	_GenerateSoundObj(objectID, schemaID, soundName, 1.0, &sfxparms, 0, 0);
}

//======================================================================================
// Name: DoEquipSound.
//
// Desc: Plays weapon selection sounds on remote players.
//======================================================================================
void DoEquipSound(sGhostRemote* ghost)
{
	//cTagSet tag;

	//ZeroMemory(&tag, sizeof(cTagSet));
	//cTagSetFns::FromString(&tag, NULL, "Event Select");

	//ConPrintF("esndplay: %d", _ESndPlay(&tag, ghost->weaponObj, NULL, NULL, NULL));
	//cDABaseSrvFns::DoResize((void**)&tag, sizeof(cTagSet), NULL);
	int ghostWeapArch = g_pTraitMan->GetArchetype(ghost->weap.weaponObj);
	if (ghostWeapArch)
	{
		if (ghostWeapArch == Gamesys.Arch.AvatarBow)
			SchemaPlayAtObj(-733, ghost->obj);
		else if (ghostWeapArch == Gamesys.Arch.AvatarSword)
			SchemaPlayAtObj(-735, ghost->obj);
		else if (ghostWeapArch == Gamesys.Arch.AvatarBlackjack)
			SchemaPlayAtObj(-737, ghost->obj);
		else
			Log.Print("Unknown avatar weapon ID for equip sound! (arch %d, obj %d)", ghostWeapArch, ghost->weap.weaponObj);
	}
}

//======================================================================================
// Name: DoUnEquipSound:
//
// Desc: Plays weapon deselection sounds on remote players.
//======================================================================================
void DoUnequipSound(sGhostRemote* ghost)
{
	int ghostWeapArch = g_pTraitMan->GetArchetype(ghost->weap.weaponObj);
	if (ghostWeapArch)
	{
		if (ghostWeapArch == Gamesys.Arch.AvatarBow)
			SchemaPlayAtObj(-734, ghost->obj);
		else if (ghostWeapArch == Gamesys.Arch.AvatarSword)
			SchemaPlayAtObj(-736, ghost->obj);
		else if (ghostWeapArch == Gamesys.Arch.AvatarBlackjack)
			SchemaPlayAtObj(-738, ghost->obj);
		else
			Log.Print("Unknown avatar weapon ID for detach sound! (arch %d, obj %d)", ghostWeapArch, ghost->weap.weaponObj);
	}
}

//======================================================================================
// Name: OnGenerateSoundVec
//
// Desc: Intercepts calls to GenerateSoundVec and sends certain sounds to other players.
//======================================================================================
int __stdcall OnGenerateSoundVec(mxs_vector* vec, int P1, int schemaID, char* soundName, float attenuation, sfx_parm* parms, int P3, int P4)
{
	int handle = GenerateSoundVec(vec, P1, schemaID, soundName, attenuation, parms, P3, P4);

	//if (Debug.FlagSet(DEBUG_SOUNDS))
	//	ConPrintF("Playing %s at vec %f %f %f (handle %d).", soundName, vec->x, vec->y, vec->z, handle);
	
	if (g_pNetMan->IsNetworkGame())
	{
		// Don't send sounds with loop params, since they will never be terminated for other players
		if (_SchemaLoopParamsGet(schemaID))
			return handle;

		if (g_pTraitMan->ObjHasDonor(schemaID, Gamesys.Arch.PLYR_DAMAGE))
		{
			if (Debug.IsFlagSet(DEBUG_SOUNDS))
				ConPrintF("Sending player damage sound with handle %d", handle);
			_SoundNetGenerateSoundVec(handle, vec, schemaID, soundName, attenuation, parms);
		}
	}

	return handle;
}

//======================================================================================
// Name: OnGenerateSoundObj
//
// Desc: Intercepts calls to GenerateSoundObj and sends certain sounds to other players.
//======================================================================================
int __stdcall OnGenerateSoundObj(int objectID, int schemaID, char* soundName, float atten, struct sfx_parm * parms, int P3, int P4)
{
	int handle = _GenerateSoundObj(objectID, schemaID, soundName, atten, parms, P3, P4);

	if (schemaID == Gamesys.Arch.AI_SPEECH)
		ConPrintF("Playing AI speech %s", soundName);

	if (g_pNetMan->IsNetworkGame())
	{
		if (g_pObjNet->ObjHostedHere(objectID))
		{
			if (schemaID == Gamesys.Arch.pinset || schemaID == Gamesys.Arch.tumblerset)
			{
				if (Debug.IsFlagSet(DEBUG_SOUNDS))
					ConPrintF("Sending object-attached sound %s.", soundName);

				_SoundNetGenerateSoundObj(handle, objectID, schemaID, soundName, atten, parms);
			}

		}
	}

	return handle;
}

//======================================================================================
// Name: OnGenerateSound
//
// Desc: Intercepts calls to GenerateSound and sends certain sounds to other players.
//======================================================================================
int __stdcall OnGenerateSound(int schemaID, char* soundName, sfx_parm* parms)
{
	//if (Debug.FlagSet(DEBUG_SOUNDS))
	//	ConPrintF("Playing simple sound %s.", soundName);
	int handle = _GenerateSound(soundName, parms);

	if (g_pNetMan->IsNetworkGame())
	{
		// Don't send sounds with loop params, since they will never be terminated for other players
		if (_SchemaLoopParamsGet(schemaID))
			return handle;
		if (handle != -1)
		{
			if (g_pTraitMan->ObjHasDonor(schemaID, Gamesys.Arch.PLYR_WEAPON))
			{
				if (Debug.IsFlagSet(DEBUG_SOUNDS))
					ConPrintF("Sending player weapon sound %s.", soundName);

				_SoundNetGenerateSoundObj(handle, *_gPlayerObj, schemaID, soundName, _SchemaAttFacGet(schemaID), parms);
			}
		}
	}

	return handle;
}

//void __stdcall SpeechPropListener(sPropertyListenMsg* listenMsg, PropListenerData)
//{
//	if (g_pNetMan->IsNetworkGame())
//	{
//		int object = listenMsg->objectID;
//		if (g_pObjNet->ObjHostedHere(object))
//		{
//			sSpeech* pSpeech = NULL;
//
//			if (g_pSpeechProp->Get(object, (void**)&pSpeech))
//			{
//				if (pSpeech->schemaID)
//				{
//					ConPrintF("Concept %s for schema %s.", cNameMapFns::NameFromID(g_Domain, NULL, pSpeech->concept), _ObjEditName(pSpeech->schemaID));
//					CNetMsg_AiSpeech msg;
//
//					msg.speaker = g_pNetMan->ToGlobalObjID(object);
//					msg.schemaID = (short)pSpeech->schemaID;
//					msg.flags = pSpeech->flags;
//					msg.concept = (BYTE)pSpeech->concept;
//
//					ConPrintF("Sending speech for %s: schema %d (%x - %d).", _ObjEditName(object), msg.schemaID, pSpeech->pSchemaPlay, pSpeech->pSchemaPlay->schemaID);
//
//					if (g_pDarkNet)
//						g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, sizeof(CNetMsg_AiSpeech), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
//				}
//			}
//		}
//		//ConPrintF("Speech prop changed for %s, event ID %d, flags %x, P1 %x", _ObjEditName(listenMsg->objectID), listenMsg->eventID, listenMsg->flags, listenMsg->P1);
//	}
//}

sSchemaPlay* SchemaIDPlay(int schemaID, sSchemaCallParams* pParams, void* P3)
{
	sSchemaPlay* pSchemaPlay = SchemaPlayAdd(schemaID, pParams);

	if (pSchemaPlay)
	{
		int P2;

		if (pParams && pParams->callFlags & 0x10)
			P2 = pParams->P2;
		else
			P2 = -1;

		int sample = SchemaChooseSample(schemaID, P2);
		if (sample == -1)
		{
			if (pSchemaPlay->next)
				pSchemaPlay->next->prev = pSchemaPlay->prev;
			else
				g_pLastSchemaPlay->next = pSchemaPlay->prev;

			if (pSchemaPlay->prev)
				pSchemaPlay->prev->next = pSchemaPlay->next;
			else
				playingSchemas->next = pSchemaPlay->next;

			dark_delete(pSchemaPlay);
			SpeechNetHandler(schemaID, false, pParams);
			return 0;
		}

		sSchemaLoopParams* pLoopParams = _SchemaLoopParamsGet(pSchemaPlay->schemaID);
		int ret = SchemaSamplePlayAndSetupNext(schemaID, sample, pSchemaPlay, pLoopParams, P3);
		if (ret == -1 && !pLoopParams)
		{
			if (pSchemaPlay->next)
				pSchemaPlay->next->prev = pSchemaPlay->prev;
			else
				g_pLastSchemaPlay->next = pSchemaPlay->prev;

			if (pSchemaPlay->prev)
				pSchemaPlay->prev->next = pSchemaPlay->next;
			else
				playingSchemas->next = pSchemaPlay->next;

			dark_delete(pSchemaPlay);
			SpeechNetHandler(schemaID, false, pParams);
			return 0;
		}
	}

	SpeechNetHandler(schemaID, (pSchemaPlay != 0), pParams);
	return pSchemaPlay;
}

sSchemaPlay* NetSchemaPlaySample(int schemaID, int sample, sSchemaCallParams* pParams, void* P3)
{
	sSchemaPlay* pSchemaPlay = SchemaPlayAdd(schemaID, pParams);

	if (pSchemaPlay)
	{
		int P2;

		if (pParams && pParams->callFlags & 0x10)
			P2 = pParams->P2;
		else
			P2 = -1;

		if (sample == -1)
		{
			if (pSchemaPlay->next)
				pSchemaPlay->next->prev = pSchemaPlay->prev;
			else
				g_pLastSchemaPlay->next = pSchemaPlay->prev;

			if (pSchemaPlay->prev)
				pSchemaPlay->prev->next = pSchemaPlay->next;
			else
				playingSchemas->next = pSchemaPlay->next;

			dark_delete(pSchemaPlay);
			SpeechNetHandler(schemaID, false, pParams);
			return 0;
		}

		sSchemaLoopParams* pLoopParams = _SchemaLoopParamsGet(pSchemaPlay->schemaID);
		int ret = SchemaSamplePlayAndSetupNext(schemaID, sample, pSchemaPlay, pLoopParams, P3);
		if (ret == -1 && !pLoopParams)
		{
			if (pSchemaPlay->next)
				pSchemaPlay->next->prev = pSchemaPlay->prev;
			else
				g_pLastSchemaPlay->next = pSchemaPlay->prev;

			if (pSchemaPlay->prev)
				pSchemaPlay->prev->next = pSchemaPlay->next;
			else
				playingSchemas->next = pSchemaPlay->next;

			dark_delete(pSchemaPlay);
			SpeechNetHandler(schemaID, false, pParams);
			return 0;
		}
	}

	return pSchemaPlay;
}

void OnSpeechHalt(int object)
{
	sSpeech* pSpeech;

	if (g_pSpeechProp->Get(object, (void**)&pSpeech))
	{
		if (pSpeech->flags & 0x1)
		{
			//ConPrintF("Halting speech schema %s on %d. Concept: %s.", _ObjEditName(pSpeech->schemaID), object, cNameMapFns::NameFromID(g_Domain, NULL, pSpeech->concept));
			_SchemaPlayHalt(pSpeech->pSchemaPlay);

			if (g_pNetMan->IsNetworkGame() && g_pObjNet->ObjHostedHere(object) && _SchemaLoopParamsGet(pSpeech->schemaID))
			{
				CNetMsg_LoopSpeechHalt msg;

				msg.speaker = g_pNetMan->ToGlobalObjID(object);

				if (Debug.IsFlagSet(DEBUG_SOUNDS))
					ConPrintF("Net halting looping speech %s on %d.", _ObjEditName(pSpeech->schemaID), object);

				if (g_pDarkNet)
					g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, sizeof(CNetMsg_LoopSpeechHalt), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
			}
		}

		g_pSpeechProp->Delete(object);
	}
}