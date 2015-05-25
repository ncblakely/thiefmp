/*************************************************************
* File: Damage.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"
#include "Main.h"
#include "Client.h"
#include "Callbacks.h"

unsigned long cBaseDamageModel::ApplyFilters(int victim, int culprit, sDamage* pDmg)
{
	ulong ret = 0;

	for (uint i = 0; i < m_DamageFilters.Size(); i++)
	{
		ulong tempret = m_DamageFilters[i].filter(victim, culprit, pDmg, m_DamageFilters[i].pData);
		
		if (tempret > 0)
			ret = tempret;
	}

	return ret;
}

int cBaseDamageModel::AlreadySentMsg(unsigned long P1, int object)
{
	if (P1 & 0xC)
	{
		int stage = 0;

		m_pDeathStageProp->Get(object, &stage);

		if (stage & P1)
			return 1;
	}

	return 0;
}

unsigned long cBaseDamageModel::ApplyResult(int victim, int culprit, unsigned long result, sChainedEvent* pEvent)
{
	switch (result)
	{
	case DR_SlayObject:
		return SlayObject(victim, culprit, pEvent);
	case DR_TerminateObject:
		return TerminateObject(victim, pEvent);
	case DR_ResurrectObject:
		return ResurrectObject(victim, culprit, pEvent);
	case DR_DestroyObject:
			m_pObjSys->Destroy(victim);
		// intentional fall-through
	}

	return result;
}

unsigned long cBaseDamageModel::SendMessage(const sDamageMsg* pDmgMsg)
{
	//int victim = pDmgMsg->victimID;
	//if (victim)
	//{
	//	if (pDmgMsg->event.data & 0xC)
	//	{
	//		int stage = 0;
	//		if (!m_pDeathStageProp->Get(victim, &stage))
	//			return 0;

	//		m_pDeathStageProp->Set(victim, stage | pDmgMsg->event.data);
	//	}

	//	for (uint i = 0; i < m_DamageListeners.Size(); i++)
	//	{
	//		m_DamageListeners[i].listener(pDmgMsg, m_DamageListeners[i].pData);
	//}
	//}
	//else
	return 0;
}

bool DoMultiDamage(int victim, int culprit, sDamage* pDmg, sChainedEvent* pEvent)
{
	if (g_pNetMan->IsNetworkGame())
	{
	}
	return false;
}

unsigned long __stdcall HookSimpleDamageModel2::OnDamageObject(int victim, int culprit, sDamage* pDmg, sChainedEvent* pEvent, int P4)
{
	sDamageMsg dmgMsg;
	int hitPts;
	ulong ret = 0;

	//if (DoMultiDamage(victim, culprit, pDmg, pEvent))
	//	return;

	ulong filterRet = ApplyFilters(victim, culprit, pDmg);

	if (pDmg->dmgPoints || P4)
	{
		if (AlreadySentMsg(4, victim))
			return 1;
		else
		{
			if (_ObjGetHitPoints(victim, &hitPts))
			{
				hitPts -= pDmg->dmgPoints;

				if (hitPts <= 0)
				{
					if (filterRet <= 4)
						filterRet = 4;
				}

				dmgMsg.dmg = pDmg;
				dmgMsg.event.eventID = 3;
				dmgMsg.event.P1 = 0x1C;
				dmgMsg.event.next = pEvent;
				dmgMsg.event.data = 2;
				dmgMsg.culpritID = culprit;
				dmgMsg.victimID = victim;
				_ObjSetHitPoints(victim, hitPts);
			}
		}
	}

	ulong messageRet = cBaseDamageModelFns::SendMessage(this, NULL, (sDamageMsg*)&dmgMsg);
	if (messageRet > filterRet)
		filterRet = messageRet;
		
	return ApplyResult(victim, culprit, filterRet, pEvent);
}

unsigned long __stdcall HookDarkCombatDamageListener(const sDamageMsg* dmgMsg, void* pData)
{
	//dbgassert(g_pObjNet->ObjHostedHere(dmgMsg->victimID));

	//if (g_pObjNet->ObjHostedHere(dmgMsg->victimID))
	//{
	//	switch ((dmgMsg->event.data))
	//	{
	//		case DR_Damage:
	//		if (g_pObjNet->ObjHostedHere(dmgMsg->victimID))
	//		{
	//			const sChainedEvent* pHitEvent;
	//			CNetMsg_ObjectDamaged msg;

	//			msg.obj = g_pNetMan->ToGlobalObjID(dmgMsg->victimID);
	//			msg.result = (BYTE)dmgMsg->event.data;
	//			msg.stimulus = dmgMsg->dmg->stimArch;

	//			pHitEvent = const_cast<sDamageMsg*>(dmgMsg)->event.Find(1);
	//			if (pHitEvent)
	//			{
	//				DamageChainedEventData* pHitData = (DamageChainedEventData*)pHitEvent->data;
	//				msg.hitPos = pHitData->hitPos;

	//				DbgPrint("Sending blood position: %0.2f %0.2f %0.2f", msg.hitPos.x, msg.hitPos.y, msg.hitPos.z);
	//			}
	//			else
	//			{
	//				DbgPrint("Could not find hit event");

	//				ZeroMemory(&msg.hitPos, sizeof(mxs_vector));
	//			}

	//			g_pDarkNet->SendToAll((void*)&msg, sizeof(CNetMsg_ObjectDamaged), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
	//		}
	//		break;

	//		default:
	//			break;
	//	}
	//}

	return DarkCombatDamageListener(dmgMsg, pData);
}

unsigned long __stdcall DarkDamageListener(const sDamageMsg* dmgMsg, void* pData)
{
	if (g_Net)
	{
		switch ((dmgMsg->event.data))
		{
		case DR_Damage:
			if (Debug.IsFlagSet(DEBUG_DAMAGE))
				ConPrintF("Dark damage listener: %s was damaged by %d from stim %d.", _ObjEditName(dmgMsg->victimID), dmgMsg->culpritID, dmgMsg->dmg->stimArch);

			if (dmgMsg->victimID == *_gPlayerObj)
			{
				if (dmgMsg->dmg->dmgPoints > 0)
					_GhostNotify(*_gPlayerObj, GHOST_ACTION_WOUNDED);
			}
			break;
		case DR_SlayObject:
			if (Debug.IsFlagSet(DEBUG_DAMAGE))
				ConPrintF("%s was destroyed by %d.", _ObjEditName(dmgMsg->victimID));

			if (dmgMsg->victimID == *_gPlayerObj)
			{
				int realCulprit = _GetRealCulprit(dmgMsg->culpritID);

				CNetMsg_PlayerDeath msg(g_pNetMan->MyPlayerNum(), realCulprit);

				msg.Send(ALL_PLAYERS, DPNSEND_GUARANTEED);
			}
			break;
		}
	}
	return 0;

	//if (!g_pObjNet->ObjHostedHere(dmg->victimID) && !g_pObjNet->ObjLocalOnly(dmg->victimID))
	//{
		//ConPrintF("Releasing blood for %s. Culprit: %d", _ObjEditName(dmg->victimID), dmg->culpritID);
		//ReleaseBlood(dmg);
	//}
}

namespace HookSimpleDamageModel
{

long __stdcall DamageObject(DWORD thisObj, int victim, int culprit, sDamage* dmg, struct sChainedEvent* chainedEvent, int P3)
{
	if (g_pDarkNet)
	{
		if (!g_pDarkNet->GetSessionInfo().teamDamage)
		{
				if (_IsAPlayer(victim) && _GetRealCulprit(culprit) == *_gPlayerObj)
					return 0;
		}
		if (Debug.IsFlagSet(DEBUG_DAMAGE))
		{
			ConPrintF("%s was damaged by %d for %d points.", _ObjEditName(victim), culprit, dmg->dmgPoints);

			//int localCopy;
			//if ((*g_pLocalCopyProp)->Get(culprit, &localCopy))
			//	ConPrintF("%s is local copy", _ObjEditName(culprit));
		}
	}

	return cSimpleDamageModelFns::DamageObject(thisObj, victim, culprit, dmg, chainedEvent, P3);
}

} // end HookSimpleDamageModel