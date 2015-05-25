/*************************************************************
* File: PasswordDialog.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements server password dialog
*************************************************************/

#include "stdafx.h"

#include "DarkWinGui.h"
#include "GlobalServer.h"

CPasswordDialog::CPasswordDialog()
{
	m_pServer = NULL;
	ZeroMemory(m_passwordBuff, sizeof(m_passwordBuff));
}

INT_PTR CALLBACK CPasswordDialog::StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	dbgassert(UI->m_pPasswordDlg);

	return UI->m_pPasswordDlg->DialogProc(hDlg, msg, wParam, lParam);
}

INT_PTR CPasswordDialog::DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmID = LOWORD(wParam); 
	int wmEvent = HIWORD(wParam);

	switch (msg)
	{
	case WM_INITDIALOG:
		OnInitDialog(hDlg, lParam); return TRUE;
	case WM_COMMAND:
		{
			switch (wmID)
			{
			case IDOK:
				GetWindowTextW(m_edtPassword.GetHandle(), m_passwordBuff, sizeof(m_passwordBuff));
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

void CPasswordDialog::OnInitDialog(HWND hDlg, LPARAM lParam)
{
	m_hDlg = hDlg;
	
	if (!lParam)
		EndDialog(hDlg, -1);

	m_edtPassword.Init(hDlg, IDC_PASSWORD);

	m_pServer = (CServerListEntry*)lParam;
}

void CPasswordDialog::DisplayPage()
{
	int ret = (int)DialogBoxParam(g_hInstDLL, MAKEINTRESOURCE(IDD_PASSWORD), UI->m_pConsole->GetHandle(), &StaticDialogProc, (LPARAM)g_pBrowser->GetSelectedServer());
	if (ret == IDC_OK)
	{
		// Got the password, start the connect attempt
		SwitchToThisWindow(g_pWinAppOps->m_GameWindow, TRUE);
		g_ConnectAttempt.Start(m_pServer->m_port, m_pServer->m_serverIP, m_passwordBuff);
	}

	// cleanup
	SAFE_DELETE(UI->m_pPasswordDlg);
}