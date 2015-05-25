#pragma once

#include "MessageManager.h"
#include "Marshal.h"

#define MAX_SAVE_DESC 34 // Maximum length of the save game description that can be entered into the save menu, not including terminating null
#define MAX_CHAINED_EVENTS 8

class CDarkMarshalBuffer;

// New packet definitions

enum PacketType
{
	NewPacketStart = 599,

	PKT_PlayerDeath,
	PKT_Chat,
	PKT_SetWeapon,
	PKT_FrobObj,
	PKT_MissionStart,
	PKT_MissionEnd,
	PKT_SaveGame,
	PKT_LoadGame,
	PKT_GiveObj,
	PKT_TimeLimit,
	PKT_CrcRequest,
	PKT_CrcResponse,
	PKT_EnteredWorld,
	PKT_GenericReaction,
	PKT_Speech,
	PKT_SpeechHalt,
	PKT_TransferAI,
	PKT_ScriptMsg,
	PKT_CreatePlayer,
	PKT_ObjectDamaged,
	PKT_AddLoot,
	PKT_RequestSnapshot,
	PKT_PlayerSnapshot,
	PKT_ObjData,

	NewPacketEnd,
};

enum ObjectDataType
{
	OD_PlayerFullUpdate,
	OD_PlayerAnimation,
	OD_PlayerStopAnimation,
};

enum DestroyReason
{
	DReason_CrcFailed,
	DReason_Kicked,
};

#pragma pack(push, 1)

class Packet 
{
public:
	short type;
};

struct ObjMsg_Header : public Packet
{
	BYTE opcode;
	sGlobalObjID id;
};

#define UpdateFlagAnimation 0x1
struct ObjMsg_PlayerFullUpdate
{
	short posX, posY, posZ;
	short velX, velY, velZ;
	mxs_angvec ang;
	BYTE flags; 

	// old stuff for compatibility for now
	BYTE motion;
	sGhostMoCap anim;
};

struct ObjMsg_PlayerAnim
{
	short anim;
};

class CNetMsg_AddLoot : public Packet
{
public:
	CNetMsg_AddLoot() { type = PKT_AddLoot; }

	short archetype;
	sLoot loot;
	BYTE containingPlayerNum;
};

class CNetMsg_CreatePlayer : public Packet
{
public:
	CNetMsg_CreatePlayer() { type = PKT_CreatePlayer; }

	short avatarArchetype;
	short localObject;
	mxs_vector position;
	short bank;
	BYTE playerNum;
	DPNID dpnid;
};

class CNetMsg_ObjectDamaged : public Packet
{
public:
	CNetMsg_ObjectDamaged() { type = PKT_ObjectDamaged; }

	sGlobalObjID obj;
	short stimulus;
	mxs_vector hitPos;
	BYTE result;
};

class CNetMsg_RequestSnapshot : public Packet
{
public:
	CNetMsg_RequestSnapshot() { type = PKT_RequestSnapshot; }

protected:
};

class CNetMsg_PlayerSnapshot : public CNetMsg_CreatePlayer
{
public:
	CNetMsg_PlayerSnapshot() { type = PKT_PlayerSnapshot; }

protected:
};

class CNetMsg_AiSpeech : public Packet
{
public:
	CNetMsg_AiSpeech() { type = PKT_Speech; }

	sGlobalObjID speaker;
	short schemaID;
	short sampleNum;
	BYTE concept;
};

class CNetMsg_LoopSpeechHalt : public Packet
{
public:
	CNetMsg_LoopSpeechHalt() { type = PKT_SpeechHalt; }

	sGlobalObjID speaker;
};

class CNetMsg_TransferAI : public Packet
{
public:
	CNetMsg_TransferAI() { type = PKT_TransferAI; }

	sGlobalObjID aiObject;
	mxs_vector position;
	short voiceArch;
	BYTE awarenessLevel;
};

struct CNetMsg_Chat : public Packet
{
	CNetMsg_Chat() { type = PKT_Chat; }

	BYTE idSpeaker;
	char chatText[kMaxMessageLength + 1];
};

enum ScanFileID
{
	ScanFile_Gamesys,
	ScanFile_Mission,
	ScanFile_MotionDB,
};

class CNetMsg_CrcRequest : public Packet
{
public:
	CNetMsg_CrcRequest() { type = PKT_CrcRequest; }

	BYTE scanFileID;
};

