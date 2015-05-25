/*************************************************************
* File: DarkOverlay.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implementation of screen overlay UI classes.
*************************************************************/

#include "stdafx.h"

#include "Engine\inc\CRC32.h"

#include "Main.h"
#include "DarkOverlay.h"
#include "Client.h"
#include "DarkUIUtils.h"

// left, top, right, bottom
//Rect MapOverlay::m_Rect = { 2, 124, 638, 424 };
Rect MapOverlay::m_Rect = { 0, 0, 552, 360};
Rect BookOverlay::m_Rect = { 0, 0, 640, 480};
Rect SimOverlay::m_Rect = { 0, 0, 640, 480};
Rect ObjectivesOverlay::m_Rect = { 0, 0, 640, 480};
Rect PlayersOverlay::m_Rect = { 0, 0, 640, 480};

//#define OBJECTIVES_DRAW_BACKGROUND

IRes* LoadPCX(const char* file, const char* crf,
				  bool forcePaletteLoad) // 7/31/10 - some images (such as cursor for overlay mgr) won't display correctly without this
{
	IPtr<IResMan> pResMan = _AppGetAggregated(IID_IResMan);
	IRes* pRes = pResMan->Bind(file, "Image", NULL, crf, 0);
	IRes* pRetypedRes;

	if (pRes)
	{
		ImageResource* res = (ImageResource*)pRes->Lock();

		if (forcePaletteLoad || !res->palette)
		{
			pRetypedRes = pResMan->Retype(pRes, "Palette", NULL);
			if (pRetypedRes)
			{
				res->palette = _palmgr_alloc_pal(pRetypedRes->Lock());
				pRetypedRes->Unlock();
				pRetypedRes->Release();
			}
		}
	}

	return pRes;
}

IRes* LoadPCXSimple(const char* fileName, const char* resName)
{
	IPtr<IResMan> pResMan = _AppGetAggregated(IID_IResMan);

	IRes* pRes = pResMan->Bind(fileName, "Image", NULL, resName, 0);

	return pRes;
}

void FreePCX(IRes* & pPCX)
{
	if (pPCX)
	{
		pPCX->Unlock();
		pPCX->Release();

		pPCX = NULL;
	}
}

void DrawByHandle(IRes* pRes, Point& pt)
{
	if (pRes)
	{
		ImageResource* res = (ImageResource*)pRes->Lock();
		_gd_bitmap((void*)res, pt.x, pt.y);
		pRes->Unlock();
	}
}

static bool PointInRect(Point pt, Rect& r)
{
	return pt.y >= r.top && pt.y < r.bottom && pt.x >= r.left && pt.x < r.right;
}

void DarkOverlay::Draw()
{
	GUIsetupStruct gss;

	_GUIsetup(&gss, &m_drawRect, 2, 0);
	for (uint i = 0; i < m_Elements.size(); i++) m_Elements[i]->Draw();
	_GUIdone(&gss);
}

Element_Text* DarkOverlay::AddText(const char* title, float x, float y, DWORD flags, DWORD color, grs_font* font)
{
	Element_Text* ele = new Element_Text(this, title, x, y, flags, color, font);
	m_Elements.push_back(ele);
	return ele;
}

Element_Image* DarkOverlay::AddImage(const char* imageFile, const char* resFile, float x, float y, DWORD flags, DialogClickCB cb)
{
	Element_Image* ele = new Element_Image(this, imageFile, resFile, x, y, flags, cb);
	m_Elements.push_back(ele);
	return ele;
}

Element_Button* DarkOverlay::AddButton(const char* title, float x, float y, DWORD flags, DWORD color, grs_font* font, DialogClickCB cb)
{
	Element_Button* ele = new Element_Button(this, title, x, y, flags, color, font, cb);
	m_Elements.push_back(ele);
	return ele;
}

Element_Edit* DarkOverlay::AddEdit(const char* title, float x, float y, DWORD flags, DWORD color, grs_font* font, int maxChars, DialogEnterCB cb)
{
	Element_Edit* ele = new Element_Edit(this, title, x, y, flags, color, font, maxChars, cb);
	m_Elements.push_back(ele);
	return ele;
}


DarkOverlayManager* g_pOverlayMgr = NULL;

DarkOverlayManager::DarkOverlayManager()
{
	ZeroMemory(m_pOverlays, sizeof(m_pOverlays));

	Add(new PlayersOverlay("overlay_players"));
	Add(new BookOverlay("overlay_book"));
	Add(new MapOverlay("overlay_map"));
	Add(new SimOverlay("overlay_sim"));
	Add(new ObjectivesOverlay("overlay_objectives"));
	Add(new StagingOverlay("overlay_staging"));

	m_pCursorImg = LoadPCX("cursor", "intrface", true);
	m_bInMouseMode = false;
}

DarkOverlayManager::~DarkOverlayManager()
{
	for (int i = 0; i < MAX_OVERLAYS; i++)
		SAFE_DELETE(m_pOverlays[i]);

	FreePCX(m_pCursorImg);
}

void DarkOverlayManager::EnableOverlay(const char* overlayName, bool bEnable)
{
	IDarkOverlay* pOverlay = FindByName(overlayName);
	dbgassert(pOverlay);

	if (bEnable && IsStagingMode())
		return;

	if (pOverlay->IsEnabled() != bEnable)
		pOverlay->SetEnable(bEnable);
}

IDarkOverlay* DarkOverlayManager::FindByName(const char* overlayName)
{
	DWORD hash = Crc32::CreateID(overlayName);

	for (int i = 0; i < MAX_OVERLAYS; i++)
	{
		if (m_pOverlays[i] && m_pOverlays[i]->GetID() == hash)
			return m_pOverlays[i];
	}

	return NULL;
}

bool DarkOverlayManager::OverlayEnabled(const char* overlayName)
{
	IDarkOverlay* pOverlay = FindByName(overlayName);

	if (pOverlay)
		return pOverlay->IsEnabled();

	return false;
}

void DarkOverlayManager::ToggleOverlay(const char* overlayName)
{
	IDarkOverlay* pOverlay = FindByName(overlayName);
	dbgassert(pOverlay);

	if (IsStagingMode())
		return;

	if (pOverlay)
	{
		bool bEnabled = pOverlay->IsEnabled();

		pOverlay->SetEnable(bEnabled ? false : true);
	}
}

