/*************************************************************
* File: SpeechNet.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "SpeechNet.h"
#include "Gamesys.h"

int GetAIVoice(int object)
{
	sLink link;
	int voiceArch;
	ILinkQuery* pQuery = g_pLinkMan->Query(object, NULL, ppVoiceLink->GetID());

	if (pQuery)
	{
		if (!pQuery->Done())
		{
			pQuery->Link(&link);
			voiceArch = link.destObj;
		}
		else
			voiceArch = -1;

		pQuery->Release();
	}
	else
		voiceArch = -1;

	return voiceArch;
}

void PrintSpeechDebug(int object, int schemaID, int sampleNum, int conceptID, const Label* pConceptLbl, bool bPlayedHere)
{
	if (Debug.IsFlagSet(DEBUG_SOUNDS))
	{
		int schemaHandle = g_SpeechHandles->GetIndex(conceptID);

		ConPrintF("Sending speech for %s: schema %d, sample %d, schema handle %d, file %s, concept %s, played here %d.", _ObjEditName(object), schemaID, sampleNum, schemaHandle, "", pConceptLbl, bPlayedHere);
	}
}

void SpeechNetHandler(int schemaID, bool bPlayedHere, const sSchemaCallParams* pParams)
{
	if (!pParams)
		return;

	if (g_pNetMan->IsNetworkGame() && pParams->callFlags == 0xB0)
	{
		int object = pParams->object1;
		if (g_pObjNet->ObjHostedHere(object) && g_pTraitMan->ObjHasDonor(schemaID, Gamesys.Arch.AI_SPEECH))
		{
			CNetMsg_AiSpeech msg;
			int conceptID = cNameMapFns::IDFromName(g_Domain, NULL, pParams->conceptName);

			msg.speaker = g_pNetMan->ToGlobalObjID(object);
			msg.schemaID = (short)schemaID;
			msg.concept = (BYTE)conceptID;
			msg.sampleNum = -1;

			if (bPlayedHere)
			{
				int sampleNum;
				if (g_pPropSchemaLastSample->Get(schemaID, &sampleNum))
					msg.sampleNum = sampleNum;
					//ConPrintF("Local play last sample %d (%s)", sampleNum, _SchemaSampleGet(schemaID, sampleNum));
			}

			PrintSpeechDebug(object, schemaID, msg.sampleNum, conceptID, pParams->conceptName, bPlayedHere);

			SpeechBroadcast(msg);
		}
	}
}

void SpeechBroadcast(CNetMsg_AiSpeech& msg)
{
	if (g_pDarkNet)
		g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, sizeof(CNetMsg_AiSpeech), NULL, DPNSEND_NOLOOPBACK | DPNSEND_COALESCE);
}