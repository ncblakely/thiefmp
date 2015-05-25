#pragma once

#include "Engine\inc\MemPatch.h"

void T2PatchVtables();
void InstallCallbacks();
void PatchCommands(bool patch);

extern PtrPatchList g_MultiplayerPatches[];
extern PtrPatchList g_StartupPatches[];
extern RawPatchList g_StartupRawPatches[];
extern RawPatchList g_MultiplayerRawPatches[];