void DarkOverlayManager::Add(IDarkOverlay* pOverlay)
{
	for (int i = 0; i < MAX_OVERLAYS; i++)
	{
		if (!m_pOverlays[i])
		{
			m_pOverlays[i] = pOverlay;
			return;
		}
	}

	assert(false && "out of overlays");
}

void DarkOverlayManager::InitAll()
{
	for (int i = 0; i < MAX_OVERLAYS; i++)
	{
		if (m_pOverlays[i] )
		//&& m_pOverlays[i]->IsEnabled()) // 8/8/2010
			m_pOverlays[i]->Init();
	}
}

void DarkOverlayManager::DrawAll()
{
	for (int i = 0; i < MAX_OVERLAYS; i++)
	{
		if (m_pOverlays[i] && m_pOverlays[i]->IsEnabled())
			m_pOverlays[i]->Draw();
	}
}

void DarkOverlayManager::DisableAll()
{
	for (int i = 0; i < MAX_OVERLAYS; i++)
	{
		if (m_pOverlays[i])
			m_pOverlays[i]->SetEnable(false);
	}
}

bool DarkOverlayManager::AnyEnabled()
{
	for (int i = 0; i < MAX_OVERLAYS; i++)
	{
		if (m_pOverlays[i])
		{
			if (m_pOverlays[i]->IsEnabled())
				return true;
		}
	}

	return false;
}

void DarkOverlayManager::TermAll()
{
	for (int i = 0; i < MAX_OVERLAYS; i++)
	{
		if (m_pOverlays[i])
		//&& m_pOverlays[i]->IsEnabled()) // 8/8/2010
			m_pOverlays[i]->Term();
	}
}

void DarkOverlayManager::OnFrame()
{
	DrawAll();
	DrawCursor();
}

void DarkOverlayManager::EnableMouseMode(bool bEnable)
{
	if (bEnable)
	{
		// Stop head movement
		_headmoveSetRelPosX(0);
		_headmoveSetRelPosY(0);

		//_RemoveIBHandler();
		//_InstallIBHandler(0x20, 0x1D, 0);

		g_pInputBinder->SetValidEvents(-9);
		*_gNoMoveKeys = 1;
		m_bInMouseMode = true;
	}
	else
	{
		g_pInputBinder->SetValidEvents(-1);

		// Re-center the cursor
		if (m_bInMouseMode)
		{
			sScreenMode mode = GetScreenMode();
			_mouse_put_xy(mode.width / 2, mode.height / 2);
		}

		*_gNoMoveKeys = 0;
		m_bInMouseMode = false;
	}
}

void DarkOverlayManager::DrawCursor()
{
	if (m_bInMouseMode)
	{
		short x, y;
		_mouse_get_xy(&x, &y);

		DrawByHandle(m_pCursorImg, Point(x, y));
	}
}

// Returns true if overlay handled click
bool DarkOverlayManager::HandleClick()
{
	if (m_bInMouseMode)
	{
		short x, y;
		_mouse_get_xy(&x, &y);

		for (int i = 0; i < MAX_OVERLAYS; i++)
		{
			if (m_pOverlays[i] && m_pOverlays[i]->IsEnabled())
			{
				m_pOverlays[i]->OnLeftClick(Point(x, y));
				return true;
			}
		}
	}

	return false;
}

///////////////////////////////////////////////////////

DarkOverlay::DarkOverlay(const char* overlayName)
{
	m_hash = Crc32::CreateID(overlayName);
	m_enabled = false;
	m_initialized = false;
	ZeroMemory(&m_drawRect, sizeof(Rect));

	safe_strcpy(m_name, overlayName, sizeof(m_name) - 1);
}

void DarkOverlay::SetEnable(bool bEnable)
{
	m_enabled = bEnable;

	if (_SimStateCheckFlags(0x8000) && !m_initialized)
		Init(); // if in game mode and not initialized yet, do it now
}

bool DarkOverlay::IsEnabled()
{
	return m_enabled;
}

DWORD DarkOverlay::GetID()
{
	return m_hash;
}

bool DarkOverlay::PointInOverlay(Point pt)
{
	return PointInRect(pt, m_drawRect);
}

void DarkOverlay::CenterRect(Rect& rectInner, Rect& rectOuter)
{
	sScreenMode mode;

	_ScrnModeGet(&mode);

	int rectWidth = rectOuter.right - rectOuter.left;
	int rectHeight = rectOuter.bottom - rectOuter.top;

	rectInner.top = (mode.height - rectHeight) / 2;
	rectInner.left = (mode.width - rectWidth) / 2;
	rectInner.right = rectInner.left + rectWidth;
	rectInner.bottom = rectInner.top + rectHeight;
}

void DarkOverlay::CenterInOverlay(Rect &r)
{
	CenterRect(r, m_drawRect);
}

Rect DarkOverlay::ScaleRect(const Rect& oldRect, const Rect& oldPanelRect)
{
	Rect newRect;

	newRect.left = oldRect.left + m_drawRect.left;
	newRect.top = oldRect.top + m_drawRect.top;
	newRect.right = m_drawRect.right - (oldPanelRect.right - oldRect.right);
	newRect.bottom = m_drawRect.bottom - (oldPanelRect.bottom - oldRect.bottom);

	return newRect;
}

bool DarkOverlay::Init()
{
	if (m_enabled && !m_initialized)
	{
		m_drawRect = GetScreenRect();
		m_initialized = true;
		return true;
	}
	
	return false;
}

bool DarkOverlay::Term()
{
	if (m_initialized)
	{
		for (uint i = 0; i < m_Elements.size(); i++)
			delete m_Elements[i];
		m_Elements.clear();
		m_initialized = false;

		return true;
	}

	return false;
}

PlayersOverlay::PlayersOverlay(const char* overlayName) : DarkOverlay(overlayName)
{
	//IResMan* pResMan = (IResMan*)_AppGetAggregated(IID_IResMan);

	//m_textFont = pResMan->Bind("textfont", "Font", NULL, "intrface", NULL);
	//m_smallFont = pResMan->Bind("smalfont", "Font", NULL, "intrface", NULL);

	//pResMan->Release();
}

PlayersOverlay::~PlayersOverlay()
{
	//SAFE_RELEASE(m_textFont);
	//SAFE_RELEASE(m_smallFont);
}

