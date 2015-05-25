#pragma once

void __stdcall EquipBowHook(int playerObj, int arrowArch, int P1);
void __stdcall UnEquipBowHook(int playerObj);
void SendSetWeapon(int weapon);
eFrobHandler __stdcall FrobItemHook(int objectID);
void PlayerCreateHook(int destroy, int playerObj);
int InvTakeObjFromWorld(int frobbedObj, int frobber);
void ThrowCreatureHook(int thrower, int thrown, float velocity, DWORD flags, DWORD P1, DWORD P2, DWORD P3);

void MPPlayerCreate();