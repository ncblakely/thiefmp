#pragma once

void DarkUIUtilsInit();
void DarkUIUtilsTerm();

float GetTextWidth(grs_font* font, const char* text);
void TextDraw(grs_font* font, int color, const char* message, int x, int y);
void TextDraw(grs_font* font, int color, const char* message, float x, float y);
void TextDrawCentered(grs_font* font, int color, const char* message, float x, float y);
void TextDrawRight(grs_font* font, int color, const char* message, float x, float y);


extern grs_font* g_TextFont;
extern IRes* g_TextFontRes;
extern grs_font* g_SmallFont;
extern IRes* g_SmallFontRes;