void PlayersOverlay::Draw()
{
	// Close if player is dead
	if (_GetPlayerMode() == MODE_Dead)
	{
		SetEnable(false);
		return;
	}

	UpdatePlayers();
	DarkOverlay::Draw();

	CamApplyAlpha(0.50f, 0);

}

void PlayersOverlay::OnLeftClick(Point pt)
{
	for (uint i = 0; i < m_Elements.size(); i++)
		if (m_Elements[i]->HandleClick(pt))
			return;
}

void PlayersOverlay::UpdatePlayers()
{
	int players[kMaxPlayers];
	DPN_CONNECTION_INFO ci;
	NString str;

	GetPlayers(players);

	if (g_DeltaFrame.GetElapsedTime() < m_playerUpdateTime)
		return;

	if (!g_pDarkNet)
		return; // Might be null if game has ended

	for (int i = 0; i < kMaxPlayers; i++)
	{
		const char* playerName = g_pNetMan->GetPlayerNameByNum(i + 1);
		m_txtPlayerNames[i]->SetText(playerName); //(g_pNetMan->GetPlayerName(players[i]));

		if (players[i])
		{
			if (i != 0) // don't need to get the ping for host
			{
				DPNID dpnid = g_pNetMan->ToNetPlayerID(g_pObjNet->ObjHostPlayer(players[i]));
				HRESULT hRes = g_pDarkNet->GetConnectionInfo(dpnid, &ci);
				if (hRes == DPN_OK)
					str.Format("%d", ci.dwRoundTripLatencyMS);
				else
					str.Format("Error: 0x%08x", hRes);

				m_txtPings[i]->SetText(str);
			}
		}

#if 1
		m_txtPlayerNames[i]->SetEnabled(players[i] ? true : false);
		m_txtPings[i]->SetEnabled(players[i] ? true : false);
		m_imgKicks[i]->SetEnabled((i != 0 && players[i] && g_pNetMan->AmDefaultHost()) ? true : false);
#endif

		m_playerUpdateTime = g_DeltaFrame.GetElapsedTime() + 1.0f;
	}
}

bool PlayersOverlay::Init()
{
	NString strTitle, strTemp;
	m_playerUpdateTime = 0;

	if (!DarkOverlay::Init())
		return false;

	m_drawRect.right = 640;
	m_drawRect.bottom = 480;
	CenterRect(m_drawRect, m_Rect);

	sMissionData* pMissionData = _GetMissionData();
	strTemp.Format("title_%d", pMissionData->missNumber);

	//strTitle.Format("%s - %s", wstos(g_pDarkNet->GetSessionManager()->GetServerName()).c_str(), FetchUIString("titles", strTemp, "strings").m_pString);

	AddText(strTitle, 0.50f, 0.05f, ElementAlignCenter, CLR_White, g_TextFont);
	AddText("Players", 0.10f, 0.1f, 0, CLR_White, g_TextFont);
	AddText("Ping", 0.45f, 0.1f, 0, CLR_White, g_TextFont);

	// Add player names
	float y = 0.15f;
	for (int i = 0; i < countof(m_txtPlayerNames); i++)
	{
		NString str;

		str.Format("Player %d", i);
		m_txtPlayerNames[i] = AddText(str, 0.10f, y, 0, i == 0 ? CLR_Blue : CLR_Red, g_SmallFont);
		y += 0.02f;
	}

	// Add player pings
	y = 0.15f;
	for (int i = 0; i < countof(m_txtPings); i++)
	{
		NString str;

		m_txtPings[i] = AddText("0", 0.45f, y, 0, CLR_White, g_SmallFont);
		y += 0.02f;
	}

	// Add kick buttons
	y = 0.15f;
	for (int i = 0; i < countof(m_txtPings); i++)
	{
		NString str;

		m_imgKicks[i] = AddImage("goali003", "intrface", 0.04f, y, 0, OnKickPlayerClicked);
		m_imgKicks[i]->SetData(new int(i + 1));
		y += 0.02f;
	}

	if (g_pNetMan->AmDefaultHost())
	{
		y = 0.10f;
		AddText("Host Options", 0.70f, y, 0, CLR_White, g_TextFont);
		y += 0.05f;
		AddButton("Add Time to Clock", 0.7f, y, ElementPlaySelectSound, CLR_Yellow, g_TextFont, OnAddTimeClicked);
		//y += 0.05f;
		//AddButton("End Mission", 0.7f, y, ElementPlaySelectSound, CLR_Yellow, g_TextFont, OnEndGameClicked);
	}

	return true;
}

void PlayersOverlay::SetEnable(bool bEnable)
{
	if (m_enabled == bEnable || _GetPlayerMode() == MODE_Dead || g_pOverlayMgr->OverlayEnabled("overlay_staging"))
		return;

	// Close other overlays
	g_pOverlayMgr->EnableOverlay("overlay_book", false);
	g_pOverlayMgr->EnableOverlay("overlay_objectives", false);

	_SFX_Play_Raw(1, 0, "select1.wav");

	g_pOverlayMgr->EnableMouseMode(bEnable);
	DarkOverlay::SetEnable(bEnable);
}

void PlayersOverlay::OnEndGameClicked(DialogElement* element, Point pt)
{
	DbgPrint("End game clicked");
}

void PlayersOverlay::OnAddTimeClicked(DialogElement* element, Point pt)
{
	if (g_Net == STATE_Host)
	{
		Client.SetTimeRemaining(Client.GetTimeRemaining() + (5.0f * 60.f));
		SendTimeLimit();
	}
}

void PlayersOverlay::OnKickPlayerClicked(DialogElement* element, Point pt)
{
	if (g_Net == STATE_Host)
	{
		int playerID = *(int*)element->GetData();
		DPNID dpnid = g_pNetMan->ToNetPlayerID(g_pNetMan->PlayerNumToObj(playerID));
		g_pDarkNet->DestroyPeer(dpnid, NULL, 0);
		_SFX_Play_Raw(1, 0, "select1.wav");
	}
}

MapOverlay::MapOverlay(const char* overlayName) : DarkOverlay(overlayName)
{
	m_pMapImg = NULL;
	m_currentPage = -1;
	m_currentLocation = -1;

	m_pQuestData = _AppGetAggregated(IID_IQuestData);
}

