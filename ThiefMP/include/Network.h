#pragma once

void ObjectSendData(int object, int opcode, void* pData, DWORD size, DWORD flags);

void SendEnteredWorld();
void SendTimeLimit();
void SendAddLoot(int object, unsigned long playerFrom);
void SendEndMission();
void SendPlayerAnimation(const char* animName, const char* playerTagName);
void SendPlayerStopAnimation();

void NetSimCallback(int a1, int a2);