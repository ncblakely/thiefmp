#pragma once

#include "DialogElement.h"

#define MAX_OVERLAYS 8

void OnAutomap();
long __stdcall OnReadBook(DWORD, const char* text, const char* background);
short HookKeyboardInput(_kbs_event& event, short& cooked, BYTE& P3, int zero);

IRes* LoadPCX(const char* file, const char* crf, bool forcePaletteLoad = false);
IRes* LoadPCXSimple(const char* fileName, const char* resName);
void FreePCX(IRes* & pcx);
void DrawByHandle(IRes* pRes, Point& pt);

interface IDarkOverlay
{
	virtual DWORD GetID() = 0;
	virtual const char* GetName() = 0;
	virtual void Draw() = 0;
	virtual bool Init() = 0;
	virtual bool Term() = 0;
	virtual void SetEnable(bool bEnable) = 0;
	virtual bool IsEnabled() = 0;
	virtual void OnLeftClick(Point pt) = 0;
	virtual const Rect& GetDrawingArea() = 0;
};

////////////////////////////////////////

class DarkOverlay : public IDarkOverlay
{
public:
	DarkOverlay(const char* overlayName);
	virtual ~DarkOverlay() { }

	virtual DWORD GetID();
	virtual const char* GetName() { return m_name; }
	virtual void Draw();
	virtual bool Init();
	virtual bool Term();
	virtual void SetEnable(bool bEnable);
	virtual bool IsEnabled();
	virtual void OnLeftClick(Point pt) { }
	virtual const Rect& GetDrawingArea() { return m_drawRect; }

	Element_Text* AddText(const char* title, float x, float y, DWORD flags, DWORD color, grs_font* font);
	Element_Button* AddButton(const char* title, float x, float y, DWORD flags, DWORD color, grs_font* font, DialogClickCB cb);
	Element_Image* AddImage(const char* imageFile, const char* resFile, float x, float y, DWORD flags, DialogClickCB cb);
	Element_Edit* AddEdit(const char* title, float x, float y, DWORD flags, DWORD color, grs_font* font, int maxChars, DialogEnterCB cb);
protected:
	bool PointInOverlay(Point pt);
	Rect ScaleRect(const Rect& oldRect, const Rect& oldPanelRect);
	void CenterRect(Rect& rectInner, Rect& rectOuter);
	void CenterInOverlay(Rect& r);
	char m_name[16];
	DWORD m_hash;
	Rect m_drawRect; // main drawing area, centered and adjusted for resolution
	bool m_enabled;
	bool m_initialized; // Init() has been called

	std::vector<DialogElement*> m_Elements; // list of dialog elements (buttons, text, images)
};

class PlayersOverlay : public DarkOverlay
{
public:
	PlayersOverlay(const char* overlayName);
	virtual ~PlayersOverlay();

	virtual void Draw();
	virtual bool Init();
	virtual void SetEnable(bool bEnable);
	virtual void OnLeftClick(Point pt);

	void UpdatePlayers();

	static void OnEndGameClicked(DialogElement* element, Point pt);
	static void OnAddTimeClicked(DialogElement* element, Point pt);
	static void OnKickPlayerClicked(DialogElement* element, Point pt);

protected:
	float m_playerUpdateTime;
	Element_Text* m_txtPlayerNames[kMaxPlayers];
	Element_Text* m_txtPings[kMaxPlayers];
	Element_Image* m_imgKicks[kMaxPlayers];
	Element_Text* m_txtServer;
	Element_Text* m_txtMission;
	Element_Text* m_txtDamagePlayers;
	Element_Text* m_txtCheats;
	Element_Text* m_txtIncreaseDifficulty;
	Element_Text* m_txtRespawn;
	Element_Text* m_txtSaveLimit;
	Element_Text* m_txtTimeLimit;

	static Rect m_Rect;
};

class StagingOverlay : public PlayersOverlay
{
public:
	StagingOverlay(const char* overlayName);
	virtual ~StagingOverlay();

	virtual void Draw();
	virtual bool Init();
	virtual void SetEnable(bool bEnable);
	virtual void OnLeftClick(Point pt);

	void UpdatePlayers();

	static void OnLoadMission(DialogElement* element, Point pt);
	static void OnQuit(DialogElement* element, Point pt);
protected:
	void UpdateSettings();
};