MapOverlay::~MapOverlay()
{
}

bool MapOverlay::Term()
{
	if (!DarkOverlay::Term())
		return false;

	SetEnable(false);

	FreePCX(m_pMapImg);
	return true;
}

bool MapOverlay::Init()
{
	if (!DarkOverlay::Init())
		return false;

	sAutomapProperty* pProp = AutomapGetCurrentRoom();
	if (pProp)
	{
		m_currentPage = pProp->page;
	}
	else
		m_currentPage = m_pQuestData->Get("MAP_MIN_PAGE");

	LoadMapImage();

	sScreenMode mode = GetScreenMode();

	int rectWidth = m_Rect.right - m_Rect.left;

	m_drawRect = m_Rect;

	m_minPage = m_pQuestData->Get("MAP_MIN_PAGE");
	m_maxPage = m_pQuestData->Get("MAP_MAX_PAGE");

	//FetchUIRects("mapr", m_UIRects, "intrface");
	return true;
}

void MapOverlay::Draw()
{
	IRes* pCurrentHighlight;
	IRes* pVisitedHighlight;
	char buff[128];
	cDynArray<Rect> tmpRects;

	// Close if player is dead
	if (_GetPlayerMode() == MODE_Dead)
	{
		SetEnable(false);
		return;
	}

	UpdateLocation();

	if (m_pMapImg)
		DrawByHandle(m_pMapImg, Point(m_drawRect.left, m_drawRect.top));

	// Get the map section rects
	sprintf(buff, "%s/p%03dra", _GetMissionData()->pathname, m_currentPage);
	FetchUIRects(buff, tmpRects, "intrface");

	if (tmpRects)
	{
		// Draw the blue highlights for the visited areas
		for (uint i = 0; i < tmpRects.Size(); i++)
		{
			if (_DarkAutomapGetLocationVisited(m_currentPage, i))
			{
				sprintf(buff, "%s/p%03dr%03d", _GetMissionData()->pathname, m_currentPage, i);
				pVisitedHighlight = LoadPCX(buff, "intrface");

				Rect r = tmpRects[i];

				r += m_drawRect;

				if (pVisitedHighlight)
					AutomapHighlightArea(pVisitedHighlight, &r, 0);
			}
		}

		// Get the highlight graphic for the player's location
		sprintf(buff, "%s/p%03dr%03d", _GetMissionData()->pathname, m_currentPage, m_currentLocation);
		pCurrentHighlight = LoadPCX(buff, "intrface");

		// Draw the current area
		if (pCurrentHighlight)
		{
			Rect r = tmpRects[m_currentLocation];

			r += m_drawRect;
			AutomapHighlightArea(pCurrentHighlight, &r, 1);
		}

		cDABaseSrvFns::DoResize((void**)&tmpRects, sizeof(Rect), 0);
	}
}

void MapOverlay::SetEnable(bool bEnable)
{
	if (_GetPlayerMode() == MODE_Dead)
		return;

	DarkOverlay::SetEnable(bEnable);

	// Load the current location now if mouse mode is enabled, since it won't be updated per-frame
	if (Cfg.GetBool("AutomapMouseMode"))
	{
		g_pOverlayMgr->EnableMouseMode(bEnable);

		if (bEnable)
		{
			sAutomapProperty* pProp = AutomapGetCurrentRoom();

			if (pProp)
			{
				m_currentPage = pProp->page;
				m_currentLocation = pProp->location;

				RefreshPage();
			}
		}
	}

	MessageMgr::Get()->EnableRendering(bEnable ? false : true);
}

void MapOverlay::UpdateLocation()
{
	// Do per-frame updates on the current location/page if we're not in mouse mode
	if (!Cfg.GetBool("AutomapMouseMode"))
	{
		sAutomapProperty* pProp = AutomapGetCurrentRoom();

		if (pProp)
		{
			if (pProp->page != m_currentPage)
			{
				DbgPrint("New page: %d", pProp->page);
				m_currentPage = pProp->page;
				RefreshPage();
			}
			if (pProp->location != m_currentLocation)
			{
				m_currentLocation = pProp->location;
				//RefreshHighlight();
			}
		}
	}
}

void MapOverlay::RefreshPage()
{
	//FreePCX(m_pMapImg);
	LoadMapImage();
}

void MapOverlay::LoadMapImage()
{
	char buff[64];
	sMissionData* pMissData = _GetMissionData();

	sprintf(buff, "%s\\page%03d", pMissData->pathname, m_currentPage);

	m_pMapImg = LoadPCX(buff, "intrface");

	if (!m_pMapImg)
		m_pMapImg = LoadPCX("map1", "intrface");

	dbgassert(m_pMapImg);
}

void MapOverlay::OnLeftClick(Point pt)
{
	if (PointInOverlay(pt))
	{	
		if (m_currentPage < m_maxPage)
			m_currentPage++;
		else
			m_currentPage = m_minPage;

		RefreshPage();
	}
}

long __stdcall OnReadBook(DWORD P1, const char* text, const char* background)
{
	if (Cfg.GetBool("UseOverlays"))
	{
		if (g_pOverlayMgr->OverlayEnabled("overlay_book"))
		{
			// just close if we're already opened
			g_pOverlayMgr->DisableAll();
			return 0;
		}

		g_pOverlayMgr->DisableAll();
		g_pOverlayMgr->EnableOverlay("overlay_book", true);

		BookOverlay* pOverlay = (BookOverlay*)(g_pOverlayMgr->FindByName("overlay_book"));

		if (pOverlay->IsEnabled())
			pOverlay->LoadBook(background, text);

		return 0;
	}
	else
		return cDarkUISrv::ReadBook(P1, text, background);
}

BookOverlay::BookOverlay(const char* overlayName) : DarkOverlay(overlayName)
{
	m_pTextRes = m_pTextStringRes = NULL;
	m_backgroundImgFile = NULL;
	m_textFile = NULL;
	m_pBookImg = NULL;
	m_pBookFont = NULL;
	ZeroMemory(m_pBookBackgrounds, sizeof(m_pBookBackgrounds));
}

BookOverlay::~BookOverlay()
{
	SAFE_RELEASE(m_pBookFont);
}

