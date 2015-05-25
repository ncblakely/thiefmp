/*************************************************************
* File: ConsoleGUI.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements console interface
*************************************************************/

#include "stdafx.h"
#include "UI.h"
#include "GlobalServer.h"
#include "Client.h"
#include "Engine\inc\EngError.h"

// Call InitCommonControls first
CConsoleBase::CConsoleBase(HINSTANCE hInst, int dlgRsrc, int outputEditRsrc, int inputEditRsrc, DLGPROC proc)
{
	// initialize dialog
	assert(hInst);

	m_hConsole = CreateDialog(hInst, MAKEINTRESOURCE(dlgRsrc), NULL, proc);
	if (!m_hConsole)
	{
		ErrorExit("CreateDialog");
		return;
	}

	m_hMenu = GetMenu(m_hConsole);

	// initialize text display edit box
	m_OutputEdit.Init(m_hConsole, outputEditRsrc);
	m_OutputEdit.SetMaxLines(300);

	// initialize edit box for text entry
	m_EditBox.Init(m_hConsole, inputEditRsrc);

	assert(m_OutputEdit.GetHandle() && m_EditBox.GetHandle());

	ShowWindow(m_hConsole, SW_SHOWNORMAL);
}

CConsoleBase::~CConsoleBase()
{
	while (m_MessageQueue.GetSize())
	{
		char* pCmdStr = m_MessageQueue.Pop();

		delete[] pCmdStr;
	}
}

void CConsoleBase::PostMessage(const char* str)
{
	char* pHeapStr = new char[strlen(str) + 1];
	strcpy(pHeapStr, str);
	m_MessageQueue.Push(pHeapStr);
}

int CConsoleBase::FindCommand(const char* command)
{
	if (strlen(command))
	{
		for (int i = 0; i < m_numCommands; ++i)
		{
			if (!_stricmp(m_pCommands[i].strCommand, command))
				return i;
			if (m_pCommands[i].strShortcut)
			{
				if (!_stricmp(m_pCommands[i].strShortcut, command))
					return i;
			}
		}
	}

	return -1;
}

void CConsoleBase::ExecuteCommand(int cmdNum, const char* args)
{
#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
#ifdef _RELEASE
	if (g_pDarkNet && g_pNetMan->IsNetworkGame())
	{
		if (ConsoleCmds[cmdNum].bIsCheat && !g_pDarkNet->GetSessionInfo().cheatsEnabled)
			return;
	}
#endif
#endif

	if (!args)
	{
		if (m_pCommands[cmdNum].argType != ARG_NONE)
			return ConsoleCmds[cmdNum].func(NULL);
		else
			return ConsoleCmds[cmdNum].func();
	}

	switch (ConsoleCmds[cmdNum].argType)
	{
	case ARG_NONE:
		return ConsoleCmds[cmdNum].func();
	case ARG_BOOL:
		return ConsoleCmds[cmdNum].func(atobool(args));
	case ARG_INT:
		return ConsoleCmds[cmdNum].func(atoi(args));
	case ARG_DOUBLE:
		return ConsoleCmds[cmdNum].func(atof(args));
	case ARG_STRING:
		return ConsoleCmds[cmdNum].func(args);
	default:
		return;
	}
}

bool CConsoleBase::ExecuteRaw(char* buffer)
{
	const char* pArgs = StringEnd(buffer, 1);
	char *pCmd = TokenizeCommand(buffer);

	int cmdNum = FindCommand(pCmd);	
	if (cmdNum >= 0)
	{
		ExecuteCommand(cmdNum, pArgs);
		return true;
	}

	return false;
}

char* CConsoleBase::TokenizeCommand(char* buffer)
{
	return strtok(buffer, " ");
}

