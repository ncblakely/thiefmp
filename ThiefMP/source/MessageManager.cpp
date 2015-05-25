/*************************************************************
* File: MessageManager.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implementation of the message manager class used for displaying chat.
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "Chat.h"
#include "DarkUIUtils.h"
#include "Client.h"
#include "DarkOverlay.h"

static MessageMgr* s_mgr = NULL;

Message::Message(const char* msgText, float expireTime) 
{
	int length = (int)strlen(msgText);
	m_text = new char[length + 1];

	m_expireTime = expireTime;

	strcpy(m_text, msgText);
	m_text[length] = NULL;
}

Message::~Message()
{
	if (m_text)
	{
		delete[] m_text;
	}
}

MessageMgr::MessageMgr() : m_cs("msg mgr")
{
	m_renderEnabled = true;
}

MessageMgr::~MessageMgr()
{
	s_mgr = NULL;
	ClearMessages();
}

MessageMgr* MessageMgr::Get()
{
	if (!s_mgr)
		s_mgr = new MessageMgr();
	return s_mgr;
}

void MessageMgr::Shutdown()
{
	if (s_mgr)
		delete s_mgr;
}

//======================================================================================
// Name: AddLine
//
// Desc: Adds a text message to the linked list of messages.
//======================================================================================
void MessageMgr::AddLine(bool bPlaySound, const char* message)
{
	m_cs.Lock();

	if (message)
	{
		Message* msg = new Message(message, g_DeltaFrame.GetElapsedTime() + 10.0f);

		if (m_Messages.size() >= kMaxMessages)
			DeleteTopmost();

		m_Messages.push_back(msg);

		if (bPlaySound)
			_SFX_Play_Raw(1, 0, "tick2");
	}

	m_cs.Unlock();
}

//======================================================================================
// Name: AddLineFormat
//
// Desc: Formats and adds a text message to the linked list of messages.
//======================================================================================

void MessageMgr::AddLineFormat(bool bPlaySound, const char* message, ...)
{
	char buf[1024];
	va_list marker;
	va_start(marker, message);
	_vsnprintf(buf, sizeof(buf)-1, message, marker);
	buf[sizeof(buf) - 1] = NULL;

	AddLine(bPlaySound, buf);
}

//======================================================================================
// Name: Clear
//
// Desc: Deletes and frees memory used for all messages that are currently being displayed.
//======================================================================================
void MessageMgr::ClearMessages()
{
	for (MessageList::iterator it = m_Messages.begin(); it != m_Messages.end(); it++)
	{
		Message* msg = *it;
		delete msg;
	}
	m_Messages.clear();
}
//======================================================================================
// Name: DeleteFirst
//
// Desc: Deletes the topmost message and shifts all messages upwards.
//======================================================================================
void MessageMgr::DeleteTopmost()
{
	if (!m_Messages.empty())
	{
		Message* msg = m_Messages.front();
		delete msg;
		m_Messages.erase(m_Messages.begin());
	}
}

void MessageMgr::DrawInput()
{
	GUIsetupStruct gss;
	IDarkOverlay* staging = g_pOverlayMgr->FindByName("overlay_staging");
	bool stagingMode = staging->IsEnabled();

	if (stagingMode)
		_GUIsetup(&gss, &staging->GetDrawingArea(), 2, 0);

	if (stagingMode)
	{
		// Draw "Chat" prompt
		TextDraw(g_SmallFont, CLR_White, "Chat: ", 0.10f, 0.43f);
	}

	if (ChatboxIsOpen())
	{
		if (stagingMode)
		{
			TextDraw(g_SmallFont, CLR_White, g_chatBuffer, 0.17f, 0.43f); // Draw entered text
		}
		else
			TextDraw(g_SmallFont, CLR_White, g_chatBuffer, 0.0f, 0.90f); // Draw entered text

		// Draw insertion point
		if ((int)(g_DeltaFrame.GetElapsedTime() * 2.0f) & 1) // flash on and off
		{	
			short height = 0;
			short width = 0;
			char save = g_chatBuffer[g_chatBox.cursorPos];
			g_chatBuffer[g_chatBox.cursorPos] = '\0';
			_gr_font_string_size(g_SmallFont, g_chatBuffer, &width, &height);

			int ypos, xpos;
			if (stagingMode)
			{
				float textWidth = (0.17f + GetTextWidth(g_SmallFont, g_chatBuffer));
				xpos = (int)(textWidth * _grd_canvas->width);
				ypos = (int)(0.43f * _grd_canvas->height);
			}
			else
			{
				ypos = (int)(0.90f * _grd_canvas->height);
				xpos = width;
			}
				
			_gd_vline(xpos, ypos, ypos + height);
			g_chatBuffer[g_chatBox.cursorPos] = save;
		}
	}

	if (stagingMode)
		_GUIdone(&gss); 
}

void MessageMgr::EnableRendering(bool enable)
{
	m_renderEnabled = enable;
}

//======================================================================================
// Name: MessageManager::Update 
//
// Desc: Updates message display, and expires messages that have timed out.
//======================================================================================
void MessageMgr::Update()
{
	if (m_renderEnabled && g_pOverlayMgr) // is the sim running?
	{
		IDarkOverlay* staging = g_pOverlayMgr->FindByName("overlay_staging");

		m_cs.Lock();

		//if (!m_textColor)
			//m_textColor = _gr_make_screen_fcolor(Cfg.GetInt("TextColor"));

		Message* pMsg = NULL;
		if (!m_Messages.empty())
		{
			pMsg = m_Messages.front();
			if (g_DeltaFrame.GetElapsedTime() > pMsg->GetExpireTime())
			{
				DeleteTopmost();
			}

			short textHeight = _gr_font_string_height(g_TextFont, "X");

			int index = 0;
			for (MessageList::iterator it = m_Messages.begin(); it != m_Messages.end(); it++)
			{
				pMsg = *it;

				// 7/16/10 - if this is needed for dromed shouldn't it be enabled for game too?
				//#if (GAME == GAME_DROMED)
				_ScrnLockDrawCanvas();
				//#endif

				if (staging->IsEnabled())
				{
					GUIsetupStruct gss;
					// Hack in different positioning when staging is shown
					_GUIsetup(&gss, &staging->GetDrawingArea(), 2, 0);
					TextDraw(g_TextFont, CLR_Red, pMsg->GetText(), 0.10f, 0.45f + (index * 0.03f));
					_GUIdone(&gss); 
				}
				else
				{
					if (Client.IsTimeLimitSet())
						TextDraw(g_TextFont, CLR_Red, pMsg->GetText(), 2, (15 * index) + textHeight); // 7/31/10 - make sure it doesn't obscure time limit
					else
						TextDraw(g_TextFont, CLR_Red, pMsg->GetText(), 2, (15 * index));
				}

				//#if (GAME == GAME_DROMED)
				_ScrnUnlockDrawCanvas();
				//#endif
				index++;
			}
		}

		// Draw time limit
		if (Client.IsTimeLimitSet())
		{
			TextDrawCentered(g_TextFont, CLR_Red, TimeString((int)Client.GetTimeRemaining()), 0.5f, 0.0f);
		}

		// Draw the chat box input
		DrawInput();

		m_cs.Unlock();
	}
}