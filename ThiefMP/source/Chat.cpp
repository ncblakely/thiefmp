/*************************************************************
* File: Chat.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Handles processing of the in-game chatbox.
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "DarkWinGui.h"
#include "DarkOverlay.h"

char g_chatBuffer[256];
static bool s_chatboxOpen = false;
static bool s_chatboxInitialized = false;
static DWORD g_prevContext;

TextboxState g_chatBoxSubroot;
_LGadTextBox g_chatBox;

// Performs basic initialization for the in-game chatbox
void DarkChatInit()
{
	ZeroMemory(&g_chatBoxSubroot, sizeof(TextboxState));
	_LGadSetupSubRoot(&g_chatBoxSubroot, _LGadCurrentRoot(), 0, 0, 0, 0);

	LGadTextboxCreateParams parms;
	ZeroMemory(&parms, sizeof(LGadTextboxCreateParams));

	parms.maxChars = kMaxMessageLength;
	parms.x10 = 4;
	parms.pfnCB = &ChatboxCallback;
	parms.pTextBuffer = g_chatBuffer;
	_LGadCreateTextboxDesc(&g_chatBox, &g_chatBoxSubroot, &parms);
	s_chatboxInitialized = true;
}

void DarkChatTerm()
{
	if (s_chatboxInitialized)
	{
		_LGadDestroyTextBox(&g_chatBox);
		_LGadDestroyRoot((_LGadRoot*)&g_chatBoxSubroot);
		s_chatboxInitialized = false;
	}
}

bool ChatboxIsOpen()
{
	return s_chatboxOpen;
}

// Opens the in-game chatbox (when 't' is pressed)
void ChatboxOpen()
{
	_region_set_invisible(&g_chatBoxSubroot, 0);
	_TextGadgUpdate(&g_chatBox);
	_TextGadgFocus(&g_chatBox);

	//_LGadDrawBox(&g_chatBoxDesc, 0);

	DWORD context;
	g_pInputBinder->GetContext(&context);

	if (context != 0x200)
	{
		g_pInputBinder->SetContext(0x200, 1); // 0x200 - shockchat value //g_pInputBinder->SetContext(0x10, 1);

		// Stop head movement
		_headmoveSetRelPosX(0);
		_headmoveSetRelPosY(0);
		_headmoveTouch();
		//g_pInputBinder->SetValidEvents(-9);

		g_prevContext = context;
	}

	s_chatboxOpen = true;
}

// Closes the chatbox and resets the input context
void ChatboxClose(_LGadTextBox* chatBox)
{
	sScreenMode mode = GetScreenMode();

	char* chatMsg = chatBox->text;
	chatMsg[0] = NULL;


	_TextGadgUnfocus(chatBox);
	_region_set_invisible(&g_chatBoxSubroot, 1);

	g_pInputBinder->SetContext(g_prevContext, 1);
	//g_pInputBinder->SetValidEvents(-1);
	_mouse_put_xy(mode.width / 2, mode.height / 2); // Re-center so head doesn't move
	s_chatboxOpen = false;
}

void ChatboxCallback(_LGadTextBox* chatBox, _LGadTextBoxEvent event, int eventID, void*)
{
	switch (eventID)
	{
	case 0x11B: // Escape key
		{
			ChatboxClose(chatBox);
			break;
		}
	case 0x10D: // Enter key
		{
			char* chatMsg = chatBox->text;
			dbgassert(chatMsg);

			// Check to see if we need to parse a command or send chat
			if (chatMsg[0] == '/')	// commands always begin with a forward slash
			{
				if (Cfg.GetInt("MenuEnabled"))
				{
					if (!UI->m_pConsole->ExecuteRaw(chatMsg))
						g_pInputBinder->ProcessCmd(&chatMsg[1]); // let Thief process the command if it's not one of ours
				}
			}
			else if (chatMsg[0])
			{
				g_pDarkNet->SendChat(false, chatMsg);
			}

			ChatboxClose(chatBox);
		}
	}
}