#pragma once

typedef void (*LogCB)(void* text);

enum LogPrefix
{
	Pfx_Log,
	Pfx_Init,
	Pfx_Warning,
	Pfx_Error,
	Pfx_FatalError,
};

class Logger
{
public:
	Logger();
	Logger(const char* file);
	~Logger();

	static void PrintFile(const char* logFile, const char* str, ...);
	static void PrintFile(const char* logFile, const wchar_t* str, ...);

	void Print(LogPrefix prefix, const wchar_t* str, ...);
	void Print(LogPrefix prefix, const char* str, ...);
	void Print(const char* str, ...);
	void Print(const wchar_t* str, ...);
	bool Close();
	bool Open(const char* fileName, bool overwrite = false);
	void SetLogCallback(LogCB pCB);
	void EnableTimestamps(bool bEnable);

protected:
	void DoPrint(LogPrefix prefix, const wchar_t* str);
	void DoPrint(LogPrefix prefix, const char* str);
	void PrintPrefix(LogPrefix prefix);

	FILE* m_file;
	bool m_useTimestamps;
	LogCB m_LogCallback;
};

extern Logger Log;