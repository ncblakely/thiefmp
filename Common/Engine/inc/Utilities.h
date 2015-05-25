#pragma once

#include <vector>

#define countof(array) (sizeof((array)) / sizeof((array)[0]))

#define FlagSet(w,f) ((w) |= (f))
#define FlagClear(w,f) ((w) &= ~(f))
#define FlagFlip(w,f) ((w) ^= (f))
#define FlagIsSet(w,f) ((w) & (f))
#define FlagIsClear(w,f) (!FlagIsSet(w,f))

std::string GetParam(std::string, char, uint);
std::wstring StrToEnd(const std::wstring& str, uint position);
std::string StrToEnd(const std::string& str, uint position);
bool	atobool(const char*);
void AnsiToWide(wchar_t* wstrDestination, const char* strSource, int cchDestChar);
void WideToAnsi(char* strDestination, const WCHAR* wstrSource, int cchDestChar);

const char* StringEnd(char* str, int pos);
void safe_strcpy(char* dest, const char* src, int chars);
void safe_snprintf(char* dest, int chars, const char* src, ...);

namespace Utils
{
	void MessageBoxF(HWND hWnd, const char* caption, const char* format, ...);
	bool GetLibVersion(const char* fileName, int& majorVersion, int& minorVersion, int& buildNumber, int& revisionNumber);
	bool FileExists(const char* path);
}

class FileHandle
{
public:
	~FileHandle() 
	{ 
		if (m_pFile)
			fclose(m_pFile); 
	}

	FileHandle& operator = (FILE* f) { m_pFile = f; return *this; }

	operator FILE*() { return m_pFile; }
protected:
	FILE* m_pFile;
};

// Generic pointer template to make the syntax for dereferencing multiple levels of indirection a bit less cumbersome
template <typename T>
class IPtr
{
public:
	IPtr() { m_interface = NULL; }
	IPtr(void* iface) { m_interface = (T*)iface;}
	~IPtr() { if (m_interface) m_interface->Release(); }

	T* operator->() const { return m_interface; }
	operator T*() const { return m_interface; }
	const IPtr& operator=(void* iface) { m_interface = (T*)iface; return *this;}

protected:
	T* m_interface;
};

// Vector append
template <typename T>
std::vector<T>& operator += (std::vector<T>& v1, std::vector<T>& v2)
{
	v1.insert(v1.end(), v2.begin(), v2.end());
	return v1;
}
