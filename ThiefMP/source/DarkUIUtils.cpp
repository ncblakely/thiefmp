/*************************************************************
* File: DarkUIUtils.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Draws on-screen text and manages font globals.
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "DarkUIUtils.h"

grs_font* g_TextFont = NULL;
IRes* g_TextFontRes = NULL;
grs_font* g_SmallFont = NULL;
IRes* g_SmallFontRes = NULL;

void DarkUIUtilsInit()
{
	IPtr<IResMan> resman = _AppGetAggregated(IID_IResMan);

	g_TextFontRes = resman->Bind("textfont", "Font", NULL, "intrface", NULL);
	g_TextFont = (grs_font*)g_TextFontRes->Lock();

	g_SmallFontRes = resman->Bind("smalfont", "Font", NULL, "intrface", NULL);
	g_SmallFont = (grs_font*)g_SmallFontRes->Lock();
}

void DarkUIUtilsTerm()
{
	g_TextFontRes->Unlock();
	g_TextFontRes->Release();

	g_SmallFontRes->Unlock();
	g_SmallFontRes->Release();
}

float GetTextWidth(grs_font* font, const char* text)
{
	float width = _gr_font_string_width(font, text);
	return width / _grd_canvas->width;
}

void TextDrawCentered(grs_font* font, int color, const char* message, float x, float y)
{
	if (!message)
		return;

	x = x - (GetTextWidth(font, message) / 2);
	if (x < 0.0f) x = 0.0f;

	TextDraw(font, color, message, (int)(x * _grd_canvas->width), (int)(y * _grd_canvas->height));
}

void TextDrawRight(grs_font* font, int color, const char* message, float x, float y)
{
	if (!message)
		return;

	x = x - (GetTextWidth(font, message));
	if (x < 0.0f) x = 0.0f;

	TextDraw(font, color, message, x, y);
}

void TextDraw(grs_font* font, int color, const char* message, float x, float y)
{
	return TextDraw(font, color, message, (int)(x * _grd_canvas->width), (int)(y * _grd_canvas->height));
}

void TextDraw(grs_font* font, int color, const char* message, int x, int y)
{
	if (!message)
		return;

#if (GAME == GAME_DROMED)
	_ScrnLockDrawCanvas();
#endif

	if (!color)
		_grd_canvas->color = _gr_make_screen_fcolor(Cfg.GetInt("TextColor"));
	else
		_grd_canvas->color = color;

	_gr_font_string(font, message, x, y);

#if (GAME == GAME_DROMED)
	_ScrnUnlockDrawCanvas();
#endif
}