bool BookOverlay::Init()
{
	if (!DarkOverlay::Init())
		return false;

	m_backgroundImgFile = NULL;
	m_textFile = NULL;
	m_pTextRes = NULL;
	m_pTextStringRes = NULL;

	CenterRect(m_drawRect, m_Rect);

	//m_pBitmap = _gr_alloc_bitmap(3, 0, m_drawRect.right - m_drawRect.left, m_drawRect.bottom - m_drawRect.top);
	//_gr_make_canvas(m_pBitmap, &m_TheCanvas);
	return true;
}

bool BookOverlay::Term()
{
	if (!DarkOverlay::Term())
		return false;

	DestroyBook();
	SetEnable(false);

	return true;
	//if (m_pBitmap)
	//	DarkFree(m_pBitmap);
}

void BookOverlay::Draw()
{
	GUIsetupStruct gss;
	char pageBuff[16];
	sScreenMode mode = GetScreenMode();

	// Close if player is dead
	if (_GetPlayerMode() == MODE_Dead)
	{
		SetEnable(false);
		return;
	}

	dbgassert(m_TextRects && m_PageRects);

	if (!m_PageRects)
		return;

	dbgassert(m_pBookImg);
	DrawByHandle(m_pBookImg, Point(m_drawRect.left, m_drawRect.top));

	dbgassert(m_PageRects.Size() > 0);

	// Draw next/previous page images
	if (m_firstPage != m_currentPage)
	{
		DrawByHandle(m_pBookBackgrounds[0], Point(m_rectPrevPage.left, m_rectPrevPage.top));
	}
	if (m_lastPage != m_currentPage)
	{
		DrawByHandle(m_pBookBackgrounds[1], Point(m_rectNextPage.left, m_rectNextPage.top));
	}

	sprintf(pageBuff, "PAGE_%d", m_currentPage);
	char* bookText = m_pTextStringRes->StringLock(pageBuff);
	if (!bookText || !bookText[0])
		return;

	grs_font* fontRes = (grs_font*)m_pBookFont->Lock();
	if (!fontRes)
		return;

	short charHeight = _gr_font_string_height(fontRes, "X");

	if (isspace(bookText[0]))
	{
		while (bookText[0])
		{
			if (isspace(bookText[0]))
				bookText++;
			else
				break;
		}
	}

	//while (isspace(*bookText) && *bookText)
	//	bookText++;

	for (uint i = 0; i < m_TextRects.Size(); i++)
	{
		Rect r = m_TextRects[i];

		// Not really sure what the purpose of this is
		for (char* str = strchr(bookText, 1); str; str = strchr(bookText, 1))
			*str = ' ';

		// Set up paging
		_gr_font_string_wrap(fontRes, bookText, m_TextRects[i].Width()); // 7/10/10 - moved outside of GUI calls

		// Find page break
		int pageHeight = r.bottom - r.top;
		char* nextPageText = bookText;
		for (int i = 0; i + charHeight < pageHeight;)
		{
			if (*nextPageText == '\n' || *nextPageText == 0x01)
			{
				i += charHeight; // new line
			}
			
			nextPageText++;
			if (*nextPageText == 0)
				break;
		}

		char tmp = *nextPageText;
		*nextPageText = 0;

		// Draw book text
		_GUIsetup(&gss, &r, 2, 0);
		_gr_font_string(fontRes, bookText, 0, 0);
		_GUIdone(&gss); 

		_gr_font_string_unwrap(bookText);

		*nextPageText = tmp;
		bookText = nextPageText;

		while (isspace(*bookText) && *bookText)
			bookText++;
	}

	m_pTextStringRes->StringUnlock(pageBuff);
	m_pBookFont->Unlock();
	CamApplyAlpha(0.50f, 0); // Fade the background a bit
}

void BookOverlay::SetEnable(bool bEnable)
{
	DarkOverlay::SetEnable(bEnable);

	MessageMgr::Get()->EnableRendering(bEnable ? false : true);
	g_pOverlayMgr->EnableMouseMode(bEnable);
}

void BookOverlay::OnLeftClick(Point pt)
{
	// Some books have page turning animations in the original book UI, should implement this eventually
	if (PointInRect(pt, m_rectNextPage))
	{
		if (m_currentPage < m_lastPage)
			m_currentPage++;
	}
	else if (PointInRect(pt, m_rectPrevPage))
	{
		if (m_currentPage > m_firstPage)
			m_currentPage--;
	}
}

void BookOverlay::DestroyBook()
{
	SAFE_DELETE(m_backgroundImgFile);
	SAFE_DELETE(m_textFile);

	for (int i = 0; i < 4; i++)
		FreePCX(m_pBookBackgrounds[i]);

	FreePCX(m_pBookImg);

	m_TextRects.Destroy();
	m_PageRects.Destroy();

	SAFE_RELEASE(m_pBookFont);
	SAFE_RELEASE(m_pTextRes);
	m_pTextRes = m_pTextStringRes = NULL;
}

void BookOverlay::LoadBook(const char* background, const char* text)
{
	char buff[64];
	sScreenMode mode = GetScreenMode();

	DestroyBook();

	IPtr<IResMan> pResMan = _AppGetAggregated(IID_IResMan);
	SAFE_DELETE(m_backgroundImgFile);
	SAFE_DELETE(m_textFile);

	m_backgroundImgFile = _strdup(background);
	m_textFile = _strdup(text);

	for (int i = 0; i < 4; i++)
	{
		sprintf(buff, "%s\\bookr%03d.pcx", m_backgroundImgFile, i);

		m_pBookBackgrounds[i] = LoadPCX(buff, "books");
		dbgassert(m_pBookBackgrounds[i]);
	}

	sprintf(buff, "%s\\textfont", m_backgroundImgFile);
	m_pBookFont = pResMan->Bind(buff, "Font", NULL, "Books", NULL);

	// Load the book 's string resource
	m_pTextRes = pResMan->Bind(m_textFile, "String", _gContextPath, "Books", NULL);

	// Fall back to a blank page if the resource didn't exist
	if (!m_pTextRes)
		m_pTextRes = pResMan->Bind("blankbk", "String", _gContextPath, "Books", NULL);

	m_pTextRes->QueryInterface(IID_IStringRes, (void**)&m_pTextStringRes);

	dbgassert(m_pBookFont);
	dbgassert(m_pTextRes);

	SetupPages();

	sprintf(buff, "Books\\%s", m_backgroundImgFile);
	FetchUIRects("textr", m_TextRects, buff);

	sprintf(buff, "Books\\%s", m_backgroundImgFile);
	FetchUIRects("bookr", m_PageRects, buff);

	sprintf(buff, "%s\\book.pcx", m_backgroundImgFile);
	m_pBookImg = LoadPCX(buff, "books");

	// Adjust button and text position for screen resolution
	m_rectPrevPage = ScaleRect(m_PageRects[0], m_Rect);
	m_rectNextPage = ScaleRect(m_PageRects[1], m_Rect);

	for (uint i = 0; i < m_TextRects.Size(); i++)
		m_TextRects[i] = ScaleRect(m_TextRects[i], m_Rect);

	DbgPrint("Loaded book with background %s, text %s. First page %d, last %d", background, text, m_firstPage, m_lastPage);
}

