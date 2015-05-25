/*************************************************************
* File: NetProperties.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Broadcasts Act/React events to other players or the host.
*************************************************************/

#include "stdafx.h"

#include "NetReactions.h"
#include "DarkStat.h"

NetworkableReactionInfo NetworkedReactions[] =
{
	"slay", SlayReactionHandler, NULL, (void**)g_ppDamageModel, NULL,
	"Knockout", KnockoutReactionHandler, FilterKnockoutReaction, NULL, NULL,
	//"add_metaprop", AddMetaPropReactionHandler, NULL, NULL, NULL,
	"frob_obj", FilterFrobObjectReactionHandler, NULL, NULL, NETREACT_ONLYLOOPBACK,
};

NetworkableReactionInfo* GetNetworkedReaction(unsigned long reactionID)
{
	for (int i = 0; i < sizeof(NetworkedReactions) / sizeof(NetworkableReactionInfo); i++)
	{
		if (g_pActReact->GetReactionNamed(NetworkedReactions[i].name) == reactionID)
			return &NetworkedReactions[i];
	}

	return NULL;
}

void PostReactionMessage(unsigned long reactionID, sReactionEvent* pEvent, sReactionParam* pParams)
{
	if (!OBJ_IS_CONCRETE(pParams->reactingObj))
		return;

	NetworkableReactionInfo* pInfo = GetNetworkedReaction(reactionID);

	if (pInfo)
	{
		if (pInfo->reactFlags & NETREACT_ONLYLOOPBACK)
		{
			if (Debug.IsFlagSet(DEBUG_GENERAL))
				DbgPrint("Doing loopback for %s. Reacting object %d/%d, reaction instigator %d.", pInfo->name, pEvent->reactingObj, pParams->reactingObj, pParams->instigatorObj);

			pInfo->handler(pEvent, pParams, pInfo->handlerData ? *pInfo->handlerData : NULL);
			return;
		}

		CNetMsg_GenericReaction msg((short)reactionID, pEvent, pParams);

		DPNID dpnidObjHost = g_pNetMan->ToNetPlayerID(g_pObjNet->ObjHostPlayer(pParams->reactingObj));

		if (dpnidObjHost)
		{
	/*		DbgPrint("Sending generic reaction: type %d, param reacting %d param inst %d param flags %d, num events %d, event P2 %d, event reacting %d", 
				msg.m_reactionID, msg.m_reactParams.reactingObj, msg.m_reactParams.instigatorObj, msg.m_reactParams.flags, msg.m_numChainedEvents, msg.m_reactEvent.P2, msg.m_reactEvent.reactingObj);*/

			//DbgPrint("Start sent events:");
			//PrintEventChain(pEvent->event);

			int bytesSent = msg.Send(dpnidObjHost, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);

			if (Debug.IsFlagSet(DEBUG_GENERAL))
				DbgPrint("Posted %s. Reacting object %d/%d, reaction instigator %d. Size: %d.", pInfo->name, pEvent->reactingObj, pParams->reactingObj, pParams->instigatorObj, bytesSent);
		}

		if (pInfo->reactFlags & NETREACT_LOOPBACK)
			pInfo->handler(pEvent, pParams, pInfo->handlerData ? *pInfo->handlerData : NULL);
	}
}

unsigned long __stdcall FilterFrobObjectReactionHandler(sReactionEvent* pEvent, const sReactionParam* pParams, void*)
{
	// Local filter for frob_obj
	if (!g_pObjNet->ObjHostedHere(pEvent->reactingObj))
	{
		//ConPrintF("Taking over %s. (event)", _ObjEditName(pEvent->reactingObj));
		g_pObjNet->ObjTakeOver(pEvent->reactingObj);
	}

	if (!g_pObjNet->ObjHostedHere(pParams->reactingObj))
	{
		//ConPrintF("Taking over %s. (params)", _ObjEditName(pParams->reactingObj));
		g_pObjNet->ObjTakeOver(pParams->reactingObj);
	}

	return cReactions::React(g_pReactions, g_pReactions->GetReactionNamed("frob_obj"), pEvent, pParams);
}

bool __stdcall FilterKnockoutReaction(DPNID dpnidFrom, sReactionEvent* pEvent, const sReactionParam* pParams, void*)
{
	//ConPrintF("KO: %s.", _ObjEditName(pEvent->reactingObj));
	//if (g_pTraitMan->ObjHasDonorIntrinsically(pEvent->reactingObj, g_pObjSys->GetObjectNamed("GetsKnockedOutEasily")))
	//{
	//	ConPrintF("%s gets knocked out easily.", _ObjEditName(pEvent->reactingObj));
	//	return true;
	//}

	int awareness;
	sReactionParam param;
	param.instigatorObj = g_pNetMan->FromNetPlayerID(dpnidFrom);
	param.reactingObj = pEvent->reactingObj;
	param.flags = AWAREFILT_ABORTCANTSEE | AWAREFILT_ABORTSTUNNED;

	awareness = AwarenessFilter(NULL, &param, NULL);
	if (awareness < 2)
	{
		g_pTraitMan->AddObjMetaProperty(pEvent->reactingObj, g_pObjSys->GetObjectNamed("M-KnockedOut"));
		g_pTraitMan->AddObjMetaProperty(pEvent->reactingObj, g_pObjSys->GetObjectNamed("M-WasKnockedOut"));

		_DarkStatIntAdd("DrSKnockout", 1);
		return true;
	}

	//if (Debug.FlagSet(DEBUG_GENERAL))
	//	ConPrintF("Aborting knockout on %s: object is aware of player.", _ObjEditName(pEvent->reactingObj));
	return false;
}