#include "stdafx.h"

#include "Debug.h"
#include "DarkPeer.h"

CDebugManager Debug;

CDebugManager::CDebugManager()
{
	m_DebugFlags = 0;
	m_LastExceptionFilter = NULL;
}

void CDebugManager::SetExceptionFilter()
{
	SetExceptionFilter(&ExceptFilter);
}

void CDebugManager::SetExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER pExceptionFilter)
{
	m_LastExceptionFilter = SetUnhandledExceptionFilter(pExceptionFilter);
}

void CDebugManager::RestoreExceptionFilter()
{
	SetUnhandledExceptionFilter(m_LastExceptionFilter);

	m_LastExceptionFilter = NULL;
}

void CDebugManager::LoadFlags()
{
	IniFile iniread(".\\ThiefMP.ini");

	m_DebugFlags = iniread.GetInt("Debug", "DebugFlags", 0, false);
}

void CDebugManager::SaveFlags()
{
	IniFile ini(".\\ThiefMP.ini");

	ini.WriteInt("Debug", "DebugFlags", m_DebugFlags);
}

void CDebugManager::ToggleFlag(unsigned int flag)
{
	m_DebugFlags ^= flag;
}

void CDebugManager::ClearFlag(unsigned int flag)
{
	m_DebugFlags &= ~flag;
}

void CDebugManager::ClearAllFlags()
{
	m_DebugFlags = 0;
}

long __stdcall CDebugManager::ExceptFilter(PEXCEPTION_POINTERS data)
{
	if (g_pDarkNet)
		g_pDarkNet->CrashCleanup();

	return RecordExceptionInfo(data);
}

void DebugInit()
{
	Debug.LoadFlags();
}