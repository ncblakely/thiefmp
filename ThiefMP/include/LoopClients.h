#pragma once

struct sSimLoopMessageData
{
	DWORD x00, x04, x08;
	int frameTime; // 0xC
};

int __stdcall SimulationLoopHook(DWORD P1, eLoopMsgType type, tLoopMessageData__* pMsgData);
int __stdcall ObjSysLoopClientHook(DWORD P1, eLoopMsgType type, tLoopMessageData__* pMsgData);
int __stdcall RenderLoopHook(DWORD P1, eLoopMsgType type, tLoopMessageData__* pMsgData);
int __stdcall SimLoopClientHook(DWORD P1, eLoopMsgType type, tLoopMessageData__* pMsgData);
int __stdcall GameModeLoopClientHook(DWORD P1, eLoopMsgType type, tLoopMessageData__* pMsgData);