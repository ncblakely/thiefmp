/*************************************************************
* File: T2ScriptMsg.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements script packets.
*************************************************************/

#include "stdafx.h"

#include "Packets.h"

CNetMsg_ScriptMsg::CNetMsg_ScriptMsg(int source, int dest, const char* message, sMultiParm* pParms)
{
	m_source = source;
	m_dest = dest;
	safe_strcpy(m_message, message, sizeof(m_message) - 1);
	m_flags = 0;

	if (pParms)
	{
		m_flags |= SCRIPTMSG_HAS_PARMS;
		m_parms = *pParms;
	}
}

void CNetMsg_ScriptMsg::Marshal(CDarkMarshalBuffer& buffer)
{
	buffer.CopyWord(m_Type);

	buffer.CopyGlobalFromObj(m_source);
	buffer.CopyGlobalFromObj(m_dest);
	buffer.CopyString(m_message, 32);
	buffer.CopyByte(m_flags);
	buffer.CopyMultiParm(m_parms);
}

void CNetMsg_ScriptMsg::Unmarshal(CDarkMarshalBuffer& buffer)
{
	buffer.SkipBytes(sizeof(short));

	m_source = buffer.GetObjFromGlobal();
	m_dest = buffer.GetObjFromGlobal();
	buffer.GetString(m_message, 32);
	m_flags = buffer.GetByte();

	if (m_flags & SCRIPTMSG_HAS_PARMS)
		buffer.GetMultiParm(m_parms);
	else
		ZeroMemory(&m_parms, sizeof(sMultiParm));
}

sScrMsg* CNetMsg_ScriptMsg::ScriptMessageCreate()
{
	if (!_stricmp(m_message, "NetFrob"))
	{
		return CreateFrobMsg();
	}

	return CreateScrMsg();
}

sScrMsg* CNetMsg_ScriptMsg::CreateFrobMsg()
{
	sFrobMsg* pFrobMsg = new sFrobMsg();

	pFrobMsg->dest = m_dest;
	pFrobMsg->source = m_source;
	pFrobMsg->msgName = m_message;
	pFrobMsg->flags = 4;

	pFrobMsg->frobber = m_source;

	return pFrobMsg;
}

sScrMsg* CNetMsg_ScriptMsg::CreateScrMsg()
{
	sScrMsg* pScrMsg = new sScrMsg();

	pScrMsg->dest = m_dest;
	pScrMsg->source = m_source;
	pScrMsg->msgName = m_message;
	pScrMsg->flags = 4;

	return pScrMsg;
}