void BookOverlay::SetupPages()
{
	char buff[64];

	int page = 0;
	while (page < MAX_BOOK_PAGES)
	{
		sprintf(buff, "PAGE_%d", page);
		if (m_pTextStringRes->StringExtract(buff, buff, sizeof(buff)))
		{
			break;
		}

		page++;
	}

	m_firstPage = page;

	while (true)
	{
		sprintf(buff, "PAGE_%d", page);

		if (!m_pTextStringRes->StringExtract(buff, buff, sizeof(buff)))
			break;

		page++;
	}

	m_lastPage = page - 1;
	m_currentPage = 0;
}

SimOverlay::SimOverlay(const char* overlayName) 	: DarkOverlay(overlayName)
{
	m_pBackgroundImg = NULL;
	m_pBitmap = NULL;

	FetchUIRects("simr", m_ButtonRects, "intrface");
}

SimOverlay::~SimOverlay()
{
	Term();

	m_ButtonRects.Destroy();
}

bool SimOverlay::Init()
{
	if (!DarkOverlay::Init())
		return false;

	m_pBackgroundImg = LoadPCX("sim", "intrface");

	m_drawRect = m_Rect;

	m_pBitmap = _gr_alloc_bitmap(3, 0, m_drawRect.right - m_drawRect.left, m_drawRect.bottom - m_drawRect.top);
	_gr_make_canvas(m_pBitmap, &m_TheCanvas);

	ZeroMemory(&m_Style, sizeof(m_Style));
	_uiGameLoadStyle("panel_", &m_Style, "intrface");
	_uiGameLoadStyle("sim_", &m_Style, "intrface");
	return true;
}

bool SimOverlay::Term()
{
	if (!DarkOverlay::Term())
		return false;

	FreePCX(m_pBackgroundImg);

	if (m_pBitmap)
	{
		DarkFree(m_pBitmap);
		m_pBitmap = NULL;
	}

	return true;
}

void SimOverlay::Draw()
{
	GUIsetupStruct gss;
	IPtr<IResMan> pResMan = _AppGetAggregated(IID_IResMan);

	// Acquire font resource
	IRes* pFont = pResMan->Bind("textfont", "Font", NULL, "intrface", NULL);
	grs_font* fontRes = (grs_font*)pFont->Lock();

	dbgassert(fontRes);

	_gd_clear(0);

	//_SetCurrentStyle(&m_Style);

	_gr_push_canvas(&m_TheCanvas);
	_GUIsetup(&gss, &m_drawRect, 2, 0);


	DrawByHandle(m_pBackgroundImg, Point(m_drawRect.left, m_drawRect.top));

	for (uint i = 0; i < m_ButtonRects.Size(); i++)
	{
		//_gr_font_string_wrap(fontRes, bookText, margins);

		_grd_canvas->color = 1;
		_gd_rect(m_ButtonRects[i].left, m_ButtonRects[i].top, m_ButtonRects[i].right, m_ButtonRects[i].bottom);

		_grd_canvas->color = _guiScreenColor(m_Style.colors.colorText);
		_grd_canvas->fontResource = fontRes;
		_gr_font_string(fontRes, "Button", m_ButtonRects[i].left, m_ButtonRects[i].top);

		//_gr_font_string_unwrap(bookText);
	}

	_GUIdone(&gss);
	_gr_pop_canvas();

	CamApplyAlpha(0.50f, 0);

	_uiGameUnloadStyle(&m_Style);

	pFont->Unlock();
	SAFE_RELEASE(pFont);
	SAFE_RELEASE(pResMan);
}

void SimOverlay::SetEnable(bool bEnable)
{
	DarkOverlay::SetEnable(bEnable);

	MessageMgr::Get()->EnableRendering(bEnable ? false : true);
	g_pOverlayMgr->EnableMouseMode(bEnable);
}

ObjectivesOverlay::ObjectivesOverlay(const char* overlayName) : DarkOverlay(overlayName)
{
	m_pQuestData = _AppGetAggregated(IID_IQuestData);
}

ObjectivesOverlay::~ObjectivesOverlay()
{
}

bool ObjectivesOverlay::Init()
{
	NString str;

	if (!DarkOverlay::Init())
		return false;

	// Get mission name
	m_strMissName.Destroy();
	m_strMissName = GetMissionName(_GetMissionData()->missNumber);

	// Get difficulty string
	m_strDifficulty = FetchUIString("newgame", "difficulty", "intrface");
	m_strDifficulty += " ";
	int difficulty = m_pQuestData->Get("Difficulty");
	str.Format("diff_%d", difficulty);
	m_strDifficulty += FetchUIString("newgame", str, "intrface");

	// Set up GUI styles
	ZeroMemory(&m_PanelStyle, sizeof(m_PanelStyle));
	m_PanelStyle = *g_DefaultPanelStyle;
	_uiGameLoadStyle("panel_", &m_PanelStyle, "intrface");
	_uiGameLoadStyle("objctiv_", &m_PanelStyle, "intrface");

	ZeroMemory(&m_GoalTextStyle, sizeof(m_GoalTextStyle));
	m_GoalTextStyle.colors = _GetCurrentStyle()->colors;
	_uiGameLoadStyle("goal_text_", &m_GoalTextStyle, "intrface");

	ZeroMemory(&m_GreyStyle, sizeof(m_GreyStyle));
	m_GreyStyle.colors = _GetCurrentStyle()->colors;
	_uiGameLoadStyle("grey_", &m_GreyStyle, "intrface");

	FetchUIRects("objctivr", m_UIRects, "intrface");

#ifndef OBJECTIVES_DRAW_BACKGROUND
	sScreenMode mode = GetScreenMode();
	m_UIRects[0].bottom = mode.height;
	m_UIRects[0].right = mode.width;
#endif
	return true;
}

