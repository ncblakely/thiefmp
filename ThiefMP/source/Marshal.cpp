/*************************************************************
* File: Marshal.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "Marshal.h"

int CMarshalled::Send(DPNID idTo, DWORD flags)
{
	CDarkMarshalBuffer buff;

	buff.UseDefaultBuffer();
	Marshal(buff);

	int bytes = buff.GetBytesWritten();

	if (g_pDarkNet)
		g_pDarkNet->Send(idTo, buff.GetStartBuffer(), bytes, NULL, flags);

	return bytes;
}

void CMarshalled::Rebuild(BYTE* pBuffer)
{
	CDarkMarshalBuffer buff;

	buff.SetBuffer(pBuffer, NULL);

	Unmarshal(buff);
}

void CDarkMarshalBuffer::CopyGlobalFromObj(int obj)
{
	sGlobalObjID glob = g_pNetMan->ToGlobalObjID(obj);

	CopyVoid((void*)&glob, sizeof(sGlobalObjID));
}

void CDarkMarshalBuffer::CopyReactionEvent(const sReactionEvent& reactEvent)
{
	CopyInt(reactEvent.P2);
	CopyGlobalFromObj(reactEvent.reactingObj);

	//DbgPrint("Marshalling reaction event: %d %d.", reactEvent.P2, reactEvent.reactingObj);
}

void CDarkMarshalBuffer::GetReactionEvent(sReactionEvent& reactEvent)
{
	// Null the chained event pointer. This will be reconstructed later.
	reactEvent.event = NULL;

	reactEvent.P2 = GetInt();
	reactEvent.reactingObj = GetObjFromGlobal();

	//DbgPrint("Unmarshalling reaction event: %d %d.", reactEvent.P2, reactEvent.reactingObj);
}

void CDarkMarshalBuffer::CopyMultiParm(const sMultiParm& parm)
{
	CopyByte(parm._type);
	CopyInt(parm._int);
}

void CDarkMarshalBuffer::CopyChainedEvent(const sChainedEvent& event)
{
	if (event.eventID < UCHAR_MAX)
		CopyByte((BYTE)event.eventID);
	else
	{
#ifndef _RELEASE
		assert(false && "event ID too large");
#endif
		CopyByte(0);
	}
	CopyInt(event.P1);
	CopyInt(event.data);
}

void CDarkMarshalBuffer::GetChainedEvent(sChainedEvent& event)
{
	event.eventID = GetByte();
	event.P1 = GetInt();
	event.next = NULL;
	event.data = GetInt();
}

void CDarkMarshalBuffer::CopyReactionParam(const sReactionParam& reactParams)
{
	// This only copies the first 12 bytes of sReactionParam. Expand as necessary

	CopyGlobalFromObj(reactParams.reactingObj);
	CopyGlobalFromObj(reactParams.instigatorObj);
	CopyInt(reactParams.flags);

	DbgPrint("Marshalling reaction param: %d %d %d.", reactParams.reactingObj, reactParams.instigatorObj, reactParams.flags);
}

void CDarkMarshalBuffer::GetReactionParam(sReactionParam& reactParams)
{
	reactParams.reactingObj = GetObjFromGlobal();
	reactParams.instigatorObj = GetObjFromGlobal();
	reactParams.flags = GetInt();

	DbgPrint("Unmarshalling reaction param: %d %d %d.", reactParams.reactingObj, reactParams.instigatorObj, reactParams.flags);
}

int CDarkMarshalBuffer::GetObjFromGlobal()
{
	sGlobalObjID id;

	GetVoid((void*)&id, sizeof(sGlobalObjID));

	return g_pNetMan->FromGlobalObjID(&id);
}

void CDarkMarshalBuffer::GetMultiParm(sMultiParm& parm)
{
	parm._type = (eMultiParmType)(int)GetByte();
	parm._int = GetInt();	// Obviously, it doesn't matter which union member we retrieve here
}