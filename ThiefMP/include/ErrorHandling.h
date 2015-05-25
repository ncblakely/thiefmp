#pragma once

void __stdcall CollisionResolverHook(const struct tSpringInfo* springInfo, DWORD P2);
int CellBinComputeFuncHook(int objectID, DWORD P2, DWORD P3);
void ObjectAnglesFuncHook(int object);
int __stdcall OnCollideEvent(int P1, int P2, int P3, float P4, class cPhysClsn* pClsn);
void LoopSampleCallbackHook(DWORD P1, int handle);
int __fastcall PropSndEnterCBHook(cPropSndInstHigh* pInst, int, const cRoom* pRoom, const cRoomPortal* pPortal, const mxs_vector& vec, float P4);
void SpeechEndCallbackHook(int P1, int P2, int sndSchema);