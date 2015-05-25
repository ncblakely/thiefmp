/*************************************************************
* File: HostDialog.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements host server dialog
*************************************************************/

#include "stdafx.h"

#include "DarkWinGui.h"
#include "Client.h"
#include "DarkOverlay.h"

CHostDialog::CHostDialog()
{
	 m_forceMission = 0;
	 ZeroMemory(m_passwordBuff, sizeof(m_passwordBuff));
}

INT_PTR CALLBACK CHostDialog::StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	dbgassert(UI->m_pHostDlg);

	return UI->m_pHostDlg->DialogProc(hDlg, msg, wParam, lParam);
}

INT_PTR CALLBACK CHostDialog::DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmID = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam);

	switch (msg)
	{
	case WM_INITDIALOG:
			OnInitDialog(hDlg); return TRUE;
	case WM_COMMAND:
		{
			switch (wmID)
			{
			case IDC_OK:
				if (wmEvent == BN_CLICKED)
				{
					SaveSettings();
					m_forceMission = (int)m_cmbMission.GetItemData(m_cmbMission.GetCurSel());

					g_pNetMan->SetPlayerName(Cfg.GetString("PlayerName"));

					if (m_edtPassword.GetTextLength() > 0)
					{
						m_edtPassword.GetText(m_passwordBuff, sizeof(m_passwordBuff));
						Cfg.SetString("Password", m_passwordBuff);
					}
					EndDialog(hDlg, IDC_OK); 
					return TRUE;
				}
				break;
			case IDC_CANCEL:
				EndDialog(hDlg, IDC_CANCEL); 
				return TRUE;
			case IDC_OPT_RESPAWN:
				{
					if (wmEvent == BN_CLICKED)
					{	
						m_edtSpawnTime.Enable(m_btnRespawn.IsChecked());
						return TRUE;
					}
					break;
				}
			case IDC_TIMELIMITCHECK:
				{
					if (wmEvent == BN_CLICKED)
					{
						m_edtTimeLimit.Enable(m_btnTimeLimit.IsChecked());
						return TRUE;
					}
					break;
				}
			case IDC_SAVELIMITCHECK:
				{
					if (wmEvent == BN_CLICKED)
					{
						m_edtSaveLimit.Enable(m_btnSaveLimit.IsChecked());
						return TRUE;
					}
					break;
				}
			}
		}
	default:
		break;
	}

	return FALSE;
}

void CHostDialog::DisplayPage()
{
	int ret = (int)DialogBoxParam(g_hInstDLL, MAKEINTRESOURCE(IDD_HOST), UI->m_pConsole->GetHandle(), &CHostDialog::StaticDialogProc, NULL);
	switch (ret)
	{
	case -1:
		ErrorMessageBox(__FUNCTION__); break;
	case IDC_OK:
		if (!g_pNetMan->IsNetworkGame())
			OnHost();
		else if (g_pNetMan->AmDefaultHost() && g_pNetMan->NumPlayers() <= 1)
		{
			g_pNetMan->Leave();
			OnHost();
		}
		else if (PromptDisconnect() == IDYES)
		{
			g_pNetMan->Leave();
			OnHost();
		}
		break;
	}

	// cleanup
	SAFE_DELETE(UI->m_pHostDlg);
}

void CHostDialog::OnHost()
{
	_config_set_int("cash_bonus", Cfg.GetInt("StartingLoot"));

	_SetNextMission(m_forceMission);
	

	SwitchToThisWindow(g_pWinAppOps->m_GameWindow, TRUE);
	g_pNetMan->Host(NULL, NULL);

#if 1
	Client.LoadMission(m_forceMission);
#else
	StagingDisplay();
#endif
}

void CHostDialog::OnInitDialog(HWND hDlg)
{
	m_hDlg = hDlg;

	InitButtons();
	InitControlStates(); // initialize controls for settings adjustable through ThiefMP.ini
}

