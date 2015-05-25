#pragma once

#include "UI.h"
#include "Engine\inc\EngError.h"

class CThiefConsole;

class InterfaceManager : public IInterfaceManager
{
public:
	InterfaceManager();
	virtual ~InterfaceManager();

	virtual void CreateChildWindow(int windowID);
	virtual void Startup();
	virtual void Shutdown();
	virtual void Update();

	CHostDialog* m_pHostDlg;
	CConnectDialog* m_pConnectDlg;
	CMissionSelectDialog* m_pMissDlg;
	CPlayerSettingsDialog* m_pPlayerDlg;
	CThiefConsole* m_pConsole;
	CPasswordDialog* m_pPasswordDlg;
};

class CThiefConsole : public CConsoleBase
{
public:
	CThiefConsole(HINSTANCE hInst, int dlgRsrc, int outputEditRsrc, int inputEditRsrc, DLGPROC proc) : CConsoleBase(hInst, dlgRsrc, outputEditRsrc, inputEditRsrc, proc) { UI->m_pConsole = this; }

	INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
};