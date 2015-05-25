#pragma once

void __stdcall OnGhostJumpOff(int objectID);
void GhostFireListener(int objectID, cCreature* creature, DWORD P3, DWORD P4);
void GhostInstallFireListener(int objectID);
void GhostAttachWeapon(int ghostObj, char weapType);
void GhostSetLocalAnimation(int objectID, short anim);
char* __stdcall GhostChooseCaptureHook(sGhostRemote* ghost, int P1);
const char* GhostStateToString(int ghostState);
int GhostHeartbeatFrequencyMax(sGhostLocal* ghost, sGhostPos* pos);
int __stdcall IsNewPacketNeededHook(sGhostLocal* ghost, sGhostPos* pos);

void HookSaveRemoteGhosts(TagFileWrite* write, unsigned long P2);
void GhostFrameProcessLocal(sGhostLocal* pGhost, float frameTime);
void GhostFrameProcessRemote(sGhostRemote* pGhost, float frameTime);

void MocapRunMe(sGhostRemote* pGhost, char* tag);