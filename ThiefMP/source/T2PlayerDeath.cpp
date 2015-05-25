/*************************************************************
* File: T2PlayerDeath.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements player death packets.
*************************************************************/

#include "stdafx.h"

#include "Packets.h"

CNetMsg_PlayerDeath::CNetMsg_PlayerDeath(int playerIndex, int culprit)
{
	m_playerIndex = (BYTE)playerIndex;
	m_culprit = culprit;
}

void CNetMsg_PlayerDeath::Marshal(CDarkMarshalBuffer& buffer)
{
	buffer.CopyWord(m_Type);

	buffer.CopyByte(m_playerIndex);
	buffer.CopyGlobalFromObj(m_culprit);
}

void CNetMsg_PlayerDeath::Unmarshal(CDarkMarshalBuffer& buffer)
{
	buffer.SkipBytes(sizeof(short));

	m_playerIndex = buffer.GetByte();
	m_culprit = buffer.GetObjFromGlobal();
}