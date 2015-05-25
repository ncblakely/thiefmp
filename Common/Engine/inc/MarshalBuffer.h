#pragma once

class CMarshalBuffer
{
public:
	CMarshalBuffer();

	void CopyByte(BYTE b);
	void CopyWord(WORD w);
	void CopyInt(int i);
	void CopyVoid(const void* pData, int size);
	void CopyString(const char*, int srcChars);

	int GetBytesWritten();
	BYTE* GetStartBuffer();

	void GetVoid(void* pBuffer, int size);
	int GetInt();
	BYTE GetByte();
	WORD GetWord();
	float GetFloat();
	void GetString(char* pBuffer, int destChars);

	void SkipBytes(int bytes);

	void SetBuffer(BYTE*, int buffSize);
	void UseDefaultBuffer();

protected:
	BYTE* m_pStartBuffer;
	int m_buffSize;
	BYTE* m_pCurrBuffer;
	int m_bytesWritten;

	static BYTE m_DefaultBuffer[1024];
};