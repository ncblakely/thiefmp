/*************************************************************
* File: DialogElement.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements objects for new-style screen overlay UIs.
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "DarkOverlay.h"
#include "DarkUIUtils.h"

static bool PointInRect(Point pt, Rect& r)
{
	return pt.y >= r.top && pt.y < r.bottom && pt.x >= r.left && pt.x < r.right;
}

//
// DialogElement

DialogElement::~DialogElement()
{
	if (m_data)
		delete m_data;
}

DialogElement::DialogElement(IDarkOverlay* parent)
{
	m_enabled = true;
	m_data = NULL;
	m_clickCB = NULL;
	m_flags = NULL;
	m_parent = parent;
}

void DialogElement::SetEnabled(bool enabled)
{
	m_enabled = enabled;
}

void* DialogElement::GetData()
{
	return m_data;
}

bool DialogElement::HandleClick(Point pt)
{
	if (m_enabled)
	{
		// Adjust position for drawing area
		Rect r = m_location;
		r.Offset(m_parent->GetDrawingArea().left, m_parent->GetDrawingArea().top);

		if (m_clickCB && PointInRect(pt, r))
		{
			m_clickCB(this, pt);

			if (m_flags & ElementPlaySelectSound)
				_SFX_Play_Raw(1, 0, "select1.wav");

			return true;
		}
	}

	return false;
}

void DialogElement::SetData(void* data)
{
	m_data = data;
}

IDarkOverlay* DialogElement::GetParent()
{
	return m_parent;
}

//
// Element_Text

Element_Text::Element_Text(IDarkOverlay* parent, const char* title, float x, float y, DWORD flags, DWORD color, grs_font* font) : DialogElement(parent)
{
	IPtr<IResMan> pResMan = _AppGetAggregated(IID_IResMan);

	m_font = font;
	m_title = title;
	m_flags = flags;
	
	m_textColor = color;

	if (FlagIsSet(flags, ElementAlignRight))
		x = x - GetTextWidth();
	else if (FlagIsSet(flags, ElementAlignCenter))
		x = x - (GetTextWidth() / 2);
	else
		x = x;

	y = y;

	short textHeight = 0;
	short textWidth = 0;
	_gr_font_string_size(m_font, m_title, &textWidth, &textHeight);

	m_location.left = (short)(x * parent->GetDrawingArea().Width());
	m_location.top = (short)(y * parent->GetDrawingArea().Height());
	m_location.right = (short)(m_location.left + (float)textWidth);
	m_location.bottom = (short)(m_location.top + (float)textHeight);
}

Element_Text::~Element_Text()
{
}

float Element_Text::GetTextWidth()
{
	GUIsetupStruct gss;

	// Constrain to parent overlay's drawing area
	_GUIsetup(&gss, &m_parent->GetDrawingArea(), 2, 0);
	float width = ::GetTextWidth(m_font, m_title);
	_GUIdone(&gss);

	return width;
}

void Element_Text::Draw()
{
	if (!m_enabled)
		return;

	_grd_canvas->color = m_textColor;
	_gr_font_string(m_font, m_title, m_location.left, m_location.top);
}

void Element_Text::SetText(const char* str)
{
	m_title = str;
}

//
// Element_Button

Element_Button::Element_Button(IDarkOverlay* parent, const char* title, float x, float y, DWORD flags, DWORD color, grs_font* font, DialogClickCB clickCB) 
: Element_Text(parent, title, x, y, flags, color, font)
{
	m_clickCB = clickCB;
}

//
// Element_Image

Element_Image::Element_Image(IDarkOverlay* parent, const char* imageFile, const char* resFile, float x, float y, DWORD flags, DialogClickCB cb)
: DialogElement(parent)
{
	m_imageFile = imageFile;
	m_resFile = resFile;
	m_clickCB = cb;


	IRes* src = LoadPCX(m_imageFile, m_resFile);
	assert(src);

	ImageResource* img = (ImageResource*)src->Lock(); // lock to get image dimensions

	m_location.left = (short)(x * parent->GetDrawingArea().Width());
	m_location.top = (short)(y * parent->GetDrawingArea().Height());
	m_location.right = (short)(m_location.left + (float)img->width);
	m_location.bottom = (short)(m_location.top + (float)img->height);

	src->Unlock();
	FreePCX(src);
}

Element_Image::~Element_Image()
{
}

void Element_Image::Draw()
{
	if (!m_enabled)
		return;

	IRes* src = LoadPCX(m_imageFile, m_resFile);
	DrawByHandle(src, Point(m_location.left, m_location.top));
	FreePCX(src);
}

//
// Element_Edit

Element_Edit::Element_Edit(IDarkOverlay* parent, const char* title, float x, float y, DWORD flags, DWORD color, grs_font* font, int maxChars, DialogEnterCB enterCB) 
: DialogElement(parent)
{
	LGadTextboxCreateParams parms;
	ZeroMemory(&m_subroot, sizeof(m_subroot));
	ZeroMemory(&parms, sizeof(parms));
	ZeroMemory(&m_textBox, sizeof(m_textBox));

	m_focused = false;
	m_font = font;
	m_textColor = color;
	m_enterCB = enterCB;
	m_maxChars = maxChars;
	safe_strcpy(m_textBuff, title, sizeof(m_textBuff));

	// Set up text gadget
	_LGadSetupSubRoot(&m_subroot, _LGadCurrentRoot(), 0, 0, 0, 0);
	parms.maxChars = maxChars;
	parms.x10 = 4;
	parms.data = this;
	parms.pfnCB = GadgetCallback;
	parms.pTextBuffer = m_textBuff;
	_LGadCreateTextboxDesc(&m_textBox, &m_subroot, &parms);

	if (FlagIsSet(flags, ElementAlignRight))
		x = x - GetTextWidth();
	else if (FlagIsSet(flags, ElementAlignCenter))
		x = x - (GetTextWidth() / 2);
	else
		x = x;

	y = y;

	short textHeight = 0;
	short textWidth = 0;
	_gr_font_string_size(m_font, "A", &textWidth, &textHeight);

	m_location.left = (short)(x * parent->GetDrawingArea().Width());
	m_location.top = (short)(y * parent->GetDrawingArea().Height());
	m_location.right = (short)(m_location.left + (textWidth * m_maxChars));
	m_location.bottom = (short)(m_location.top + (float)textHeight);
}

Element_Edit::~Element_Edit()
{
	_LGadDestroyTextBox(&m_textBox);
	_LGadDestroyRoot((_LGadRoot*)&m_subroot);
}

void Element_Edit::GadgetCallback(_LGadTextBox* chatBox, _LGadTextBoxEvent event, int eventID, void* data)
{
	Element_Edit* edit = (Element_Edit*)data;

	switch (eventID)
	{
	case 0x11B: // Escape key
	case 0x10D: // Enter key
		edit->Focus(false);
		break;
	}
}

void Element_Edit::Draw()
{
	if (!m_enabled)
		return;

	_grd_canvas->color = m_textColor;
	_gr_font_string(m_font, m_textBuff, m_location.left, m_location.top);

	short height = 0;
	short width = 0;

	char save = m_textBuff[m_textBox.cursorPos];
	m_textBuff[m_textBox.cursorPos] = '\0';
	_gr_font_string_size(g_SmallFont, m_textBuff, &width, &height);
	m_textBuff[m_textBox.cursorPos] = save;

	// Draw outer frame
	short charWidth = _gr_font_string_width(m_font, "X");
	_gd_box(m_location.left - charWidth, m_location.top, m_location.right + charWidth, m_location.bottom);

	// Draw insertion point
	if ((int)(g_DeltaFrame.GetElapsedTime() * 4.0f) & 1) // flash on and off
	{	
		TextDraw(g_SmallFont, _grd_canvas->color, m_focused ? "|" : "_", m_location.left + width, m_location.bottom - height);
		//_gd_vline(m_location.left + width, m_location.bottom - height, m_location.bottom);
	}

}

void Element_Edit::Focus(bool focus)
{
	if (focus)
	{
		_TextGadgUpdate(&m_textBox);
		_TextGadgFocus(&m_textBox);

		g_pInputBinder->GetContext(&m_prevContext);

		if (m_prevContext != 0x200)
		{
			g_pInputBinder->SetContext(0x200, 1);
			g_pInputBinder->SetValidEvents(-9);
		}
	}
	else
	{
		_TextGadgUnfocus(&m_textBox);
		g_pInputBinder->SetContext(m_prevContext, 1);
	}

	m_focused = focus;
}

bool Element_Edit::HandleClick(Point pt)
{
	if (m_enabled)
	{
		// Adjust position for drawing area
		Rect r = m_location;
		r.Offset(m_parent->GetDrawingArea().left, m_parent->GetDrawingArea().top);

		if (PointInRect(pt, r))
		{
			Focus(true);

			return true;
		}
	}

	return false;
}

float Element_Edit::GetTextWidth()
{
	GUIsetupStruct gss;

	// Constrain to parent overlay's drawing area
	_GUIsetup(&gss, &m_parent->GetDrawingArea(), 2, 0);
	float width = ::GetTextWidth(m_font, m_textBuff);
	_GUIdone(&gss);

	return width;
}