#pragma once

#include "Engine\inc\Lists.h"

#include "Main.h"

using namespace EngCtrls;

class HistoryItem
{
public:
	HistoryItem(const char* text) { m_pText = new char[strlen(text) + 1]; strcpy(m_pText, text); }
	virtual ~HistoryItem() 
	{
		if (m_pText)
		{
			delete[] m_pText; 
		}
	}

	const char* GetText() { return m_pText; }

protected:
	char* m_pText;
};

class HistoryManager
{
public:
	typedef std::list<HistoryItem*> HistoryList;

	HistoryManager()
	{
		m_bDisplayingHistory = false;
		m_currentItem = NULL;
	}
	~HistoryManager();

	void AddLine(const char* text);
	void Advance(bool forward);
	HistoryItem* GetLast();

	bool IsDisplayingHistory() { return m_bDisplayingHistory; }
	void SetDisplayingHistory(bool bDisplaying) { m_bDisplayingHistory = bDisplaying; }

	HistoryItem* GetCurrentItem() { return m_currentItem; }
	void SetCurrentItem(HistoryItem* pItem) { m_currentItem = pItem; }

protected:
	bool m_bDisplayingHistory;
	HistoryItem* m_currentItem;
	static const int m_kMaxLines = 10;

	
	HistoryList m_History;
};

typedef void (*CmdHandler)(...);
struct ConsoleCommand
{
	const char* strCommand;
	const char* strShortcut;
	eArgType argType;
	CmdHandler	func;
	const char* strHelp;
	bool bIsCheat;
};

class CConsoleBase
{
public:
	CConsoleBase(HINSTANCE hInst, int dlgRsrc, int outputEditRsrc, int inputEditRsrc, DLGPROC proc);
	virtual ~CConsoleBase();

	void PostMessage(const char* str);
	HWND GetHandle() { return m_hConsole; }
	int FindCommand(const char* command);
	void ExecuteCommand(int cmdNum, const char* args);
	bool ExecuteRaw(char* buffer);
	char* TokenizeCommand(char* buffer);
	void SetWindowTitle(const char* windTitle);
	void SetCommands(ConsoleCommand* pCommands, int numCommands);
	void SetAcceleratorTable(int resource);
	void Update();

	bool QueueEmpty() { return (m_MessageQueue.GetSize() == 0); }

	const CEEditBox& GetOutput() const { return m_OutputEdit; }
	const CEEditBox& GetInput() const { return m_EditBox; }

	virtual void OnInit();
	virtual void OnUpKey();
	virtual void OnDownKey();
	virtual void OnEnterKey();
	virtual void OnCharacter(WPARAM wParam) { };

protected:
	CEEditBox m_OutputEdit;
	CEEditBox m_EditBox;	// edit box for text entry
	HistoryManager m_History;
	SimpleQueue<char> m_MessageQueue;
	HACCEL m_hAccel; // accelerators
	HWND m_hConsole;  // Main window handle for the console
	HMENU m_hMenu;
	ConsoleCommand* m_pCommands;
	int m_numCommands;
};

class CHostDialog
{
public:
	CHostDialog();

	INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	void DisplayPage();
	void OnHost();
	void OnInitDialog(HWND hDlg);
	void InitButtons();
	void InitControlStates();
	void SaveSettings();
	DWORD GetMissionFlags(int mission);

protected:
	HWND m_hDlg;
	CEButton m_btnShareLoot;
	CEButton m_btnRespawn;
	CEButton m_btnDifficulty;
	CEButton m_btnTransparency;
	CEButton m_btnTimeLimit;
	CEButton m_btnSaveLimit;
	CEButton m_btnNoLoadout;
	CEButton m_btnTeamDmg;
	CEButton m_btnListServer;
	CEButton m_btnEnableCheats;
	CEComboBox m_cmbGameType;
	CEComboBox m_cmbPlayers;
	CEComboBox m_cmbMission;
	CEEditBox m_edtTimeLimit;
	CEEditBox m_edtSaveLimit;
	CEEditBox m_edtSpawnTime;
	CEEditBox m_edtPort;
	CEEditBox m_edtServerName;
	CEEditBox m_edtStartingLoot;
	CEEditBox m_edtPassword;

	int m_forceMission;
	char m_passwordBuff[kMaxPassword + 1];
};

class CPasswordDialog
{
public:
	CPasswordDialog();

	INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

	void DisplayPage();
	void OnInitDialog(HWND hDlg, LPARAM lParam);

protected:
	HWND m_hDlg;
	CEEditBox m_edtPassword;

	class CServerListEntry* m_pServer;
	wchar_t m_passwordBuff[kMaxPassword * 2 + 2];
};

class CConnectDialog
{
public:
	CConnectDialog();
	INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	void DisplayPage();
	void OnConnect();
	void OnInitDialog(HWND hDlg);
	void SaveSettings();

protected:
	HWND m_hDlg;
	HWND m_hConnect;
	CEEditBox m_edtIPAddress;
	CEEditBox m_edtPort;
	CEEditBox m_edtPassword;

	char m_selectedIP[32];
	int m_selectedPort;
	bool m_bPasswordEntered;
	wchar_t m_passwordBuff[(kMaxPassword * sizeof(wchar_t)) + sizeof(wchar_t)];
};

class CMissionSelectDialog
{
public:
	INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

	void DisplayPage();
	void OnInitDialog(HWND hDlg);

protected:
	CEListBox m_MissionList;
	int m_SelectedMission;
};

enum ChildWindows
{
	CW_Console,
	CW_ConnectDialog,
	CW_HostDialog,
	CW_MissionDialog,
	CW_PlayerDialog,
	CW_PasswordDialog,
};

class CPlayerSettingsDialog
{
public:
	INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK StaticDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

	void DisplayPage();
	void OnInitDialog(HWND hDlg);

	void InitButtons();
	void InitControlStates();

	void SaveSettings();

protected:
	HWND m_hDlg;
	CEEditBox m_edtName;
	CEComboBox m_cmbModels;
};

interface IInterfaceManager
{
public:
	virtual void CreateChildWindow(int windowID) = 0;
	virtual void Startup() = 0;
	virtual void Shutdown() = 0;
	virtual void Update() = 0;
};

int PromptDisconnect();
void EnableDebugMenuItems(HMENU hMenu);
void DbgMenuToggleEcho();

extern HINSTANCE g_hInstDLL;