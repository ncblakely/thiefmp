#pragma once

#include "IGSClient.h"

interface IGSBrowser
{
	virtual void HandlePingResponse(void* msgData) = 0;
	virtual void HandleRemoveServer(void* msgData, DWORD dataSize) = 0;
	virtual void HandleServerInfo(void* msgData, DWORD dataSize) = 0;

	// Event handlers
	virtual void OnStateChange(IGSClient::ConnState status) = 0;
	//void HandleRemoveServer(MsgRemoveServer* pMsg);
	//void HandleServerInfo(CGPServerInfo* pMsg);
};