/*************************************************************
* File: DarkSessionManager.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Session manager for Thief.
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "DarkPeer.h"

CDarkSessionManager::CDarkSessionManager(IDirectPlay8Peer* pDP)
{
	m_pDP = pDP;
	m_pTempAppDesc = NULL;
	m_pQuestData = _AppGetAggregated(IID_IQuestData);

	dbgassert(m_pDP);
}

CDarkSessionManager::~CDarkSessionManager()
{
	SAFE_DELETE_ARRAY(m_pTempAppDesc);
}

void CDarkSessionManager::SetApplicationDesc(const DPN_APPLICATION_DESC* pAppDesc)
{
	m_pDP->SetApplicationDesc(pAppDesc, NULL);
	m_serverName = pAppDesc->pwszSessionName;
}

const DPN_APPLICATION_DESC* CDarkSessionManager::GetApplicationDesc()
{
	DPGetApplicationDesc();

	return m_pTempAppDesc;
}

void CDarkSessionManager::DPGetApplicationDesc()
{
	HRESULT hRes;
	DWORD size = 0;

	hRes = m_pDP->GetApplicationDesc(NULL, &size, NULL);
	if (hRes != DPNERR_BUFFERTOOSMALL)
	{
		dbgassert(false);

		return;
	}

	SAFE_DELETE_ARRAY(m_pTempAppDesc);

	m_pTempAppDesc = (DPN_APPLICATION_DESC*) new BYTE[size];
	ZeroMemory(m_pTempAppDesc, sizeof(DPN_APPLICATION_DESC));
	m_pTempAppDesc->dwSize = sizeof(DPN_APPLICATION_DESC);

	hRes = m_pDP->GetApplicationDesc(m_pTempAppDesc, &size, NULL);
	if (hRes != DPN_OK)
	{
		dbgassert(false);

		return;
	}
}

void CDarkSessionManager::SetSessionInfo(const DPN_APPLICATION_DESC* desc)
{
	SessionInfo* info = (SessionInfo*)desc->pvApplicationReservedData;
	dbgassert(info);

	if (!info)
		return;

	m_sessionDesc = *info;
	m_serverName = desc->pwszSessionName;
}

const SessionInfo& CDarkSessionManager::GetSessionInfo() const
{
	return m_sessionDesc;
}

const char* CDarkSessionManager::GetServerName() const
{
	return m_serverName;
}

void CDarkSessionManager::UpdateSessionInfo()
{
	DPGetApplicationDesc();

	m_sessionDesc.gameStarted = (BYTE)g_pNetMan->m_bSynchDone;
	m_sessionDesc.difficulty = m_pQuestData->Get("Difficulty");
	m_sessionDesc.nextMission = _GetNextMission();
	m_sessionDesc.build = TMP_BUILD;
	m_sessionDesc.respawnEnabled = Cfg.GetBool("RespawnEnabled");
	m_sessionDesc.respawnDelay = Cfg.GetInt("RespawnDelay");
	m_sessionDesc.shareLoot = Cfg.GetBool("ShareLoot");
	m_sessionDesc.playerTransparency = Cfg.GetBool("PlayerTransparency");
	m_sessionDesc.startingLoot = Cfg.GetInt("StartingLoot");
	m_sessionDesc.noLoadoutScreen = Cfg.GetBool("NoLoadout");
	m_sessionDesc.teamDamage = Cfg.GetBool("TeamDamage");
	m_sessionDesc.cheatsEnabled = Cfg.GetBool("CheatsEnabled");
	m_sessionDesc.increaseDifficulty = Cfg.GetBool("IncreaseDifficulty");
	m_sessionDesc.saveLimit = Cfg.GetInt("SaveLimit");
	m_sessionDesc.timeLimit = Cfg.GetInt("TimeLimit");

	m_pTempAppDesc->pvApplicationReservedData = (void*)&m_sessionDesc;
	m_pTempAppDesc->dwApplicationReservedDataSize = sizeof(m_sessionDesc);

	SetApplicationDesc(m_pTempAppDesc);
}