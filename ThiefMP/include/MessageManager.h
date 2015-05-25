#pragma once

#include "Engine\inc\Engine.h"
#include "..\GlobalServer\Shared\CriticalSection.h"

const int kMaxMessageLength = 110;

enum eFont
{
	MFONT_TextFont,
	MFONT_SmallFont,
};

enum MessageColor
{
	CLR_White = 0xFFFF,
	CLR_Green = 0x3CA2,
	CLR_Gray = 0x94B2,

	// SS2 player chat colors
	CLR_Red = 0xFCB2, // Player 1
	CLR_Blue = 0x94BF, // Player 2
	CLR_Purple = 0xFCBF, // Player 3
	CLR_Yellow = 0xFFF2 // Player 4
};

class Message
{
public:
	Message(const char* msgText, float expireTime);
	virtual ~Message();

	float GetExpireTime() const { return m_expireTime; }
	const char* GetText() const { return &m_text[0]; }

protected:
	char* m_text;
	float m_expireTime;
};

class MessageMgr
{
public:
	MessageMgr();
	virtual ~MessageMgr();

	void	 AddLine(bool bPlaySound, const char* message);
	void	 AddLineFormat(bool bPlaySound, const char* message, ...);
	void	 ClearMessages();
	void EnableRendering(bool bEnable);
	void DrawInput();
	void	Update();

	static MessageMgr* Get();
	static void Shutdown();

protected:
	void	 DeleteTopmost();
	IRes* FontFromType(eFont fontType);

	bool m_renderEnabled;
	CCriticalSection m_cs;
	int m_textColor;

	static const int kMaxMessages = 35;
	typedef std::list<Message*> MessageList;
	MessageList m_Messages;
};