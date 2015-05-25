#pragma once

#include "Main.h"

class cMultiUI : public cDarkPanel
{
public:
	cMultiUI(const sDarkPanelDesc* pDesc, const char* resPath) : cDarkPanel(pDesc, resPath) { }
	~cMultiUI() { Log.Print("In destructor"); }
	void *operator new(size_t size) { return DarkNew(size); }

	virtual void RedrawDisplay();
	virtual void OnButtonList(unsigned short, int);
	virtual void OnLoopMsg(int, tLoopMessageData__ *);

	void RedrawImage(int, int P2);

	//{ "cancel", "host", "join", "opts" };
protected:
	enum eButtons
	{
		kMPB_Cancel = 0,
		kMPB_Host = 1,
		kMPB_Join = 2,
		//kMPB_Options = 3,
	};
};

//class cShockMultiplayUI : public cDarkPanel
//{
//		LGadBox - 0xC0
//};

void SwitchToMultiplayUIMode(int P1);

extern "C" cMultiUI* g_pMultiUI;