#pragma once

void DarkChatInit();
void DarkChatTerm();

void ChatboxOpen();
bool ChatboxIsOpen();

extern char g_chatBuffer[256];
extern _LGadTextBox g_chatBox;