#pragma once

#include "Engine\inc\EventManager.h"

#include "Packets.h"
#include "DarkNet.h"
#include "Network.h"

enum EndReason
{
	ER_None,
	ER_HostLeft,
	ER_Kicked,
	ER_KickedCrc,
	ER_TimeExpired,
};


class PacketHandler
{
	friend class CClient;
public:
	void OnPollNetwork();

protected:
	void HandleFrobObject();
	void HandleGiveObj();
	void HandleSetWeapon();
	void HandleMissionStart();
	void HandleMissionEnd();
	void HandleLoadGame();
	void HandleSaveGame();
	void HandleChat();
	void HandleSetTimeLimit();
	void HandleCrcRequest();
	void HandleCrcResponse();
	void HandleEnteredWorld();
	void HandleSpeech();
	void HandleSpeechHalt();
	void HandleTransferAI();
	void HandleSnapshotRequest();
	void HandlePlayerSnapshot();
	void HandleCreatePlayer();
	void HandleAddLoot();
	void HandleObjectDamaged();
	void HandleGenericReaction();
	void HandlePlayerDeath();
	void HandleScriptMsg();
	void HandleObjectData();

	BYTE* m_pRcvData;
	DPNID m_dpnidSender;
};

class CTimeLimit
{
};

class CClient : public IEventListener
{
public:
	bool GameIsEnding();

	void DrawPlayerNames();
	void DebugDrawVisibility();
	void DebugDrawNetStats();
	void DebugDrawMiscStats();
	void RenderOverlays();

	void LoadMission(int missionID);
	void RespawnPlayer();

	void OnLoadWorld();

	float GetTimeRemaining();
	void SetTimeRemaining(float timeLimit);
	bool IsTimeLimitSet();

	void StartFadeIn(unsigned long fadeTime, BYTE r, BYTE g, BYTE b);
	void StartFadeOut(unsigned long fadeTime, BYTE r, BYTE g, BYTE b);
	void StartEndGameTimer(float endTimeSeconds, EndReason er);

	void Update();
	void UpdateTransparency();

	void OnDeath();

	bool IsSimRunning();
	bool MissionStarted();
	void SetSimRunning(bool bRunning);

	bool IsClient();
	bool IsHost();

	virtual void EventHandler(int eventType, void* eventData);

	// Static game callbacks
	static void StaticOnDeath();
	static int OnJump();
	static void __stdcall StartGameModeCallback(DWORD P1);
	static void OnPlayerCreate();

	PacketHandler m_PacketHandler;
	bool m_missionStarted; // has the mission been loaded? (used to determine whether new players can connect or not)
protected:
	bool m_bSimRunning;

	// time limit stuff
	float m_timeLimit;				// time limit for the game (if we have one)
	float m_timeLimitSyncTimer; // time before the host sends out another message to synchronize time limits
	bool m_timeLimitActive; // is there a time limit set?

	float m_respawnTimer; // time before the local player respawns
	float m_endGameTimer; // time before the game ends (set if time limit reached or host has left, generally)

	bool m_playerIsDead;	// are we dead?
	bool m_gameIsEnding;
	uint	 m_numEnteredWorld;
};

enum PlayerModels
{
	MID_Garrett,
	MID_MaleThief,
	MID_Keeper,
	MID_MaleServant,
	MID_Basso,
};

const int kTotalModels = 5;

extern CClient Client;