void CConsoleBase::OnEnterKey()
{
	if (m_EditBox.GetTextLength() > 0)
	{
		char buffer[MAX_PATH];
		GetDlgItemText(m_hConsole, IDC_CMD_EDIT, buffer, MAX_PATH - 1);

		m_History.AddLine(buffer);
		m_History.SetDisplayingHistory(false);

		if (buffer[0] == '/')
		{
			if (!ExecuteRaw(buffer))
				ConPrintF("Unknown command.");
		}
		else if (g_pDarkNet)
			g_pDarkNet->SendChat(false, buffer);
		else
			ConPrintF("You are not connected to a server.");
		m_EditBox.ClearText();
	}
}

void CConsoleBase::OnUpKey()
{
	HistoryItem* pItem;
	if (!m_History.IsDisplayingHistory())
	{
		pItem = m_History.GetLast();
		if (!pItem)
			return;

		m_History.SetCurrentItem(pItem);
		m_History.SetDisplayingHistory(true);
	}
	else
	{
		pItem = m_History.GetCurrentItem();
		if (!pItem)
			return;

		m_History.Advance(false);
	}

	m_EditBox.SetText(pItem->GetText());
	m_EditBox.CaretToEnd();
}

void CConsoleBase::OnDownKey()
{
	if (!m_History.IsDisplayingHistory())
		return;

	m_History.Advance(true);

	if (m_History.GetCurrentItem())
	{
		m_EditBox.SetText(m_History.GetCurrentItem()->GetText());
		m_EditBox.CaretToEnd();
	}
}

void CConsoleBase::OnInit()
{
	SetFocus(GetDlgItem(m_hConsole, IDC_CMD_EDIT));
}

void CConsoleBase::Update()
{
	MSG msg;
	assert(m_hConsole);

	while (m_MessageQueue.GetSize())
	{
		char* pCmdStr = m_MessageQueue.Pop();
		m_OutputEdit.AppendString(pCmdStr);
		delete[] pCmdStr;
	}

	while (PeekMessage(&msg, m_hConsole, NULL, NULL, PM_REMOVE))
	{
		if (!TranslateAccelerator(m_hConsole, m_hAccel, &msg))
		{
			if (!IsDialogMessage(m_hConsole, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}

void CConsoleBase::SetWindowTitle(const char* windTitle)
{
	SetWindowText(m_hConsole, windTitle);
}

void CConsoleBase::SetCommands(ConsoleCommand* pCommands, int numCommands)
{
	m_pCommands = pCommands;
	m_numCommands = numCommands;
}

void CConsoleBase::SetAcceleratorTable(int resource)
{
	// initialize accelerators
	if (resource)
		m_hAccel = LoadAccelerators(g_hInstDLL, MAKEINTRESOURCE(resource));
	if (!m_hAccel)
		Log.Print("Failed to load accelerators.");
}

void HistoryManager::AddLine(const char* text)
{
	if (m_History.size() >= m_kMaxLines)
	{
		m_History.erase(m_History.begin());
	}

	m_History.push_back(new HistoryItem(text));
}

HistoryItem* HistoryManager::GetLast()
{
	if (!m_History.empty())
	{
		return m_History.front();
	}

	return NULL;
}

HistoryManager::~HistoryManager()
{
	for (HistoryList::iterator it = m_History.begin(); it != m_History.end(); it++)
		delete *it;
}

void HistoryManager::Advance(bool forward)
{
	if (m_History.empty())
		return;

	HistoryItem* item = GetCurrentItem();
	HistoryItem* nextItem = NULL;
	for (HistoryList::iterator it = m_History.begin(); it != m_History.end(); it++)
	{
		if (*it == item)
		{
			HistoryList::iterator nextIt = it;
			if (forward)
			{		
				if (++nextIt != m_History.end())
					nextItem = *nextIt;
			}
			else
			{
				if (nextIt != m_History.begin())
				{
					nextIt--;
					nextItem = *nextIt;
				}
			}
		}
	}

	if (nextItem)
		SetCurrentItem(nextItem);
}