#pragma once

#include "Main.h"

#define NETREACT_LOOPBACK 0x1
#define NETREACT_ONLYLOOPBACK 0x2

typedef bool (__stdcall*ReactionNetFilter)(DPNID dpnidSender, sReactionEvent* pEvent, const sReactionParam* pParam, void*);

struct NetworkableReactionInfo
{
	const char* name;
	ReactionHandler handler;
	ReactionNetFilter netReceiveFilter;
	void** handlerData;
	DWORD reactFlags;
};

class HookReactions : public IReactions
{
public:
	void __stdcall OnReact(unsigned long reactionID, sReactionEvent* pEvent, sReactionParam* pParams);
};

NetworkableReactionInfo* GetNetworkedReaction(unsigned long reactionID);
void PostReactionMessage(unsigned long reactionID, sReactionEvent* pEvent, sReactionParam* pParams);

unsigned long __stdcall FilterFrobObjectReactionHandler(sReactionEvent* pEvent, const sReactionParam* pParams, void*);
bool __stdcall FilterKnockoutReaction(DPNID dpnidFrom, sReactionEvent* pEvent, const sReactionParam* pParams, void*);
