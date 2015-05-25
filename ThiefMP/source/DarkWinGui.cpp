/*************************************************************
* File: DarkWinGUI.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements the god-awful mess that is Thief MP's UI
*************************************************************/

#include "stdafx.h"

#include "DarkWinGui.h"
#include "GlobalServer.h"
#include "Engine\inc\MemPatch.h"

#if (GAME == GAME_THIEF)
#define ADDR_FIXFOCUS 0x00599160
#elif (GAME == GAME_DROMED)
#define ADDR_FIXFOCUS 0x00638670
#endif

#ifdef _DEBUG
#define _BUILDSTRING "Debug"
#elif defined _RELEASE
#define _BUILDSTRING "Release"
#else
#define _BUILDSTRING "Optimized"
#endif

InterfaceManager* UI = NULL;

InterfaceManager::InterfaceManager()
{
	m_pConsole = NULL;
	m_pConnectDlg = NULL;
	m_pHostDlg = NULL;
	m_pMissDlg = NULL;
	m_pPlayerDlg = NULL;
	m_pPasswordDlg = NULL;
}

InterfaceManager::~InterfaceManager()
{
	Shutdown();
}

void InterfaceManager::CreateChildWindow(int windowID)
{
	switch (windowID)
	{
	case CW_Console:
		if (!m_pConsole)
		{
			new CThiefConsole(g_hInstDLL, IDD_CONSOLE, IDC_OUTPUT_BOX, IDC_CMD_EDIT, CThiefConsole::StaticDialogProc);

			m_pConsole->SetCommands(ConsoleCmds, g_numCommands);
			m_pConsole->SetAcceleratorTable(IDR_ACCELERATOR1);
			m_pConsole->SetWindowTitle("Thief Multiplayer");

			// Prevent Thief from stealing the console's focus
			SafeWriteData((void*)ADDR_FIXFOCUS, 0xEB, 1);
		}

		ConPrintF("Console initialized.");
		ConPrintF("%s build %d, compiled on %s %s.", _BUILDSTRING, TMP_BUILD, __DATE__, __TIME__);
		//ConPrintF("Type /help for a listing of commands.");
		break;

	case CW_ConnectDialog:
		if (!m_pConnectDlg)
			m_pConnectDlg = new CConnectDialog;
		m_pConnectDlg->DisplayPage();
		break;
	case CW_HostDialog:
		if (!m_pHostDlg)
			m_pHostDlg = new CHostDialog;
		else
			ConPrintF("Shouldn't have tried to create dialog twice");
		m_pHostDlg->DisplayPage();
		break;
	case CW_MissionDialog:
		{
			if (!m_pMissDlg)
				m_pMissDlg = new CMissionSelectDialog;
			m_pMissDlg->DisplayPage();
			break;
		}
	case CW_PlayerDialog:
		{
			if (!m_pPlayerDlg)
				m_pPlayerDlg = new CPlayerSettingsDialog;
			m_pPlayerDlg->DisplayPage();
			break;
		}
	case CW_PasswordDialog:
		{
			if (!m_pPasswordDlg)
				m_pPasswordDlg = new CPasswordDialog;

			m_pPasswordDlg->DisplayPage();
		}
	default:
		return;
	}
}

void InterfaceManager::Startup()
{
	CreateChildWindow(CW_Console);
}

void InterfaceManager::Shutdown()
{
	SAFE_DELETE(m_pConsole);
	SAFE_DELETE(m_pHostDlg);
	SAFE_DELETE(m_pConnectDlg);
}

void InterfaceManager::Update()
{
	if (m_pConsole)
		m_pConsole->Update();
}

INT_PTR CALLBACK CThiefConsole::StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (UI && UI->m_pConsole)
		return UI->m_pConsole->DialogProc(hDlg, msg, wParam, lParam);
	return 0;
}

