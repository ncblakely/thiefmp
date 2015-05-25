/*************************************************************
* File: AIManager.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements VTable hooks of AI-related methods.
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "SpeechNet.h"
#include "Client.h"
#include "Gamesys.h"

namespace HookAIManager
{

int __stdcall TransferAI(IAIManager* thisObj, int aiObject, int playerObject)
{
	// Cameras shouldn't be transferred, since their looping alarm sound will never be terminated if they are transferred away from the local player after it has begun.
	if (g_pTraitMan->ObjHasDonor(aiObject, Gamesys.Arch.Cameras))
		return 1;

	return cAIManager::TransferAI(thisObj, aiObject, playerObject);
}

int __stdcall CreateAI(IAIManager* thisObj, int index, const char* aiType)
{
	int ret = cAIManager::CreateAI(thisObj, index, aiType);

	//g_pAIMan->ListenForCollisions(index);

	//DbgPrint("CreateAI type %s ind %d ret %d", aiType, index, ret);

	//g_pAISeesProjectileProperty->Set(index, TRUE);

	return ret;
}

int __stdcall StartConversation(IAIManager* thisObj, int convID)
{
	if (Client.GameIsEnding())
		return 1;

	if (g_Net == STATE_Client)
		return 1;
	else if (g_Net == STATE_Host)
	{
		// Loop through the conversors and make sure that they are all controlled locally -- if not, return without starting the conversation
		// A better solution would be to prevent AIs with conversation links from being handed off to clients, but I don't know how to do that yet
		for (int i = 0; i < kAICA_Num; i++)
		{
			int aiObj = 0;

			thisObj->m_ConvManager->GetActorObj(convID, i, &aiObj);
			if (aiObj)
			{
				if (!g_pObjNet->ObjHostedHere(aiObj))
					return 1;
			}
		}
	}

	return cAIManager::StartConversation(thisObj, convID);
}

} // end HookAIManager

namespace HookAINetManager
{

long __stdcall MakeProxyAI(IAINetManager* pThis, int aiObject, int playerObject)
{
	CNetMsg_TransferAI msg;
	IAI* pAI;
	DPNID dpnidPlayer;

	if (pThis->IsProxy(aiObject))
	{
		ConPrintF("%s is a proxy.", _ObjEditName(aiObject));
		return E_FAIL;
	}

	msg.aiObject = g_pNetMan->ToGlobalObjID(aiObject);
	msg.position = *(mxs_vector*)_ObjPosGet(aiObject);
	msg.voiceArch = (short)GetAIVoice(aiObject);
	msg.awarenessLevel = 0;

	pAI = g_pAIMan->GetAI(aiObject);

	// Try and get the AI's current awareness of the player, and include it in the transfer message
	if (pAI)
	{
		const sAIAwareness* pAwareness = pAI->GetAwareness(playerObject);

		if (pAwareness)
			msg.awarenessLevel = pAwareness->level;

		pAI->Release();
	}

	g_pAIMan->DestroyAI(aiObject);
	g_pObjNet->ObjGiveWithoutObjID(aiObject, playerObject);

	dpnidPlayer = g_pNetMan->ToNetPlayerID(playerObject);
	if (dpnidPlayer > 0 && dpnidPlayer != *cNetManager::gm_PlayerDPID)
	{
		if (Debug.IsFlagSet(DEBUG_SENDS))
			ConPrintF("Transferring %s to %d (%f %f %f - %d %d)", _ObjEditName(aiObject), playerObject, msg.position.x, msg.position.y, msg.position.z, msg.awarenessLevel, msg.voiceArch);

		if (g_pDarkNet)
			g_pDarkNet->Send(dpnidPlayer, (void*)&msg, sizeof(CNetMsg_TransferAI), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
	}
	else
	{
#ifndef _RELEASE
		assert(false && "bad DPNID for AI transfer");
#endif
	}

	sAIProp* pAIProp = NULL;
	g_pAIProperty->Get(aiObject, (void**)&pAIProp);
	g_pAIIsProxyProperty->Set(aiObject, 1);

	g_pAIMan->CreateAI(aiObject, pAIProp->behaviorSet);

	return S_OK;
}

} // end HookAINetManager