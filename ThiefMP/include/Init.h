#pragma once

//init
void ConfigInit();
void DebugInit();
void StringHashInit();
void PatcherInit();
void PatchPropDescs();
void WinGuiInit();
void OverlayInit();
void OverlayTerm();
void NetPropertiesInit();
void GamesysInit();
void SaveManagerInit();

// term
void WrapperTerm();
void WinGuiTerm();
void BrowserTerm();
void PatcherTerm();
void StringHashTerm();
void NetPropertiesTerm();
void GamesysTerm();
void SaveManagerTerm();