void CHostDialog::InitButtons()
{
	int mapID;
	char buffer[256];

	m_btnShareLoot.Init(m_hDlg, IDC_OPT_SHARELOOT);
	m_btnRespawn.Init(m_hDlg, IDC_OPT_RESPAWN);
	m_btnDifficulty.Init(m_hDlg, IDC_OPT_DIFFICULTY);
	m_btnNoLoadout.Init(m_hDlg, IDC_NOLOADOUT);
	m_btnTransparency.Init(m_hDlg, IDC_OPT_TRANSPARENCY);
	m_cmbGameType.Init(m_hDlg, IDC_OPT_GAMETYPE);
	m_cmbPlayers.Init(m_hDlg, IDC_OPT_MAXPLAYERS);
	m_cmbMission.Init(m_hDlg, IDC_OPT_MISS);
	m_btnTimeLimit.Init(m_hDlg, IDC_TIMELIMITCHECK);
	m_btnSaveLimit.Init(m_hDlg, IDC_SAVELIMITCHECK),
	m_btnTeamDmg.Init(m_hDlg, IDC_TEAMDAMAGECHECK);
	m_btnListServer.Init(m_hDlg, IDC_LISTSERVCHECK);
	m_btnEnableCheats.Init(m_hDlg, IDC_ENABLECHEATS);

	m_edtPort.Init(m_hDlg, IDC_OPT_PORT);
	m_edtSpawnTime.Init(m_hDlg, IDC_RESPAWNTIME);
	m_edtServerName.Init(m_hDlg, IDC_SERVERNAME);
	m_edtStartingLoot.Init(m_hDlg, IDC_STARTLOOT);
	m_edtTimeLimit.Init(m_hDlg, IDC_TIMELIMIT);
	m_edtSaveLimit.Init(m_hDlg, IDC_SAVELIMIT);
	m_edtPassword.Init(m_hDlg, IDC_PASSWORD);

	m_edtServerName.LimitText(kMaxGameName - 1);
	m_edtStartingLoot.LimitText(6);
	m_edtTimeLimit.LimitText(3);

	for (int i = 2; i <= kMaxPlayers; i++)
	{
		sprintf(buffer, "%d", i);
		m_cmbPlayers.AddString(buffer);
	}


	m_cmbGameType.AddString("Cooperative");

	for (int i = 0; i < MAX_MISSION_MAPS; i++)
	{
		if (i < 17)
			mapID = MissionNumToMap(i);
		else
			mapID = i;

		DWORD missFlags = GetMissionFlags(mapID);

		if (missFlags & MISSFLAG_SKIP || mapID == -1)
			continue;

		_snprintf(buffer, sizeof(buffer), "title_%d", mapID);

		cAnsiStr& str = FetchUIString("titles", buffer, "strings");

		if (str.Length())
		{
			_snprintf(buffer, sizeof(buffer), "%d - %s", i + 1, str.m_pString);
			int index = m_cmbMission.AddString(buffer);
			m_cmbMission.SetItemData(index, mapID);
		}

		str.Destroy();
	}

	int save = m_cmbMission.AddString("Saved Game...");
	int mission = Cfg.GetInt("Mission");

	if (mission <= save)
		m_cmbMission.SetCurSel(mission);
	else
		m_cmbMission.SetCurSel(0);
}

DWORD CHostDialog::GetMissionFlags(int mission)
{
	DWORD flags;
	char buff[64];

	sprintf(buff, "miss_%d", mission);

	IPtr<IGameStrings> pGameStrings = _AppGetAggregated(IID_IGameStrings);
	cAnsiStr strMissFlags = pGameStrings->FetchString("missflag", buff, "", "strings");

	IPtr<IStructDescTools> pDescTools = _AppGetAggregated(IID_IStructDescTools);
	pDescTools->ParseField(pFD_MissionFlags, strMissFlags.m_pString, (void*)&flags);

	strMissFlags.Destroy();

	return flags;
}

