#pragma once

void ChatboxCallback(struct _LGadTextBox* pTextBox, enum _LGadTextBoxEvent, int eventID, void*);

void ConPrintF(const char* str, ...);
void ConPrintEchoF(const char* str, ...);
void InitNetCommands();

extern struct ConsoleCommand ConsoleCmds[];
extern int g_numCommands;