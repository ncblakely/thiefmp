/*************************************************************
* File: ConnectDialog.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements connect to server dialog
*************************************************************/

#include "stdafx.h"

#include "DarkWinGui.h"

CConnectDialog::CConnectDialog()
{
	m_selectedPort = 0;
	m_bPasswordEntered = false;

	ZeroMemory(m_selectedIP, sizeof(m_selectedIP));
	ZeroMemory(m_passwordBuff, sizeof(m_passwordBuff));
}

INT_PTR CALLBACK CConnectDialog::StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	dbgassert(UI->m_pConnectDlg);

	return UI->m_pConnectDlg->DialogProc(hDlg, msg, wParam, lParam);
}

INT_PTR CALLBACK CConnectDialog::DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
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
			case IDC_CONNECT:
				if (wmEvent == BN_CLICKED)
				{
					SaveSettings();

					m_edtIPAddress.GetText(m_selectedIP, 32);
					m_selectedPort = m_edtPort.GetInt();

					g_pNetMan->SetPlayerName(Cfg.GetString("PlayerName"));

					if (m_edtPassword.GetTextLength() > 0)
					{
						GetWindowTextW(m_edtPassword.GetHandle(), m_passwordBuff, sizeof(m_passwordBuff));
						m_bPasswordEntered = true;
					}
					EndDialog(hDlg, IDC_JOIN); return TRUE;
				}
			case IDC_CANCEL:
				if (wmEvent == BN_CLICKED)
					EndDialog(hDlg, IDC_CANCEL); return TRUE;
			case IDC_SERVERIP_EDIT:
				if (wmEvent == EN_UPDATE)
					EnableWindow(m_hConnect, GetWindowTextLength(m_edtIPAddress.GetHandle()) ? TRUE : FALSE);
				return TRUE;
			}
			break;
		}
	default:
		break;
	}
	return FALSE;
}

void CConnectDialog::DisplayPage()
{
	int ret = (int)DialogBoxParam(g_hInstDLL, MAKEINTRESOURCE(IDD_SERVERCONNECT), UI->m_pConsole->GetHandle(), &CConnectDialog::StaticDialogProc, NULL);
	switch (ret)
	{
	case -1:
		return ConPrintF("Failed to display connect dialog: %s", LastErrorToString().c_str());
	case IDC_JOIN:
		if (!g_pNetMan->IsNetworkGame())
			OnConnect();
		else if (PromptDisconnect() == IDYES)
		{
			g_pNetMan->Leave();
			OnConnect();
		}
	}

	// cleanup
	SAFE_DELETE(UI->m_pConnectDlg);
}

void CConnectDialog::OnConnect()
{
	ConPrintF("Connecting to %s...", m_selectedIP);

	SwitchToThisWindow(g_pWinAppOps->m_GameWindow, TRUE);

	if (m_bPasswordEntered)
		g_ConnectAttempt.Start(m_selectedPort, m_selectedIP, m_passwordBuff);
	else
		g_ConnectAttempt.Start(m_selectedPort, m_selectedIP, NULL);

	//g_ConnectAttempt.m_active = true;
	//g_pNetMan->Join(NULL, NULL, m_selectedIP);
}

void CConnectDialog::OnInitDialog(HWND hDlg)
{
	m_hDlg = hDlg;
	m_hConnect = GetDlgItem(m_hDlg, IDC_JOIN);

	m_edtIPAddress.Init(m_hDlg, IDC_SERVERIP_EDIT);
	m_edtPort.Init(m_hDlg, IDC_PORT);
	m_edtPassword.Init(m_hDlg, IDC_PASSWORD);

	m_edtIPAddress.SetText(Cfg.GetString("IPAddress"));

	SetDlgItemInt(m_hDlg, IDC_PORT, DEFAULT_PORT, TRUE);
}

void CConnectDialog::SaveSettings()
{
	IniFile ini(".\\ThiefMP.ini");
	char buffer[256];

	m_edtIPAddress.GetText(buffer, 256);
	ini.WriteString("History", "IPAddress", buffer);

	Config::Reload();

	// get the selected port, but don't save it to the config file
	m_edtPort.GetText(buffer, 256);

	Cfg.SetInt("ServerPort", atoi(buffer));
}