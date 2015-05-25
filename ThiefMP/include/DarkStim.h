#pragma once

struct sDamageParam // is this related to sReactionParam? that'd explain a few things...
{
	int damageType; // 0
	BOOL bUseStimulusAsType; // 4
	float multiplyBy; // 8
	float thenAdd; // C
};

struct sReactionEvent
{
	sChainedEvent* event;	// 0 -- total size is at least 1C (see StimScriptMsg)
	DWORD P2;	// 4
	int reactingObj;	 // 8
};

struct sReactionParam
{
	int reactingObj;		// 0
	int instigatorObj;	// 4
	DWORD flags; // 8
	float P3;					// C
	float P4;					// 10
	float P5;					// 14
};

// reaction handler params

struct sStimulateParam
{
	char stimulus[16];
	float multiplyBy;
	float thenAdd;
};

struct sAmplifyParam
{
	float multiplyBy;
	float thenAdd;
};

struct sWeakPointParam
{
	float multiplyBy;
	float thenAdd;
	float offsetX;
	float offsetY;
	float offsetZ;
	float radius;
};

struct sSpoofDamageParam
{
	int damageType;
	BOOL bUseStimulusAsType;
	float multiplyBy;
	float thenAdd;
};

struct sSetModelParam
{
	char modelName[16];
};

struct sObjCreateParam
{
	mxs_vector position;
	float heading;
	float pitch;
	float bank;
};

struct sObjMoveParam
{
	mxs_vector position;
	float heading;
	float pitch;
	float bank;
};

// size is 32 for these two?
struct sPropRemParam
{
	char propName[16];
};

struct sPropAddParam
{
	char propName[16];
};