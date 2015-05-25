#pragma once

//#include "Main.h"

extern "C"
{
	// sshock2.asm
	int  ShockPlayerActionToSchemaIdx(int, int, int);
	int DarkPlayerActionToSchemaIdx(const char* animName, const char* playerTagName);
	void ShockSetMode(int, int);

	//void __GhostPrintGhostPos(int objectID, struct sGhostPos* pos, int ghostType, char* text, int seqID);

	// message manager
	void AddConsoleMessage(const char* messageText);

	class cPhysModel* cPhysModels_Get(int);
}