void CHostDialog::InitControlStates()
{
	m_edtServerName.SetText(Cfg.GetString("ServerName"));
	m_edtPassword.SetText(Cfg.GetString("Password"));

	m_btnShareLoot.SetCheck(Cfg.GetInt("ShareLoot") ? true : false);
	m_btnRespawn.SetCheck(Cfg.GetInt("RespawnEnabled") ? true : false);
	m_btnDifficulty.SetCheck(Cfg.GetBool("IncreaseDifficulty") ? true : false);
	m_btnTransparency.SetCheck(Cfg.GetBool("PlayerTransparency") ? true : false);
	m_btnTimeLimit.SetCheck(Cfg.GetInt("TimeLimit") > 0 ? true : false);
	m_btnSaveLimit.SetCheck(Cfg.GetInt("SaveLimit") > 0 ? true : false);
	m_btnNoLoadout.SetCheck(Cfg.GetBool("NoLoadout") ? true : false);
	m_btnTeamDmg.SetCheck(Cfg.GetBool("TeamDamage") ? true : false);
	m_btnEnableCheats.SetCheck(Cfg.GetInt("CheatsEnabled") ? true : false);
	m_btnListServer.SetCheck(Cfg.GetInt("ListOnGlobal") ? true : false);

	SetDlgItemInt(m_hDlg, IDC_RESPAWNTIME, Cfg.GetInt("RespawnDelay"), TRUE);
	SetDlgItemInt(m_hDlg, IDC_OPT_PORT, Cfg.GetInt("ServerPort"), TRUE);
	SetDlgItemInt(m_hDlg, IDC_STARTLOOT, Cfg.GetInt("StartingLoot"), TRUE);
	SetDlgItemInt(m_hDlg, IDC_TIMELIMIT, Cfg.GetInt("TimeLimit"), TRUE);
	SetDlgItemInt(m_hDlg, IDC_SAVELIMIT, Cfg.GetInt("SaveLimit"), TRUE);

	if (!Cfg.GetInt("RespawnEnabled"))
		m_edtSpawnTime.Enable(false);
	if (Cfg.GetInt("TimeLimit") <= 0)
		m_edtTimeLimit.Enable(false);
	if (Cfg.GetInt("SaveLimit") <= 0)
		m_edtSaveLimit.Enable(false);

	m_cmbPlayers.SetCurSel(Cfg.GetInt("MaximumPlayers") - 2);
	m_cmbGameType.SetCurSel(0);
}

void CHostDialog::SaveSettings()
{
	char buffer[256];

	IniFile ini(".\\ThiefMP.ini");

	ini.WriteString("Server", "ShareLoot", m_btnShareLoot.IsChecked()  ? "True" : "False");
	ini.WriteString("Server", "RespawnEnabled", m_btnRespawn.IsChecked() ? "True" : "False");
	ini.WriteString("Server", "IncreaseDifficulty", m_btnDifficulty.IsChecked() ? "True" : "False");
	ini.WriteString("Server", "PlayerTransparency", m_btnTransparency.IsChecked() ? "True" : "False");
	ini.WriteString("Server", "NoLoadout", m_btnNoLoadout.IsChecked() ? "True" : "False");
	ini.WriteString("Server", "TeamDamage", m_btnTeamDmg.IsChecked() ? "True": "False");
	ini.WriteString("Server", "ListOnGlobal", m_btnListServer.IsChecked() ? "True": "False");
	ini.WriteString("Server", "CheatsEnabled", m_btnEnableCheats.IsChecked() ? "True": "False");

	m_cmbPlayers.GetText(buffer, 256);
	ini.WriteString("Server", "MaximumPlayers", buffer);

	m_edtPort.GetText(buffer, 256);
	ini.WriteString("Server", "ServerPort", buffer);

	m_edtSpawnTime.GetText(buffer, 256);
	int respawnTime = GetDlgItemInt(m_hDlg, IDC_RESPAWNTIME, NULL, TRUE);
	if (respawnTime < 5)
		ini.WriteString("Server", "RespawnDelay", "5");
	else
		ini.WriteString("Server", "RespawnDelay", buffer);

	m_edtStartingLoot.GetText(buffer, 256);
	ini.WriteString("Server", "StartingLoot", buffer);

	if (m_btnTimeLimit.GetCheck())
	{
		m_edtTimeLimit.GetText(buffer, 256);
		ini.WriteString("Mission", "TimeLimit", buffer);
	}
	else
		ini.WriteString("Mission", "TimeLimit", "0");

	m_edtSaveLimit.GetText(buffer, 256);
	ini.WriteString("Mission", "SaveLimit", buffer);

	m_edtServerName.GetText(buffer, 256);
	ini.WriteString("Server", "ServerName", buffer);

	m_edtPassword.GetText(buffer, 256);
	ini.WriteString("Server", "Password", buffer);

	// Update history
	sprintf(buffer, "%d", m_cmbMission.GetCurSel());
	ini.WriteString("History", "Mission", buffer);

	// Update settings in memory from the ones we just wrote to the config file
	Config::Reload();
}