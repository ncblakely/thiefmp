/*************************************************************
* File: MissSelect.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements change mission dialog
*************************************************************/

#include "stdafx.h"

#include "DarkWinGui.h"
#include "Client.h"

INT_PTR CALLBACK CMissionSelectDialog::StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	dbgassert(UI->m_pMissDlg);

	return UI->m_pMissDlg->DialogProc(hDlg, msg, wParam, lParam);
}

INT_PTR CMissionSelectDialog::DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
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
				m_SelectedMission = (int)m_MissionList.GetItemData(m_MissionList.GetCurSel());
				EndDialog(hDlg, IDC_OK); 
				return TRUE;
			case IDC_CANCEL:
				EndDialog(hDlg, IDC_CANCEL);
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CMissionSelectDialog::OnInitDialog(HWND hDlg)
{
	char buffer[256];
	cAnsiStr str;
	m_SelectedMission = 0;

	m_MissionList.Init(hDlg, IDD_MISSLIST);

	for (int i = 0; i < 15; i++)
	{
		int mapID = MissionNumToMap(i);
		if (mapID != -1)
		{
			_snprintf(buffer, sizeof(buffer), "title_%d", mapID);

			str.m_pString = NULL;
			str = FetchUIString("titles", buffer, "strings");

			if (str.Length())
			{
				_snprintf(buffer, sizeof(buffer), "%d - %s", i + 1, str.m_pString);
				int index = m_MissionList.AddString(buffer);
				m_MissionList.SetItemData(index, mapID);

			}

			str.Destroy();
		}
	}
}

void CMissionSelectDialog::DisplayPage()
{
	int ret = (int)DialogBoxParam(g_hInstDLL, MAKEINTRESOURCE(IDD_CHGMISS), UI->m_pConsole->GetHandle(), &CMissionSelectDialog::StaticDialogProc, NULL);
	switch (ret)
	{
	case -1:
		ErrorMessageBox(__FUNCTION__); break;
	case IDC_OK:
		Client.LoadMission(m_SelectedMission);
		SwitchToThisWindow(g_pWinAppOps->m_GameWindow, TRUE);
	}

	// cleanup
	SAFE_DELETE(UI->m_pMissDlg);
}