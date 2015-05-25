/*************************************************************
* File: Net.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Implements VTable hooks of cNet member functions.
*************************************************************/

#include "stdafx.h"
#include "Main.h"
#include "DarkPeer.h"

namespace HookNet
{

int __stdcall Host(INet* thisObj, char* gameType, char* sessionName)
{
	g_pNetMan->SetMaxPlayers(Cfg.GetInt("MaximumPlayers"));

	assert(!g_pDarkNet);

	int ret = cNet::Host(thisObj, gameType, sessionName);
	if (ret)
		ConPrintF("Successfully hosted game. Awaiting connections.");
	else
		ConPrintF("Failed to host game.");

	return ret;
}

// Note: second arg repurposed to password (previously game name)
int __stdcall Join(INet* thisObj, const char* media, const char* password, const char* ip)
{
	if (!g_pDarkNet)
	{
		try
		{
			g_pDarkNet = new CDarkPeer(STATE_Client);
		}

		catch (DPlayException& error)
		{
			Log.Print("Join failed: %s (%x).", error.GetMessage(), error.GetCode());
			return 0;
		}
	}
	else
		Log.Print("Error: Dark peer already allocated!");

	if (!g_pDarkNet->ConnectToServer(ip, password))
		return 0;

	// 1/9/10: calling Join() doesn't seem to be necessary
	return 1;
	//int ret = cNet::Join(thisObj, media, "darksession", ip);
	//return ret;
}

long __stdcall SimpleCreatePlayer(INet* thisObj, char* player)
{
	int ret = cNet::SimpleCreatePlayer(thisObj, player);
	if (!ret)
		ConPrintF("Failed to create local player.");
	
	return ret;
}

} // HookNet