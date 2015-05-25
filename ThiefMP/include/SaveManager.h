#pragma once

#include "Engine\inc\EventManager.h"

#define QS_SLOT 15

static VersionNum T2Version =
{
	0x1, 0x18
};

static const _TagFileTag TimerTag = 
{
	"MPDATA"
};

struct MPDataBlock
{
	float timeLeft;
	int savesLeft;
};

class SaveManager : public IEventListener
{
public:
	SaveManager();
	virtual ~SaveManager();

	void OnSaveRemoteGhosts(TagFileWrite* write);
	void SendLoadMsg();
	void SendSaveMsg(int slot);

	void GetSlotName(int slot, char* buffer);

	void DoNetworkLoad(int slot);
	void DoNetworkSave(int slot);

	void SetLastSlot(int slot);

	virtual void EventHandler(int eventType, void* eventData);

	static void __stdcall HookQuickSave();
	static void __stdcall HookQuickLoad();
	static void __stdcall HookLoadRemoteGhosts(TagFileRead* tag, unsigned long P2);
	static void HookSaveRemoteGhosts(TagFileWrite* write, unsigned long P2);

protected:
	void OnLoadRemoteGhosts(TagFileRead* tag, unsigned long P2);
	void OnSaveRemoteGhosts(TagFileWrite* tag, unsigned long P2);

	int m_lastSelectedSlot;
	int m_savesRemaining;
};

extern SaveManager* g_pSaveMgr;