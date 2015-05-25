/*************************************************************
* File: ProxyActions.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Main.h"

namespace HookAIBehaviorSet
{

void __stdcall EnactProxyJointScanAction(IAIBehaviorSet* pBehaviorSet, IAI* pAI, void* P2)
{
	if (Debug.IsFlagSet(DEBUG_GLOBAL))
		ConPrintF("Enacting proxy joint scan action.");

	return cAIBehaviorSet::EnactProxyJointScanAction(pBehaviorSet, pAI, P2);
}

} // end HookAIBehaviorSet