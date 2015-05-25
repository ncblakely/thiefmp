#pragma once

interface IDarkOverlay;

// Element type flags
#define ElementTypeText 0x1
#define ElementTypeButton 0x2
#define ElementTypeImage 0x4
#define ElementTypeEdit 0x8

// Element behavior flags
#define ElementAlignRight 0x1
#define ElementAlignCenter 0x2
#define ElementPlaySelectSound 0x4

typedef void (__cdecl* DialogClickCB)(class DialogElement* element, Point pt);
typedef void (__cdecl* DialogEnterCB)(class DialogElement* element);
// Dialog classes
class DialogElement
{
public:
	DialogElement(IDarkOverlay* parent);
	virtual ~DialogElement();

	// Pure virtual methods
	virtual void Draw() = 0;
	virtual DWORD GetType() = 0;

	// Generic methods
	virtual IDarkOverlay* GetParent();
	virtual void* GetData();
	virtual void SetData(void* data);
	void SetEnabled(bool enabled);
	virtual bool HandleClick(Point pt);

protected:
	bool m_enabled;
	void* m_data;
	DWORD m_flags;
	DialogClickCB m_clickCB;
	Rect m_location; // location of element (scaled for resolution and image size)
	IDarkOverlay* m_parent;
};

class Element_Text : public DialogElement
{
public:
	Element_Text(IDarkOverlay* parent, const char* title, float x, float y, DWORD flags, DWORD color, grs_font* font);
	virtual ~Element_Text();

	virtual void Draw();
	virtual DWORD GetType() { return ElementTypeText; }

	float GetTextWidth();

	void SetText(const char* str);

	static Element_Text* Cast(DialogElement* ele) { return FlagIsSet(ele->GetType(), ElementTypeText) ? (Element_Text*)ele : NULL;}
protected:
	grs_font* m_font;
	NString m_title;
	DWORD m_textColor;
};

class Element_Button : public Element_Text
{
public:
	Element_Button(IDarkOverlay* parent, const char* title, float x, float y, DWORD flags, DWORD color, grs_font* font, DialogClickCB clickCB);

	//virtual void Draw();
	virtual DWORD GetType() { return ElementTypeButton; }

	static Element_Button* Cast(DialogElement* ele) { return FlagIsSet(ele->GetType(), ElementTypeButton) ? (Element_Button*)ele : NULL;}

protected:
};

class Element_Image : public DialogElement
{
public:
	Element_Image(IDarkOverlay* parent, const char* imageFile, const char* resFile, float x, float y, DWORD flags, DialogClickCB cb);
	virtual ~Element_Image();

	virtual DWORD GetType() { return ElementTypeImage; }
	virtual void Draw();

	static Element_Image* Cast(DialogElement* ele) { return FlagIsSet(ele->GetType(), ElementTypeImage) ? (Element_Image*)ele : NULL;}

protected:
	NString m_imageFile;
	NString m_resFile;
};

class Element_Edit : public DialogElement
{
public:
	Element_Edit(IDarkOverlay* parent, const char* title, float x, float y, DWORD flags, DWORD color, grs_font* font, int maxChars, DialogEnterCB enterCB);
	virtual ~Element_Edit();

	virtual void Draw();
	virtual DWORD GetType() { return ElementTypeEdit; }
	virtual bool HandleClick(Point pt);

	static Element_Edit* Cast(DialogElement* ele) { return FlagIsSet(ele->GetType(), ElementTypeEdit) ? (Element_Edit*)ele : NULL;}
	static void GadgetCallback(_LGadTextBox* chatBox, _LGadTextBoxEvent event, int eventID, void* data);
	void Focus(bool focus);
	float GetTextWidth();

protected:
	grs_font* m_font;
	DWORD m_textColor;
	DialogEnterCB m_enterCB;
	int m_maxChars;
	char m_textBuff[256];
	DWORD m_prevContext;
	TextboxState m_subroot;
	_LGadTextBox m_textBox;
	bool m_focused;
};