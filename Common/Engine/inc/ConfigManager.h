#pragma once

#include "Crc32.h"
#include "IniFile.h"
#include <list>
#include "crtdbg.h"

#define _ASSERTVARTYPES

#ifdef _ASSERTVARTYPES
#define AssertType(a) assert(a)
#else
#define AssertType(a)
#endif

class ConfigVar
{
	friend class ConfigManager;

protected:
	union Var
	{
		int m_int;
		float m_float;
		char* m_string;
	};

	enum CVarType
	{
		CV_Int,
		CV_Float,
		CV_String,
	};

	Var m_Var;
	CVarType m_Type;
	DWORD m_nameHash;

public:
	ConfigVar(const char* varName, CVarType type) 
	{ 
		ZeroMemory(&m_Var, sizeof(Var));

		m_nameHash = Crc32::CreateID(varName);
		m_Type = type;
	}

	~ConfigVar() 
	{ 
		if (m_Type == CV_String && m_Var.m_string)
			delete[] m_Var.m_string;
	}

	inline float GetFloat() const
	{ 
		AssertType(m_Type == CV_Float); 
		return m_Var.m_float; 
	}

	inline int GetInt() const 
	{ 
		AssertType(m_Type == CV_Int);
		return m_Var.m_int; 
	}

	inline const char* GetStr() const
	{
		AssertType(m_Type == CV_String);
		return m_Var.m_string;
	}

	inline DWORD GetNameHash() const
	{
		return m_nameHash;
	}

	void SetFloat(const float f) 
	{ 
		AssertType(m_Type == CV_Float);
		m_Var.m_float = f; 
	}

	void SetInt(const int i)
	{
		AssertType(m_Type == CV_Int);
		m_Var.m_int = i;
	}

	void SetStr(const char* str) 
	{ 
		AssertType(m_Type == CV_String); 

		if (m_Var.m_string)
			delete[] m_Var.m_string;

		m_Var.m_string = new char[strlen(str) + 1]; 
		strcpy(m_Var.m_string, str);
	}
};

class ConfigManager
{
public:
	ConfigManager();
	ConfigManager(const char* iniPath);
	virtual ~ConfigManager();

	// Delete all config variables
	void ClearVariables();

	// Sets the .ini file to read from and initializes the reader
	void SetIniFile(const char* iniPath);

	// Reads config variables from the selected .ini file
	void ReadInt(const char* appName, const char* keyName, int defaultVal, bool bCreateIfMissing) { _ASSERT(!FindVariableByName(keyName)); SetInt(keyName, m_Reader->GetInt(appName, keyName, defaultVal, bCreateIfMissing)); }
	void ReadFloat(const char* appName, const char* keyName, float defaultVal, bool bCreateIfMissing) { _ASSERT(!FindVariableByName(keyName)); SetFloat(keyName, m_Reader->GetFloat(appName, keyName, defaultVal, bCreateIfMissing)); }
	void ReadString(const char* appName, const char* keyName, int size, const char* defaultStr, bool bCreateIfMissing) { _ASSERT(!FindVariableByName(keyName)); SetString(keyName, m_Reader->GetString(appName, keyName, size, defaultStr, bCreateIfMissing)); }
	void ReadBool(const char* appName, const char* keyName, bool bDefaultVal, bool bCreateIfMissing) { _ASSERT(!FindVariableByName(keyName)); SetInt(keyName, (int)m_Reader->GetBool(appName, keyName, bDefaultVal, bCreateIfMissing)); }

	// Gets loaded config variables
	int GetInt(const char* varName);
	float GetFloat(const char* varName);
	const char* GetString(const char* varName);
	bool GetBool(const char* varName);

	// Sets an existing config variable, or creates it if it does not exist
	void SetInt(const char* varName, int i);
	void SetFloat(const char* varName, float f);
	void SetString(const char* varName, const char* s);

protected:
	inline ConfigVar* CreateIntVar(const char* varName) { return new ConfigVar(varName, ConfigVar::CV_Int); }
	inline ConfigVar* CreateFloatVar(const char* varName) { return new ConfigVar(varName, ConfigVar::CV_Float); }
	inline ConfigVar* CreateStringVar(const char* varName) { return new ConfigVar(varName, ConfigVar::CV_String); }

	ConfigVar* FindVariableByName(const char* varName);
	ConfigVar* FindVariableByHash(DWORD nameHash);

	IniFile* m_Reader;

	typedef std::list<ConfigVar*> VarList;
	VarList m_Vars;
};