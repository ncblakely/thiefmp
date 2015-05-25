/*************************************************************
* File: Tools.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: General utility functions
*************************************************************/

#include "stdafx.h"
#include "Main.h"

// Application-defined callback for Logger::Print
void LogPrintCallback(void* text)
{
	ConPrintF((const char*)text);
}

void __stdcall DebugStringHook(LPCTSTR lpOutputString)
{
	ConPrintF(lpOutputString);
}


//======================================================================================
// Purpose: Translates a DirectPlay HRESULT to an error string.
//======================================================================================
const char* DPlayErrorToString(HRESULT hrError)
{
	static char msgBuf[16];

	switch (hrError)
	{
	// Error messages
	case DPNERR_ABORTED: return "DPNERR_ABORTED";
	case DPNERR_ADDRESSING: return "DPNERR_ADDRESSING";
	case DPNERR_ALREADYCONNECTED: return "DPNERR_ALREADYCONNECTED";
	case DPNERR_ALREADYCLOSING: return "DPNERR_ALREADYCLOSING";
	case DPNERR_ALREADYDISCONNECTING: return "DPNERR_ALREADYDISCONNECTING";
	case DPNERR_ALREADYINITIALIZED: return "DPNERR_ALREADYINITIALIZED";
	case DPNERR_BUFFERTOOSMALL: return "DPNERR_BUFFERTOOSMALL";
	case DPNERR_HOSTREJECTEDCONNECTION: return "DPNERR_HOSTREJECTEDCONNECTION";
	case DPNERR_DATATOOLARGE: return "DPNERR_DATATOOLARGE";
	case DPNERR_INVALIDPARAM: return "DPNERR_INVALIDPARAM"; 
	case DPNERR_INVALIDDEVICEADDRESS: return "DPNERR_INVALIDDEVICEADDRESS"; 
	case DPNERR_INVALIDAPPLICATION: return "DPNERR_INVALIDAPPLICATION"; 
	case DPNERR_INVALIDFLAGS: return "DPNERR_INVALIDFLAGS"; 
	case DPNERR_INVALIDHOSTADDRESS: return "DPNERR_INVALIDHOSTADDRESS"; 
	case DPNERR_INVALIDINSTANCE: return "DPNERR_INVALIDINSTANCE"; 
	case DPNERR_INVALIDINTERFACE: return "DPNERR_INVALIDINTERFACE"; 
	case DPNERR_INVALIDPASSWORD: return "DPNERR_INVALIDPASSWORD"; 
	case DPNERR_NOCONNECTION: return "DPNERR_NOCONNECTION"; 
	case DPNERR_NOTHOST: return "DPNERR_NOTHOST"; 
	case DPNERR_SESSIONFULL: return "DPNERR_SESSIONFULL"; 
	case E_POINTER: return "E_POINTER";

	// Success messages
	case DPNSUCCESS_PENDING:
		return "DPNSUCCESS_PENDING"; 

	default:
		sprintf(msgBuf, "0x%08X", hrError);
		return (msgBuf);
	}
}

void SetFrobHandler(int schema, enum eFrobHandler handlerType)
{
	gFrobHandlerProp->Set(schema, handlerType);
}

//======================================================================================
// Name: SummonObject
//
// Desc: Creates the specified archetype at the player's position.
//======================================================================================
int SummonObject(int objectArch)
{
	mxs_vector vec;
	int objectID; 

	objectID = g_pObjSys->BeginCreate(objectArch, 1);

	_ObjPosSetLocation(objectID, _ObjPosGet(*_gPlayerObj));

	g_pObjSys->EndCreate(objectID);

	vec.x = 1.0f;
	vec.y = 0;
	vec.z = 0;
	_PhysSetVelocity(objectID, &vec);

	return objectID;
}

int MissionNumFromMap(int mapID)
{
	for (int i = 0; i < 15; i++)
	{
		if (mission_map[i] == mapID)
			return i;
	}

	return -1;
}

int MissionNumToMap(int missionID)
{
	if (missionID >= 0 && missionID < 15)
		return mission_map[missionID];
	else
		return -1;
}

bool GetMissionName(int missionID, char* buff, int buffSize)
{
	cAnsiStr str;

	if (!missionID)
		return false;

	_snprintf(buff, buffSize, "title_%d", missionID);
	str.m_pString = NULL;
	str = FetchUIString("titles", buff, "strings");

	if (str.Length())
	{
		_snprintf(buff, buffSize, "%s", str.m_pString);
		str.Destroy();
		return true;
	}
	else
		return false;

}

cPhysModel* PhysModelFromObj(int obj)
{
	tHashSetHandle h = cHashSetBaseFns::FindIndex(&g_PhysModels->pHashSet, NULL, (tHashSetKey__*)obj);

	if (!h.x04)
		return NULL;

	cPhysModel** pModel = (cPhysModel**)h.x04;

	if (!*pModel)
		return NULL;

	return *pModel;
}

bool ObjHasScript(int obj, const char* script)
{
	sScriptProp* pScrProp;
	if (g_pScriptProp->Get(obj, (void**)&pScrProp))
	{
		for (int i = 0; i < 4; i++)
		{
			if (!_stricmp(pScrProp->scripts[i], script))
				return true;
		}
	}

	return false;
}

int ProxyFrobGetInverse(int obj)
{
	ILinkQuery* pQuery = ppFrobProxyRel->Query(0, obj);
	int objFound = 0;
	sLink link;

	if (pQuery)
	{
		while (!pQuery->Done())
		{
			if (unnamed_4AFF10(pQuery->Data(), 0, 1))
			{
				pQuery->Link(&link);
				objFound = link.sourceObj;
				break;
			}

			pQuery->Next();
		}

		pQuery->Release();
	}

	return objFound;
}

int GetNumChainedEvents(sChainedEvent* pEvent)
{
	int events = 0;

	while (pEvent)
	{
		events++;

		pEvent = pEvent->next;
	}

	return events;
}

void PrintEventChain(sChainedEvent* pEvent)
{
	int events = 0;

	while (pEvent)
	{
		events++;

		ConPrintF("(%x) Event %d: Type %d P1 %x Data %x Next %x", pEvent, events, pEvent->eventID, pEvent->P1, pEvent->data, pEvent->next);

		pEvent = pEvent->next;
	}

	ConPrintF("%d events total.", events);
}

sScreenMode GetScreenMode()
{
	sScreenMode mode;
	_ScrnModeGet(&mode);
	return mode;
}

Rect GetScreenRect()
{
	Rect r;
	sScreenMode mode = GetScreenMode();

	r.left = 0;
	r.right = mode.width;
	r.top = 0;
	r.bottom = mode.height;
	return r;
}

Rect GetCanvasRect()
{
	Rect r;

	r.left = 0;
	r.right = _grd_canvas->width;
	r.top = 0;
	r.bottom = _grd_canvas->height;
	return r;
}

cAnsiStr GetMissionName(int missionID)
{
	NString str;
	str.Format("title_%d", missionID);
	return FetchUIString("titles", str, "strings");
}

NString TimeString(int time)
{	
	NString timeStr;

	int hours = time / 3600;
	int minutes = (time % 3600) / 60;
	int seconds = (time % 3600) % 60;

	if (minutes < 0) minutes = 0;
	if (seconds < 0) seconds = 0;

	if (hours > 0)
		timeStr.Format("%u:%02u:%02u", hours, minutes, seconds);
	else
		timeStr.Format("%u:%02u", minutes, seconds);

	return timeStr;
}