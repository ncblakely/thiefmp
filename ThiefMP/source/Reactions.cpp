/*************************************************************
* File: Reactions.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements VTable hooks of cReactions methods.
*************************************************************/

#include "stdafx.h"
#include "Main.h"
#include "Marshal.h"
#include "NetReactions.h"

void __stdcall HookReactions::OnReact(unsigned long reactionID, sReactionEvent* pEvent, sReactionParam* pParams)
{
	if (g_pNetMan->IsNetworkGame())
	{
		if (pParams->reactingObj && !g_pObjNet->ObjHostedHere(pParams->reactingObj))
		{
			//DbgPrint("Trying to post reaction %s on %s", g_pReactions->DescribeReaction(reactionID)->description, _ObjEditName(pParams->reactingObj));
			PostReactionMessage(reactionID, pEvent, pParams);
		}
	}

	cReactions::React(this, reactionID, pEvent, pParams);
}
