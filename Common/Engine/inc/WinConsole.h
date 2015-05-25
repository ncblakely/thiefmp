#ifndef _WINCONSOLE_H
#define _WINCONSOLE_H

#include "TimerManager.h"

typedef BOOL (WINAPI *ConsoleHandlerFunc)(DWORD dwCtrlType);
typedef void (*ExecuteCommandFunc)(const char* cmd, const char* data);
typedef void (*_CmdFunc)(...);

enum ArgType
{
	AT_NONE = 0,
	AT_BOOL = 1,
	AT_INT = 2,
	AT_FLOAT = 3,
	AT_DOUBLE = 4,
	AT_STRING = 5,
	AT_BOOLVAR = 6,
	AT_INTVAR = 7,
	AT_STRINGVAR = 8,
	AT_INTARRAY = 9,
	AT_FLOATVAR = 10,
	AT_BOOLTOGGLE = 11,
	AT_INTTOGGLE = 12,
	AT_HELP = 13,
};

struct CommandEntry
{
	const char* name;
	const char* shortName;
	ArgType argType;
	_CmdFunc	func;
	const char* helpText;
};

namespace WinConsole
{
	void Create(const char* consoleTitle, ConsoleHandlerFunc pfnHandler, ExecuteCommandFunc pfnExecute);
	TimerResult ProcessQueue();
	void Print(const char* str, ...);
	void Print(const wchar_t* str, ...);
	void PollInput();
	void Shutdown();
}

#endif