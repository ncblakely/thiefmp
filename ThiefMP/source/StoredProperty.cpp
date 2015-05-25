/*************************************************************
* File: StoredProperty.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements VTable hooks of stored property methods.
*************************************************************/

#include "stdafx.h"

#include "Main.h"

#ifdef _DEBUG
std::map<char*, int> PropSendsMap;
#endif

namespace HookStoredProperty
{

// value is used for various things, such as quantity for stack count
int __stdcall SendPropertyMsg(sNetMsg_Generic* msg, int objectID, int value, unsigned long type)
{
	//if (g_pObjNet->ObjLocalOnly(objectID))
	//{
	//	return 1;
	//}

#ifdef _DEBUG
	if (!PropSendsMap[msg->desc->msgDesc])
		PropSendsMap[msg->desc->msgDesc] = 1;
	else
		PropSendsMap[msg->desc->msgDesc]++;
#endif

	if (Debug.IsFlagSet(DEBUG_PROPSENDS))
		ConPrintF("Sending property msg for %s: Name %s, type 0x%x, flags %x", _ObjEditName(objectID), msg->desc->msgDesc, type, value);
	return cStoredProperty_SendPropertyMsg(msg, objectID, value, type);
}

void __stdcall ReceivePropertyMsg(struct sNetMsg_Generic const * msg, unsigned long P1, DWORD P2, int P3)
{
	if (Debug.IsFlagSet(DEBUG_RECEIVES))
		ConPrintF("Received property msg %s.", msg->desc->msgDesc);

	cStoredProperty_ReceivePropertyMsg(msg, P1, P2, P3);
}

} // HookStoredProperty