#pragma once

class NString
{
public:
	NString();
	NString(const NString& rhs);
	NString(int length);
	NString(const char* str);
	NString(const wchar_t* str);
	~NString();

	// Operators
	const NString& operator = (const char* str);
	const NString& operator = (const NString& rhs);
	inline const NString& NString::operator += (const char* str)
	{
		if (str) Append(str);
		return *this;
	}

	bool operator == (const NString& rhs);
	char& operator [] (int pos);
	char& operator [] (int pos) const;

	// Assignment
	void Assign(const char* str);
	void Assign(int, const char* str);

	// Modification
	void Append(const char* str);
	void Erase(int startPos, int length);

	// Comparison (default comparison is done case insensitively)
	bool Compare(const NString& rString);
	bool CompareCase(const NString& rString);

	bool Empty() const;
	int Length() const;

	// Formatting
	void Format(const char* fmt, ...);

	void PreAllocate(int length);

	// Conversion operations
	const char* Str() const { return m_pBuffer; }
	operator const char* () const { return Str(); }

protected:

	// Buffer memory manipulation
	void Allocate(int length);
	void FreeBuffer();
	char* m_pBuffer; // pointer to internal string buffer
	int m_dataLength; // size of string buffer
	int m_stringLength; // size of string currently in buffer, not including terminating null

	static const char* m_nullString;

	static const int kMaxFormatStringSize = 512; // Maximum size of a formatted string
};

