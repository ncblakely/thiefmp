/*************************************************************
* File: MultiUI.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "MultiUI.h"
#include "DarkWinGui.h"

cMultiUI* g_pMultiUI = NULL;

const char* g_MultiUIButtons[] =  { "cancel", "host", "join" };
const sDarkPanelDesc g_MultiUIDesc =  { "netplay", 3, 5, 3, g_MultiUIButtons, 0, 0, 1, 0 };

void SwitchToMultiplayUIMode(int P1)
{
	if (!g_pMultiUI)
	{
		g_pMultiUI = new cMultiUI(&g_MultiUIDesc, "multi_interface");
	}

	g_pMultiUI->m_panelMode->AddRef();

	g_pMultiUI->m_panelMode->Switch(P1 ? 0 : 1, NULL);
	g_pMultiUI->m_panelMode->Release();
}

void cMultiUI::OnLoopMsg(int flags, tLoopMessageData__* pMsgData)
{
	switch (flags)
	{
	case kMsgBeginFrame:
		//RedrawDisplay(); break;
	default:
		break;
	}

	//ConPrintF("Loop msg: %x", flags);

	cDarkPanel::OnLoopMsg(flags, pMsgData);
}

void cMultiUI::OnButtonList(unsigned short flags, int selection)
{
	if (flags & 0x4)
	{
		switch (selection)
		{
		case kMPB_Cancel:
			OnEscapeKey(); break;
		case kMPB_Host:
			UI->CreateChildWindow(CW_HostDialog); break;
		case kMPB_Join:
			UI->CreateChildWindow(CW_ConnectDialog); break;
		default:
			break;
		}
	}
}

void cMultiUI::RedrawDisplay()
{
	sPanelModeDesc* pDesc = m_panelMode->Describe();
	RedrawImage(0, pDesc->x04 & 0x2);
	RedrawImage(1, pDesc->x04 & 0x4);

	Rect r;

	r.left = 0;
	r.top = 0;
	r.right = 640;
	r.bottom = 480;

	_region_expose(_LGadCurrentRoot(), &r);
	int exp = 0;
	for (uint i = 0; i < m_Rects.Size(); i++)
	{
		_region_expose(_LGadCurrentRoot(), &m_Rects[i]);
		exp++;
	}
}

void cMultiUI::RedrawImage(int image, int P2)
{
	IDarkDataSource* pDataSource = m_panelMode->GetImage((ePanelModeImage(image)));

	if (pDataSource)
	{
		void* pRes = pDataSource->Lock();

		int width, height;
		width = height = 0;

		if (P2)
		{
			width = _grd_canvas->width / 2;
			height = _grd_canvas->height / 2;
		}

		_gd_bitmap(pRes, width, height);

		pDataSource->Unlock();
		pDataSource->Release();
	}
}