bool ObjectivesOverlay::Term()
{
	if (!DarkOverlay::Term())
		return false;

	m_strMissName.Destroy();
	m_UIRects.Destroy();

	//_uiGameUnloadStyle(&m_PanelStyle);
	//_uiGameUnloadStyle(&m_GoalTextStyle);
	//_uiGameUnloadStyle(&m_GreyStyle);

	return true;
}

void ObjectivesOverlay::DrawGoalStatus(int goalState, grs_font* font, int ypos)
{
	NString str;

	str.Format("goali%03d", goalState);
	IRes* imgSrc = LoadPCX(str, "intrface");

	// Get image dimensions
	ImageResource* img = (ImageResource*)imgSrc->Lock();
	int dim = img->height - 3; // x or y dimension of image?
	imgSrc->Unlock();

	// Draw goal state
	DrawByHandle(imgSrc, Point(0, ypos + (_gr_font_string_height(font, "X") - dim) / 2));
	FreePCX(imgSrc);
}

int ObjectivesOverlay::GetGoalSpacing(grs_font* font)
{
	int spacing = _gr_font_string_height(font, "X") / 4;
	if (spacing <= 5 )
		spacing = 5;

	return spacing;
}

bool ObjectivesOverlay::DisplayFits(const Rect& rect, grs_font* font)
{
	int maxHeight = rect.Height();
	int ypos = GetGoalSpacing(font) + 2 * _gr_font_string_height(font, "X");
	int spacing = 0;

	for (int i = 0;; i++)
	{
		int state = GetGoalState(m_pQuestData, i);
		if (!state) break;

		if (state != 1)
		{
			ypos += spacing;
			spacing = GetGoalSpacing(font);

			cAnsiStr goal = GoalDescription(i);
			_gr_font_string_wrap(font, goal, rect.Width() - 16);

			ypos += _gr_font_string_height(font, goal);

			if (ypos >= maxHeight)
			{
				return false; // won't fit
			}
		}
	}
	return true;
}

void ObjectivesOverlay::Draw()
{
	NString str;
	GUIsetupStruct gss;
	IPtr<IResMan> resman = _AppGetAggregated(IID_IResMan);

	// Close if player is dead
	if (_GetPlayerMode() == MODE_Dead)
	{
		SetEnable(false);
		return;
	}

#ifdef OBJECTIVES_DRAW_BACKGROUND
	// Draw background
	IRes* pImg = LoadPCX("objctiv", "intrface");
	DrawByHandle(pImg, Point(m_drawRect.left, m_drawRect.top));
	FreePCX(pImg);

#endif

	Rect r = m_UIRects[0];
	_GUIsetup(&gss, &r, 3, 0);

	// Acquire font resource
	grs_font* fontRes = g_TextFont;
	if (!DisplayFits(r, fontRes))
		fontRes = g_SmallFont;

	// Get dimensions
	int rectWidth = r.right - r.left;

	int missTextWidth = _gr_font_string_width(fontRes, m_strMissName);
	int ypos = _gr_font_string_height(fontRes, m_strMissName) + GetGoalSpacing(fontRes);

	// Draw mission name (centered at top)
	_grd_canvas->color = _guiStyleGetColor(&m_GoalTextStyle, 2);
	_gr_font_string(fontRes, m_strMissName, (r.Width() - missTextWidth) / 2, 0);

	// Draw difficulty text
	_grd_canvas->color = _guiStyleGetColor(&m_GoalTextStyle, 2);
	_gr_font_string(fontRes, m_strDifficulty, 0, ypos);
	ypos += _gr_font_string_height(fontRes, m_strDifficulty);

	for (int i = 0;; i++)
	{
		int state = GetGoalState(m_pQuestData, i);
		if (!state) break;

		if (state != 1)
		{
			cAnsiStr goal = GoalDescription(i);
			_gr_font_string_wrap(fontRes, goal, rectWidth - 16);

			int goalHeight = _gr_font_string_height(fontRes, goal);
			str.Format("GOAL_STATE_%d", i);
			int goalState = m_pQuestData->Get(str);
			if (goalState == GOAL_STATE_INACTIVE)
				_grd_canvas->color = _guiStyleGetColor(&m_GreyStyle, 2);
			else
				_grd_canvas->color = _guiStyleGetColor(&m_GoalTextStyle, 2);

			// Draw goal fiction text
			_gr_font_string(fontRes, goal, kGoalTextStatusBorder, ypos);

			// Get image resource for current status
			DrawGoalStatus(goalState, fontRes, ypos);

			ypos += goalHeight;
		}

	}

	_GUIdone(&gss);

	CamApplyAlpha(0.50f, 0);
}

void ObjectivesOverlay::SetEnable(bool bEnable)
{
	if (_GetPlayerMode() == MODE_Dead)
		return;

	DarkOverlay::SetEnable(bEnable);

	MessageMgr::Get()->EnableRendering(bEnable ? false : true);
	g_pOverlayMgr->EnableMouseMode(bEnable);
	_SFX_Play_Raw(1, 0, "select2.wav");
}

StagingOverlay::StagingOverlay(const char* overlayName) : PlayersOverlay(overlayName)
{
}

StagingOverlay::~StagingOverlay()
{
}

void StagingOverlay::Draw()
{
	// Need to reload every frame to avoid graphical corruption for some reason, crappy
	IRes* img = LoadPCX("objctivthin", "multi_interface");
	DrawByHandle(img, Point(m_drawRect.left, m_drawRect.top));
	FreePCX(img);

	// Update player info
	PlayersOverlay::UpdatePlayers();

	DarkOverlay::Draw();
}

#define HeaderColY 0.10f
#define InfoColY 0.15f
#define InfoColSpacing 0.03f
#define GameSettingsColX 0.55f