void StagingDisplay();
void StagingClose();
bool IsStagingMode();

class ObjectivesOverlay : public DarkOverlay
{
public:
	ObjectivesOverlay(const char* overlayName);
	virtual ~ObjectivesOverlay();

	virtual void Draw();
	virtual bool Init();
	virtual bool Term();
	virtual void SetEnable(bool bEnable);
protected:
	void DrawGoalStatus(int goalState, grs_font* font, int ypos);
	int GetGoalSpacing(grs_font* font);
	bool DisplayFits(const Rect& rect, grs_font* font);

	cAnsiStr m_strMissName;
	cAnsiStr m_strDifficulty;
	cDynArray<Rect> m_UIRects;
	IPtr<IQuestData> m_pQuestData;
	guiStyle m_PanelStyle;
	guiStyle m_GoalTextStyle; // style at 0xC4 in original UI
	guiStyle m_GreyStyle; // style at 0x114 in original UI

	static const int kGoalTextStatusBorder = 20; // distance between goal status image and text
	static Rect m_Rect;
};

class MapOverlay : public DarkOverlay
{
public:
	MapOverlay(const char* overlayName);
	virtual ~MapOverlay();

	virtual void Draw();
	virtual bool Init();
	virtual bool Term();
	virtual void OnLeftClick(Point pt);
	virtual void SetEnable(bool bEnable);

	void UpdateLocation();
	void RefreshPage();
	void LoadMapImage();

protected:
	IRes* m_pBackgroundImg;
	IRes* m_pMapImg;
	IPtr<IQuestData> m_pQuestData;
	int m_currentPage;
	int m_minPage;
	int m_maxPage;
	int m_currentLocation;

	static Rect m_Rect;
};

class BookOverlay: public DarkOverlay
{
public:
	BookOverlay(const char* overlayName);
	virtual ~BookOverlay();

	virtual void Draw();
	virtual bool Init();
	virtual bool Term();
	virtual void SetEnable(bool bEnable);
	virtual void OnLeftClick(Point pt);

	void DestroyBook();
	void LoadBook(const char* background, const char* text);
	void SetupPages();

protected:
	IRes* m_pBookFont;
	IRes* m_pBookBackgrounds[4];
	IRes* m_pBookImg;
	IRes* m_pTextRes;
	cStringResource* m_pTextStringRes;
	cDynArray<Rect> m_TextRects;
	cDynArray<Rect> m_PageRects;
	Rect m_rectNextPage; // rect for next page button, adjusted for resolution
	Rect m_rectPrevPage; // rect for previous page button, adjusted for resolution
	const char* m_backgroundImgFile;
	const char* m_textFile;
	int m_currentPage;
	int m_firstPage;
	int m_lastPage;
	grd_canvas m_TheCanvas;
	grs_bitmap* m_pBitmap;

	static Rect m_Rect;
};

class SimOverlay: public DarkOverlay
{
public:
	SimOverlay(const char* overlayName);
	virtual ~SimOverlay();

	virtual void Draw();
	virtual bool Init();
	virtual bool Term();
	virtual void SetEnable(bool bEnable);
	//virtual void OnLeftClick(Point pt);

protected:
	IRes* m_pBackgroundImg;
	cDynArray<Rect> m_ButtonRects;
	guiStyle m_Style;
	grd_canvas m_TheCanvas;
	grs_bitmap* m_pBitmap;

	static Rect m_Rect;
};

class DarkOverlayManager
{
public:
	DarkOverlayManager();
	virtual ~DarkOverlayManager();

	bool AnyEnabled();
	void Add(IDarkOverlay* pOverlay);
	void ToggleOverlay(const char* overlayName);
	void EnableMouseMode(bool bEnable);
	void EnableOverlay(const char* overlayName, bool bEnable);
	bool OverlayEnabled(const char* overlayName);
	void OnFrame();
	void DrawCursor();
	void DrawAll();
	void InitAll();
	void DisableAll();
	void TermAll();
	IDarkOverlay* FindByName(const char* overlayName);
	bool HandleClick();

protected:
	IRes* m_pCursorImg;
	IDarkOverlay* m_pOverlays[MAX_OVERLAYS];
	guiStyle m_DefaultStyle;
	bool m_bInMouseMode;
};

extern DarkOverlayManager* g_pOverlayMgr;