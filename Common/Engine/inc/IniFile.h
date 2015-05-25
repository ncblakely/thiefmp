#pragma once

class IniFile
{
public:
	IniFile(const char* iniPath);
	IniFile();

	float GetFloat(const char* appName, const char* keyName, float defaultVal, bool bCreateIfMissing);
	double	GetDouble(const char* appName, const char* keyName, double defaultVal, bool bCreateIfMissing);
	bool GetBool(const char* appName, const char* keyName, bool bDefaultVal, bool bCreateIfMissing);	
	int GetInt(const char* appName, const char* keyName, int defaultVal, bool bCreateIfMissing);
	const char* GetString(const char* appName, const char* keyName, int size, const char* defaultStr, bool bCreateIfMissing);

	bool SetFile(const char* iniPath);

	bool WriteString(const char* appName, const char* keyName, const char* str);
	bool WriteInt(const char* appName, const char* keyName, int val);

protected:
	const char* m_file;
	char m_sStringBuff[MAX_PATH];
};