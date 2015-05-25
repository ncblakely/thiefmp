#pragma once

struct sChainedEvent
{
	unsigned long eventID;	// 0
	DWORD P1;	// 4
	sChainedEvent *next;	// 8
	DWORD data; // C

	const sChainedEvent* Find(unsigned long eventID);
};

struct DamageChainedEventData
{
	DWORD x00, x04, x08, x0c;
	mxs_vector hitPos;
};

struct sChainedEvent2 : public sChainedEvent
{
	float intensity; // 10 - assuming this is intensity, this is multiplied by sReactionParam + 8 then added to by + C in the amplify reaction handler
};

// Size = 0x10
struct sDamage
{
	int dmgPoints;
	int stimArch;
	DWORD x08, x0c;
};

// Size = 0x1C
struct sDamageMsg
{
	sChainedEvent event;
	int victimID;	// 10	- confirmed 5/25/08
	int culpritID;	// 14 - confirmed 5/25/08
	sDamage* dmg;	// 18
};

struct sStimEvent
{
	int P1; // set to 7 in constructor
	int P2; // set to 28h in constructor
	sChainedEvent* pEvent;

};