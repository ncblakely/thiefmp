/*************************************************************
* File: NetManager.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements VTable hooks of cNetManager member functions.
*************************************************************/

#include "stdafx.h"
#include "Main.h"
#include "Client.h"
#include "GlobalServer.h"

namespace HookNetManager
{

void __stdcall SendCreatePlayerNetMsg()
{
	//CNetMsg_CreatePlayer msg;
	//INetAppServices* pNetApp = (INetAppServices*)_AppGetAggregated(IID_INetAppServices);

	//Position* pos = _ObjPosGet(*_gPlayerObj);

	//msg.avatarArchetype = pNetApp->MyAvatarArchetype();
	//msg.localObject = (short)*_gPlayerObj;
	//msg.playerNum = (BYTE)g_pNetMan->m_playerNum;
	//msg.position = pos->vec;
	//msg.bank = pos->angle.bank;
	//msg.dpnid = *cNetManager::gm_PlayerDPID;

	//g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, sizeof(CNetMsg_CreatePlayer), NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);

	//_GhostAddLocal(*_gPlayerObj, 1.0f, 3);

	//SAFE_RELEASE(pNetApp);
}

void __stdcall Leave(INetManager* pThis)
{
	cNetManager::Leave(pThis);

	SAFE_DELETE(g_pBrowser);

//#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
//	SAFE_DELETE(g_pMessageMgr);
//#endif
}

// Called by DirectPlay
// Flags will always be DPPLAYER_REMOTE
// Type is DPPLAYERTYPE_PLAYER
// Context is cNetManager::gm_Net
// Called twice upon connection for a client, once for the host and once for the client
int __stdcall EnumPlayersCallback(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
	if (Debug.IsFlagSet(DEBUG_NET))
		ConPrintF("NET DEBUG: Enum players callback for: %s. Context: %x", lpName->lpszShortName, lpContext);
	else
		ConPrintF("%s is in the game.", lpName->lpszShortName);
	return cNetManager::EnumPlayersCallback(dpId, dwPlayerType, lpName, dwFlags, lpContext);
}

int __stdcall InitExistingPlayer(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
	if (Debug.IsFlagSet(DEBUG_NET))
		ConPrintF("NET DEBUG: Init existing player: %s. Context: %x", lpName->lpszShortName, lpContext);
	return cNetManager::InitExistingPlayer(dpId, dwPlayerType, lpName, dwFlags, lpContext);
}

int __stdcall _DestroyNetPlayerCallback(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext)
{
	if (Debug.IsFlagSet(DEBUG_NET))
		ConPrintF("NET DEBUG: Destroying net player: %s. Context: %x", lpName->lpszShortName, lpContext);
	return cNetManager::_DestroyNetPlayerCallback(dpId, dwPlayerType, lpName, dwFlags, lpContext);
}

const char* __stdcall GetPlayerAddress(DWORD thisObj, int playerNum)
{
	return ""; // doesn't seem to be important
}

#if (GAME == GAME_THIEF)
void __stdcall SuspendMessaging(INetManager* thisObj)
{
	thisObj->m_SuspendMessageDepth++;
}

void __stdcall ResumeMessaging(INetManager* thisObj)
{
	thisObj->m_SuspendMessageDepth--;

	if (thisObj->m_SuspendMessageDepth < 0)
	{
		// catch negative suspend depth errors
		__debugbreak();
	}
}
#endif

} // end HookNetManager namespace

namespace HookDarkNetServices
{

#if (GAME == GAME_THIEF || GAME == GAME_DROMED)
int __stdcall MyAvatarArchetype(DWORD thisObj)
{
	int arch;

	switch (Cfg.GetInt("Model"))
	{
	default:
	case MID_Garrett: arch = g_pObjSys->GetObjectNamed("MP Avatar"); break;
	case MID_MaleThief: arch = g_pObjSys->GetObjectNamed("MP Avatar 2"); break;
	case MID_Keeper: arch = g_pObjSys->GetObjectNamed("MP Avatar 3"); break;
	case MID_MaleServant: arch = g_pObjSys->GetObjectNamed("MP Avatar 4"); break;
	case MID_Basso: arch = g_pObjSys->GetObjectNamed("MP Avatar 5"); break;
	}

	assert(arch);
	return arch;
}
#endif

} // end HookDarkNetServices

void NetDisconnectListener(unsigned long eventID, unsigned long playerObject, void* pData)
{
	switch (eventID)
	{
	case 2:
		{
			if (g_pNetMan->AmDefaultHost())
			{
				sContainIter* pIter = g_pContainSys->IterStart(playerObject);

				//ConPrintF("Destroying player %d's inventory.", playerObject);

				while (true)
				{
					if (g_pObjSys->Exists(pIter->object))
					{
						//ConPrintF("Destroying %s", _ObjEditName(pIter->object));
						g_pObjNet->ObjTakeOver(pIter->object);
						g_pObjSys->Destroy(pIter->object);
					}

					if (!g_pContainSys->IterNext(pIter))
						break;
				}

				g_pContainSys->IterEnd(pIter);
			}
			else
				dbgassert(false && "not net host?");
		}
	}
}

void NetDisconnectListenerInit()
{
	static bool listening = false;

	if (!listening)
	{
		g_pNetMan->Listen(NetDisconnectListener, 0x93, NULL);
		listening = true;
	}
}