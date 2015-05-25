/*************************************************************
* File: PlayerSettings.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements player settings dialog
*************************************************************/

#include "stdafx.h"

#include "DarkWinGui.h"
#include "Client.h"

INT_PTR CALLBACK CPlayerSettingsDialog::StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	dbgassert(UI->m_pPlayerDlg);

	return UI->m_pPlayerDlg->DialogProc(hDlg, msg, wParam, lParam);
}

INT_PTR CPlayerSettingsDialog::DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
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
			case IDOK:
				SaveSettings();
				g_pNetMan->SetPlayerName(Cfg.GetString("PlayerName"));
				EndDialog(hDlg, IDC_OK); 
				return TRUE;
			case IDCANCEL:
				EndDialog(hDlg, IDC_CANCEL);
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CPlayerSettingsDialog::OnInitDialog(HWND hDlg)
{
	m_hDlg = hDlg;

	InitButtons();
	InitControlStates();
}

void CPlayerSettingsDialog::DisplayPage()
{
	int ret = (int)DialogBoxParam(g_hInstDLL, MAKEINTRESOURCE(IDD_PLAYERDLG), UI->m_pConsole->GetHandle(), &CPlayerSettingsDialog::StaticDialogProc, NULL);
	switch (ret)
	{
	case -1:
		ErrorMessageBox(__FUNCTION__); break;
	//case IDC_OK:
	}

	// cleanup
	SAFE_DELETE(UI->m_pPlayerDlg);
}

void CPlayerSettingsDialog::InitButtons()
{
	m_cmbModels.Init(m_hDlg, IDC_COMBOMODEL);
	m_edtName.Init(m_hDlg, IDC_EDITNAME);
	m_edtName.LimitText(kMaxPlayerName - 1);

	m_cmbModels.AddString("Garrett", MID_Garrett);
	m_cmbModels.AddString("Thief", MID_MaleThief);
	m_cmbModels.AddString("Keeper", MID_Keeper);
	m_cmbModels.AddString("Servant", MID_MaleServant);
	m_cmbModels.AddString("Basso", MID_Basso);
}

void CPlayerSettingsDialog::InitControlStates()
{
	m_edtName.SetText(Cfg.GetString("PlayerName"));
	m_edtName.LimitText(kMaxPlayerName - 1);

	m_cmbModels.SetCurSel(0);
	int model = Cfg.GetInt("Model");

	for (int i = 0; i < kTotalModels; i++)
	{
		if (m_cmbModels.GetItemData(i) == model)
		{
			m_cmbModels.SetCurSel(i);
		}
	}
}

void CPlayerSettingsDialog::SaveSettings()
{
	char buffer[256];

	IniFile ini(".\\ThiefMP.ini");

	m_edtName.GetText(buffer, 256);
	ini.WriteString("Player", "PlayerName", buffer);

	sprintf(buffer, "%d", m_cmbModels.GetItemData(m_cmbModels.GetCurSel()));
	ini.WriteString("Player", "Model", buffer);
	

	Config::Reload();
}