INT_PTR CALLBACK CThiefConsole::DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	dbgassert(UI->m_pConsole);

	switch (msg)
	{
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDA_ENTER:
			case IDC_SEND: OnEnterKey(); return TRUE;
			case IDA_HOST:
			case ID_SERVER_HOST: UI->CreateChildWindow(CW_HostDialog); return TRUE;
			case ID_SERVER_SERVERBROWSER:
				g_pBrowser = new CServerBrowser(); 
				if (!g_pBrowser->CreateBrowserWindow(IDD_BROWSER))
					ConPrintF("Failed to create server browser: %s.", LastErrorToString().c_str());
				return TRUE;
			case IDA_PLAYER:
			case ID_OPTIONS_PLAYERSETTINGS: UI->CreateChildWindow(CW_PlayerDialog); return TRUE;
			case IDA_JOIN:
			case ID_SERVER_CONNECT: UI->CreateChildWindow(CW_ConnectDialog); return TRUE;
			case ID_SERVER_DISCONNECT: 
				g_pNetMan->Leave(); 
				ConPrintF("Disconnected from session."); return TRUE;
			case ID_HOSTOPTIONS_CHANGEMISSION: UI->CreateChildWindow(CW_MissionDialog); return TRUE;
			case IDA_ESCAPE: return TRUE;
			case IDA_UP: OnUpKey(); return TRUE;
			case IDA_DOWN: OnDownKey(); return TRUE;
			case IDC_CMD_EDIT:
				if (HIWORD(wParam) == EN_UPDATE)
					EnableWindow(GetDlgItem(GetHandle(), IDC_SEND), m_EditBox.GetTextLength() ? TRUE : FALSE);
				break;

				// Debug menu
			case ID_DEBUG_GENERAL: Debug.ToggleFlag(DEBUG_GENERAL); break;
			case ID_DEBUG_PACKETSENDS: Debug.ToggleFlag(DEBUG_SENDS); break;
			case ID_DEBUG_PACKETRECEIVES: Debug.ToggleFlag(DEBUG_RECEIVES); break;
			case ID_DEBUG_OBJECTS: Debug.ToggleFlag(DEBUG_OBJECTS); break;
			case ID_DEBUG_FROBS: Debug.ToggleFlag(DEBUG_FROBS); break;
			case ID_DEBUG_DAMAGE: Debug.ToggleFlag(DEBUG_DAMAGE); break;
			case ID_DEBUG_NETWORK: Debug.ToggleFlag(DEBUG_NET); break;
			case ID_DEBUG_PROPERTYSENDS: Debug.ToggleFlag(DEBUG_PROPSENDS); break;
			case ID_DEBUG_QUESTDATA: Debug.ToggleFlag(DEBUG_QUESTS); break;
			case ID_DEBUG_SOUNDS: Debug.ToggleFlag(DEBUG_SOUNDS); break;
			case ID_DEBUG_SCRIPTS: Debug.ToggleFlag(DEBUG_SCRIPTS); break;
			case ID_DEBUG_INVENTORY: Debug.ToggleFlag(DEBUG_INVENTORY); break;
			case ID_DEBUG_GHOSTS: Debug.ToggleFlag(DEBUG_GHOSTS); break;
			case ID_DEBUG_GLOBALSERVER: Debug.ToggleFlag(DEBUG_GLOBAL); break;
			case ID_DEBUG_PROXYACTIONS: Debug.ToggleFlag(DEBUG_PROXYACTS); break;

			case ID_DEBUG_DISABLEALL: Debug.ClearAllFlags(); break;
			case ID_DEBUG_SAVEFLAGS: Debug.SaveFlags(); break;
			case ID_DEBUG_ECHO: DbgMenuToggleEcho(); break;
			}
		}
	case WM_INITMENU:
		{
			if ((HMENU)wParam == m_hMenu)
			{
				EnableMenuItem(m_hMenu, ID_SERVER_DISCONNECT, g_pNetMan->IsNetworkGame() ? MF_ENABLED : MF_GRAYED);
				EnableMenuItem(m_hMenu, ID_HOSTOPTIONS_CHANGEMISSION, (g_pNetMan->IsNetworkGame() && g_pNetMan->AmDefaultHost()) ? MF_ENABLED : MF_GRAYED);
				EnableDebugMenuItems(m_hMenu);
			}
		}
	}

	return FALSE;
}

int PromptDisconnect()
{
	return MessageBox(UI->m_pConsole->GetHandle(), "Would you like to disconnect from the current game?", "Already Connected", MB_YESNO | MB_ICONQUESTION);
}

void WinGuiInit()
{
	if (Cfg.GetInt("MenuEnabled"))
	{
		UI = new InterfaceManager();
		UI->Startup();

		Log.SetLogCallback(LogPrintCallback);
	}
}

void WinGuiTerm()
{
	if (UI)
		UI->Shutdown();

	SAFE_DELETE(UI);
}