bool StagingOverlay::Init()
{
	NString strTitle, strTemp;
	m_playerUpdateTime = 0;

	if (!DarkOverlay::Init())
		return false;

	m_drawRect.right = 640;
	m_drawRect.bottom = 480;
	CenterRect(m_drawRect, m_Rect);

	sMissionData* pMissionData = _GetMissionData();
	strTemp.Format("title_%d", pMissionData->missNumber);

	//strTitle.Format("%s - %s", wstos(g_pDarkNet->GetSessionManager()->GetServerName()).c_str(), FetchUIString("titles", strTemp, "strings").m_pString);

	//AddText(strTitle, 0.50f, 0.05f, ElementAlignCenter, MCOL_White, g_TextFont);
	AddText("Players", 0.11f, HeaderColY, 0, CLR_White, g_TextFont);

	// Add player names
	float y = 0.15f;
	for (int i = 0; i < countof(m_txtPlayerNames); i++)
	{
		NString str;

		str.Format("Player %d", i);
		m_txtPlayerNames[i] = AddText(str, 0.13f, y, 0, i == 0 ? CLR_Blue : CLR_Red, g_SmallFont);
		y += InfoColSpacing;
	}

	// Add player pings
	AddText("Ping", 0.35f, HeaderColY, 0, CLR_White, g_TextFont);
	y = InfoColY;
	for (int i = 0; i < countof(m_txtPings); i++)
	{
		NString str;

		m_txtPings[i] = AddText("0", 0.35f, y, 0, CLR_White, g_SmallFont);
		y += InfoColSpacing;
	}

	// Add kick buttons
	y = InfoColY;
	for (int i = 0; i < countof(m_txtPings); i++)
	{
		NString str;

		m_imgKicks[i] = AddImage("goali003", "intrface", 0.10f, y, 0, OnKickPlayerClicked);
		m_imgKicks[i]->SetData(new int(i + 1));
		y += InfoColSpacing;
	}

	// Add game settings
	AddText("Game Settings", 0.55f, 0.1f, 0, CLR_White, g_TextFont);
	y = InfoColY;

	m_txtServer = AddText("", 0.55f, y, 0, CLR_Yellow, g_SmallFont);
	y += InfoColSpacing;
	m_txtMission = AddText("", 0.55f, y, 0, CLR_Yellow, g_SmallFont);
	y += InfoColSpacing;
	m_txtDamagePlayers = AddText("", 0.55f, y, 0, CLR_Yellow, g_SmallFont);
	y += InfoColSpacing;
	m_txtIncreaseDifficulty = AddText("", 0.55f, y, 0, CLR_Yellow, g_SmallFont);
	y += InfoColSpacing;
	m_txtRespawn = AddText("", 0.55f, y, 0, CLR_Yellow, g_SmallFont);
	y += InfoColSpacing;
	m_txtCheats = AddText("", 0.55f, y, 0, CLR_Yellow, g_SmallFont);
	y += InfoColSpacing;
	m_txtTimeLimit = AddText("", 0.55f, y, 0, CLR_Yellow, g_SmallFont);
	y += InfoColSpacing;
	m_txtSaveLimit = AddText("", 0.55f, y, 0, CLR_Yellow, g_SmallFont);
	y += InfoColSpacing;

	UpdateSettings();

	AddButton("Load Mission", 0.755f, 0.89f, ElementAlignRight, g_pNetMan->AmDefaultHost() ? CLR_Green : CLR_Gray, g_TextFont, OnLoadMission);
	AddButton("Quit", 0.32f, 0.89f, ElementAlignRight, CLR_Green, g_TextFont, OnQuit);
	//AddEdit("Edit test", 0.40f, 0.335f, 0, CLR_Yellow, g_SmallFont, 30, 0);
	return true;
}

// Updates game setting text after a change
void StagingOverlay::UpdateSettings()
{
	NString str;
	const SessionInfo& info = g_pDarkNet->GetSessionInfo();

	str.Format("Server: %s", g_pDarkNet->GetSessionManager()->GetServerName());
	m_txtServer->SetText(str);
	str.Format("Mission: %s", GetMissionName(info.nextMission));
	m_txtMission->SetText(str);
	str.Format("Damage Players: %s", info.teamDamage ? "Yes" : "No");
	m_txtDamagePlayers->SetText(str);
	str.Format("Increase Difficulty: %s", info.increaseDifficulty ? "Yes" : "No");
	m_txtIncreaseDifficulty->SetText(str);
	str.Format("Cheats: %s", info.cheatsEnabled ? "Yes" : "No");
	m_txtCheats->SetText(str);

	str.Format("Players Respawn: %s", info.respawnEnabled ? "Yes" : "No");
	m_txtRespawn->SetText(str);

	if (info.timeLimit > 0) str.Format("Time Limit: %s", TimeString(info.timeLimit * 60));
	else str = "Time Limit: None";
	m_txtTimeLimit->SetText(str);

	if (info.saveLimit > 0) str.Format("Save Limit: %d", info.saveLimit);
	else str = "Save Limit: None";
	m_txtSaveLimit->SetText(str);
}

void StagingOverlay::OnLoadMission(DialogElement* element, Point pt)
{
	if (g_pNetMan->AmDefaultHost())
	{
		StagingClose();
		Client.LoadMission(0);
	}
}

void StagingOverlay::OnQuit(DialogElement* element, Point pt)
{
	StagingClose();
	 g_pNetMan->Leave();
	_SwitchToMainMenuMode(0);
	return;
}

void StagingOverlay::SetEnable(bool bEnable)
{
	if (m_enabled == bEnable)
		return;

	DarkOverlay::SetEnable(bEnable);
	g_pOverlayMgr->EnableMouseMode(bEnable);
	
}

void StagingOverlay::OnLeftClick( Point pt )
{
	PlayersOverlay::OnLeftClick(pt);
}

bool IsStagingMode()
{
	return g_pOverlayMgr->OverlayEnabled("overlay_staging");
}

void StagingDisplay()
{
	return;

	// Disable movement and UI rendering
	*enable_inv = FALSE;
	*_gNoMoveKeys = 1;

	SwitchToGameMode(1);
	g_pOverlayMgr->EnableOverlay("overlay_staging", true);
}

void StagingClose()
{
	*enable_inv = TRUE;
	*_gNoMoveKeys = 0;

	g_pOverlayMgr->EnableOverlay("overlay_staging", false);
}

void OverlayInit()
{
	g_pOverlayMgr = new DarkOverlayManager;
}

void OverlayTerm()
{
	SAFE_DELETE(g_pOverlayMgr);
}