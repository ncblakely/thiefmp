/*************************************************************
* File: T2GenericReaction.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements Act/React packets.
*************************************************************/

#include "stdafx.h"

#include "Packets.h"
#include "Console.h"
#include "Imports.h"

int GetNumChainedEvents(sChainedEvent* pEvent);

CNetMsg_GenericReaction::CNetMsg_GenericReaction(short reactionID, sReactionEvent* pReactEvent, sReactionParam* pParams)
{
	m_reactionID = reactionID;
	m_reactEvent = *pReactEvent;
	m_reactParams = *pParams;

	int chainedEvents = GetNumChainedEvents(pReactEvent->event);

	if (chainedEvents > MAX_CHAINED_EVENTS)
	{
		chainedEvents = MAX_CHAINED_EVENTS;
		MessageMgr::Get()->AddLineFormat(true, "Too many chained events for %s. Message truncated.", g_pReactions->DescribeReaction(m_reactionID)->description);
	}

	m_numChainedEvents = (BYTE)chainedEvents;

	sChainedEvent* pEvent = pReactEvent->event;

	for (int i = 0; i < chainedEvents; i++)
	{
		m_chainedEvents[i] = *pEvent;

		pEvent = pEvent->next;
	}
}

void CNetMsg_GenericReaction::Marshal(CDarkMarshalBuffer& buffer)
{
	buffer.CopyWord(m_Type);

	buffer.CopyByte(m_numChainedEvents);
	buffer.CopyWord(m_reactionID);
	buffer.CopyReactionEvent(m_reactEvent);
	buffer.CopyReactionParam(m_reactParams);

	if (m_numChainedEvents > MAX_CHAINED_EVENTS)
	{
		m_numChainedEvents = MAX_CHAINED_EVENTS;
		MessageMgr::Get()->AddLineFormat(true, "Too many chained events for %s. Message truncated.", g_pReactions->DescribeReaction(m_reactionID)->description);
	}

	int marshalled = 0;
	for (int i = 0; i < m_numChainedEvents; i++)
	{
		buffer.CopyChainedEvent(m_chainedEvents[i]);

		marshalled++;
	}

	ConPrintF("Marshalled %d events.", marshalled);
}

void CNetMsg_GenericReaction::Unmarshal(CDarkMarshalBuffer& buffer)
{
	// Skip the packet type
	buffer.SkipBytes(sizeof(WORD));

	m_numChainedEvents = buffer.GetByte();
	m_reactionID = buffer.GetWord();

	buffer.GetReactionEvent(m_reactEvent);
	buffer.GetReactionParam(m_reactParams);

	if (m_numChainedEvents > MAX_CHAINED_EVENTS)
	{
		MessageMgr::Get()->AddLineFormat(true, "Reaction receive (%s): too many chained events.", g_pReactions->DescribeReaction(m_reactionID)->description);
		m_numChainedEvents = MAX_CHAINED_EVENTS;
	}

	int unmarshalled = 0;
	if (m_numChainedEvents > 0)
	{
		for (int i = 0; i < m_numChainedEvents; i++)
		{
			buffer.GetChainedEvent(m_chainedEvents[i]);

			if (i != m_numChainedEvents && i > 0)
			{
				m_chainedEvents[i - 1].next = &m_chainedEvents[i];
				//ConPrintF("Setting next ptr for event %d to %x", i - 1, &m_chainedEvents[i]);
			}

			unmarshalled++;
		}

		m_reactEvent.event = &m_chainedEvents[0];
	}
	else
		m_reactEvent.event = NULL;

	//ConPrintF("Unmarshalled %d events.", unmarshalled);

	//ConPrintF("addr start event: %x, start events %x", m_reactEvent.event, &m_chainedEvents[0]);
}