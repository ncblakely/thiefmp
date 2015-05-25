#pragma once

#include "Defines.h"

#ifdef _DEFINE_SCRIPT_OFFSETS
	#define DEF_SCRIPTPTR(type, calltype, p, args, a) type (calltype* p)args = (type (calltype*) args)a;
#else
	#define DEF_SCRIPTPTR(type, calltype, p, args, a) extern type (calltype* p)args;

#endif

DEF_SCRIPTPTR(void, __stdcall, ScriptReceiveMessage, (IRootScript* pThis, sScrMsg* pMsg, cMultiParm*, enum eScrTraceAction), NULL)