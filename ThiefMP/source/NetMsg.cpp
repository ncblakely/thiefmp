/*************************************************************
* File: NetMsg.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements hooks of cNetMsg member functions.
*************************************************************/

#include "stdafx.h"

#include "Imports.h"
#include "Debug.h"

#include "Callbacks.h"

namespace HookNetMsg
{

void __fastcall HandleMsg(cNetMsg* pNetMsg, int, struct sNetMsg_Generic const * msg, unsigned long P1, int P2)
{
	if (Debug.IsFlagSet(DEBUG_RECEIVES))
		//if (pNetMsg->m_msgDesc->handlerFunction == HookHandleEndCreate || pNetMsg->m_msgDesc->handlerFunction == HookHandleBeginCreate)
			cNetMsgFns::ReceiveSpew(pNetMsg, NULL, P2);

	cNetMsgFns::HandleMsg(pNetMsg, NULL, msg, P1, P2);
}

int __fastcall MarshalArguments(cNetMsg* pNetMsg, int, char * msgData, int* P1, unsigned int* P2)
{
	int ret = cNetMsgFns::MarshalArguments(pNetMsg, NULL, msgData, P1, P2);

	if (ret && Debug.IsFlagSet(DEBUG_SENDS))
		//if (pNetMsg->m_msgDesc->handlerFunction == HookHandleEndCreate || pNetMsg->m_msgDesc->handlerFunction == HookHandleBeginCreate)
			cNetMsgFns::SendSpew(pNetMsg, NULL, msgData);

	return ret;
}
} // end HookNetMsg namespace