class CNetMsg_CrcResponse : public Packet
{
public:
	CNetMsg_CrcResponse() { type = PKT_CrcResponse; }

	DWORD crc;
	BYTE scanFileID;
};

class CNetMsg_EnteredWorld : public Packet
{
public:
	CNetMsg_EnteredWorld() { type = PKT_EnteredWorld; }
};

class CNetMsg_SetWeapon : public Packet
{
public:
	CNetMsg_SetWeapon() { type = PKT_SetWeapon; }

	enum WeaponType
	{
		WeaponType_None = -1,
		WeaponType_Sword,
		WeaponType_Blackjack,
		WeaponType_Bow,
	};

	char weapType;
};

class CNetMsg_FrobObject : public Packet
{
public:
	CNetMsg_FrobObject() { type = PKT_FrobObj; }

	int msDown;
	sGlobalObjID sourceObj;
	sGlobalObjID destObj;
	int srcLoc;
	int destLoc;
	BYTE frobberID;
	unsigned long flags;
};

class CNetMsg_StartMission : public Packet
{
public:
	CNetMsg_StartMission() { type = PKT_MissionStart; }

	int missionID;
	int difficulty;
};

class CNetMsg_EndMission : public Packet
{
public:
	CNetMsg_EndMission() { type = PKT_MissionEnd; }
};

class CNetMsg_SaveGame : public Packet
{
public:
	CNetMsg_SaveGame() { type = PKT_SaveGame; }

	BYTE slot;
};

class CNetMsg_LoadGame : public Packet
{
public:
	CNetMsg_LoadGame() { type = PKT_LoadGame; }

	BYTE slot;
};

class CNetMsg_GiveObject : public Packet
{
public:
	CNetMsg_GiveObject() { type = PKT_GiveObj; }

	sGlobalObjID object;
};

class CNetMsg_TimeLimit: public Packet
{
public:
	CNetMsg_TimeLimit()  { type = PKT_TimeLimit; }

	float timer;
};

class SDestroyReason
{
public:
	BYTE reason;
};

class CMarshalled
{
public:
	virtual void Marshal(CDarkMarshalBuffer& buffer) = 0;
	virtual void Unmarshal(CDarkMarshalBuffer& buffer) = 0;

	virtual int Send(DPNID idTo, DWORD flags);
	virtual void Rebuild(BYTE* pBuffer);

protected:
	short m_Type;
};

//////////////////////////////////////////////
// Marshalled packets
//////////////////////////////////////////////

class CNetMsg_GenericReaction : public CMarshalled
{
public:
	CNetMsg_GenericReaction() { }
	CNetMsg_GenericReaction(short reactionID, sReactionEvent* pEvent, sReactionParam* pParams);

	virtual void Marshal(CDarkMarshalBuffer& buffer);
	virtual void Unmarshal(CDarkMarshalBuffer& buffer);

	BYTE m_numChainedEvents;
	short m_reactionID;
	sReactionParam m_reactParams;
	sReactionEvent m_reactEvent;
	sChainedEvent m_chainedEvents[MAX_CHAINED_EVENTS];
protected:
	static const short m_Type = PKT_GenericReaction;
};

#define SCRIPTMSG_HAS_PARMS 0x1
class CNetMsg_ScriptMsg: public CMarshalled
{
public:
	CNetMsg_ScriptMsg() { }
	CNetMsg_ScriptMsg(int source, int dest, const char* message, sMultiParm* pParms);

	virtual void Marshal(CDarkMarshalBuffer& buffer);
	virtual void Unmarshal(CDarkMarshalBuffer& buffer);

	sScrMsg* ScriptMessageCreate();

	int m_source;
	int m_dest;
	char m_message[32];
	BYTE m_flags;
	sMultiParm m_parms;

protected:
	sScrMsg* CreateFrobMsg();
	sScrMsg* CreateScrMsg();

	static const short m_Type = PKT_ScriptMsg;
};

class CNetMsg_PlayerDeath : public CMarshalled
{
public:
	CNetMsg_PlayerDeath() { }
	CNetMsg_PlayerDeath(int playerIndex, int culprit);

	BYTE m_playerIndex;
	int m_culprit;

	virtual void Marshal(CDarkMarshalBuffer& buffer);
	virtual void Unmarshal(CDarkMarshalBuffer& buffer);

protected:
	static const short m_Type = PKT_PlayerDeath;
};

#pragma pack(pop)