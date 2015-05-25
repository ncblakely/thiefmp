/*************************************************************
* File: ClientPacketHandler.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Engine\inc\Crc32.h"
#include "Engine\inc\Lists.h"
#include "Engine\inc\NetMessage.h"

#include "Main.h"
#include "Client.h"
#include "Ghost.h"
#include "SaveManager.h"
#include "PlayerDB.h"
#include "NetReactions.h"
#include "NetProperties.h"
#include "SpeechNet.h"
#include "Sound.h"
#include "Callbacks.h"
#include "Gamesys.h"
#include "DarkOverlay.h"

void HookHandleFrobPickup(int frobber, int object);

extern SimpleQueue<NetMessage> NewReceiveQueue;

void PacketHandler::OnPollNetwork()
{
	if (g_pDarkNet)
	{
		while (NewReceiveQueue.GetSize())
		{
			NetMessage* pMessage = NewReceiveQueue.Pop();

			if (!pMessage)
			{
#ifdef _DEBUG
				__debugbreak();
#endif
				return;
			}

			DPNID dpnidFrom = pMessage->m_dpnidSender;

			m_pRcvData = pMessage->m_pMsgData;
			m_dpnidSender = pMessage->m_dpnidSender;
			Packet* pPacket = (Packet*)pMessage->m_pMsgData;

			switch (pPacket->type)
			{
			case PKT_SpeechHalt:
				HandleSpeechHalt(); break;
			case PKT_Speech:
				HandleSpeech(); break;
			case PKT_FrobObj: 
				HandleFrobObject(); break;
			case PKT_GiveObj:
				HandleGiveObj(); break;
			case PKT_SetWeapon:
				HandleSetWeapon(); break;
			case PKT_GenericReaction:
				HandleGenericReaction(); break;
			case PKT_TransferAI:
				HandleTransferAI(); break;
			case PKT_PlayerDeath:
				HandlePlayerDeath(); break;
			case PKT_Chat: 
				HandleChat(); break;
			case PKT_MissionStart:
				HandleMissionStart(); break;
			case PKT_MissionEnd:
				HandleMissionEnd(); break;
			case PKT_SaveGame:
				HandleSaveGame(); break;
			case PKT_LoadGame:
				HandleLoadGame(); break; 
			case PKT_TimeLimit:
				HandleSetTimeLimit(); break;
			case PKT_CrcRequest:
				HandleCrcRequest(); break;
			case PKT_CrcResponse:
				HandleCrcResponse(); break;
			case PKT_EnteredWorld:
				HandleEnteredWorld(); break;
			case PKT_ScriptMsg:
				HandleScriptMsg(); break;
			case PKT_CreatePlayer:
				HandleCreatePlayer(); break;
			case PKT_AddLoot:
				HandleAddLoot(); break;
			case PKT_ObjectDamaged:
				HandleObjectDamaged(); break;
			case PKT_RequestSnapshot:
				HandleSnapshotRequest(); break;
			case PKT_PlayerSnapshot:
				HandlePlayerSnapshot(); break;
			case PKT_ObjData:
				HandleObjectData(); break;
			default:
#ifndef _RELEASE
				__debugbreak();
#endif
				Log.Print("Got unknown packet type %d.", pPacket->type); break;
			}

			pMessage->FreeData();
			//SAFE_DELETE_ARRAY(pMessage->m_pMsgData);
			delete pMessage;
		}
	}
}

void PacketHandler::HandleObjectData()
{
	// Skip past the packet type and grab the object data header
	BYTE* dataPtr = m_pRcvData;
	ObjMsg_Header* header = (ObjMsg_Header*)dataPtr;

	dataPtr += sizeof(ObjMsg_Header);

	int obj = g_pNetMan->FromGlobalObjID(&header->id);
	int opcode = header->opcode;

	if (!obj)
		dbgassert(false);

	sGhostRemote* ghost = _GhostGetRemote(obj);
	if (!ghost)
		dbgassert(false);

	switch (opcode)
	{
	case OD_PlayerFullUpdate:
		{
			ObjMsg_PlayerFullUpdate* msg = (ObjMsg_PlayerFullUpdate*)dataPtr;

			if (ghost)
			{
				Position pos;
				sGhostPos ghostPos;
				ZeroMemory(&ghostPos, sizeof(sGhostPos));

				// clear "unitialized" flag for the ghost? at least I assume that's what this is, old thief 2 code did this
				ghost->cfg.flags &= ~0x10000;

				ghostPos.pos.pos.x= msg->posX;
				ghostPos.pos.pos.y = msg->posY;
				ghostPos.pos.pos.z = msg->posZ;
				ghostPos.pos.vel.x = msg->velX;
				ghostPos.pos.vel.y = msg->velY;
				ghostPos.pos.vel.z = msg->velZ;
				ghostPos.pos.angle_info.fac = msg->ang;
				ghostPos.time = _GetSimTime();
				ghostPos.pos.mode = msg->motion;

				ghost->info.last = ghostPos;
				ghost->info.pred = ghost->info.last;

				pos.vec = ghostPos.pos.pos;
				pos.heading = ghostPos.pos.angle_info.fac.heading;
				pos.pitch = ghostPos.pos.angle_info.fac.pitch;
				pos.bank = ghostPos.pos.angle_info.fac.bank;

				if (msg->flags & UpdateFlagAnimation)
				{
					if (msg->anim.schema_idx != ghost->critter.cur_mocap.schema_idx || msg->anim.motion_num != ghost->critter.cur_mocap.motion_num)
					{
						ghost->critter.cur_mocap = msg->anim;
						DbgPrintEcho("Setting animation to %d:%d", msg->anim.schema_idx, msg->anim.motion_num);
					}
				}

				//DbgPrintEcho("Updating ghost for obj %s", _ObjEditName(obj));

				//_ObjPosSetLocation(ghost->objectID, &pos);
				//_PhysSetVelocity(ghost->objectID, &ghostPos.heartbeat.velocity);
			}
			break;
		}
	case OD_PlayerAnimation:
		{
			ObjMsg_PlayerAnim* msg = (ObjMsg_PlayerAnim*)dataPtr;

			ghost->critter.cur_mocap.schema_idx = msg->anim;
			ghost->critter.cur_mocap.motion_num = -1;

			break;
		}
	case OD_PlayerStopAnimation:
		{
			if (ghost->critter.pMotCoord)
			{
				cMotionCoordinator* coord = (cMotionCoordinator*)ghost->critter.pMotCoord;
				coord->NotifyAboutMotionAbortRequest();
			}
			break;
		}
	default:
			DbgPrintEcho("Unknown packet for object data");
			__debugbreak();
			break;
	}
}

void PacketHandler::HandleFrobObject()
{
	CNetMsg_FrobObject* pMsg = (CNetMsg_FrobObject*)m_pRcvData;
	dbgassert(g_Net == STATE_Client);

	int frobberObj = g_pNetMan->PlayerNumToObj(pMsg->frobberID);
	int sourceObj = g_pNetMan->FromGlobalObjID(&pMsg->sourceObj);
	int destObj = g_pNetMan->FromGlobalObjID(&pMsg->destObj);

	if (Debug.IsFlagSet(DEBUG_FROBS))
		ConPrintF("Frob receive: ms down %d, source %d dest %d srcloc %d destloc %d frobber %s flags %d", pMsg->msDown, sourceObj, destObj, pMsg->srcLoc, pMsg->destLoc, _ObjEditName(frobberObj), pMsg->flags);
	HandleFrobRequest(pMsg->msDown, sourceObj, destObj, pMsg->srcLoc, pMsg->destLoc, frobberObj, pMsg->flags);
}

int g_giveObj = 0;
void PacketHandler::HandleGiveObj()
{
	CNetMsg_GiveObject* pMsg = (CNetMsg_GiveObject*)m_pRcvData;
	bool bAutoEquipOn = false;

	dbgassert(g_pNetMan->AmDefaultHost());

	int obj = g_pNetMan->FromGlobalObjID(&pMsg->object);

	if (OBJ_IS_CONCRETE(obj))
	{
		if (!_stricmp(g_pInputBinder->ProcessCmd("echo $auto_equip"), "1"))
			bAutoEquipOn = true;

		SendAddLoot(obj, g_pNetMan->ObjToPlayerNum(g_pNetMan->FromNetPlayerID(m_dpnidSender)));

		g_giveObj = obj;
		if (bAutoEquipOn)
			g_pInputBinder->ProcessCmd("auto_equip 0");
		HandleFrobPickup(*_gPlayerObj, obj);
		if (bAutoEquipOn)
			g_pInputBinder->ProcessCmd("auto_equip 1");

		if (Debug.IsFlagSet(DEBUG_INVENTORY))
			ConPrintF("giveobj: %s", _ObjEditName(obj));

		g_giveObj = 0;
	}
}

void PacketHandler::HandleAddLoot()
{
	CNetMsg_AddLoot* pMsg = (CNetMsg_AddLoot*)m_pRcvData;
	char buffer[256];
	sLoot* loot = NULL;
	int fakeLootObj = 0;

	sContainIter* pIter = g_pContainSys->IterStart(*_gPlayerObj);

	// find the fake loot object in the player's inventory, if we have one
	while (true)
	{
		if (g_pTraitMan->GetArchetype(pIter->object) == Gamesys.Arch.FakeLoot)
		{
			fakeLootObj = pIter->object;
			break;
		}

		if (!g_pContainSys->IterNext(pIter))
			break;
	}

	g_pContainSys->IterEnd(pIter);

	if (!fakeLootObj)
	{
		// no fake loot? make it
		fakeLootObj = g_pObjSys->Create(Gamesys.Arch.FakeLoot, 1);

		if (!_stricmp(g_pInputBinder->ProcessCmd("echo $auto_equip"), "1"))
		{
			g_pInputBinder->ProcessCmd("auto_equip 0");
			cContainSys::Add(g_pContainSys, *_gPlayerObj, fakeLootObj, 0, 1);
			g_pInputBinder->ProcessCmd("auto_equip 1");
		}
		else
			cContainSys::Add(g_pContainSys, *_gPlayerObj, fakeLootObj, 0, 1);
	}
	else
	{
		// play the pickup sound
		int soundArch = g_pObjSys->GetObjectNamed("pickup_loot");
		if (soundArch)
		{
			g_cSoundScrSrvScriptService->PlaySchemaAmbient(fakeLootObj, soundArch, (eSoundNetwork)0);
		}
		else
			dbgassert(false);
	}

	if (g_pLootProp->Get(fakeLootObj, (void**)&loot))
	{
		loot->art += pMsg->loot.art;
		loot->gems += pMsg->loot.gems;
		loot->gold += pMsg->loot.gold;
		loot->special += pMsg->loot.special;

		g_pLootProp->Set(fakeLootObj, (void*)loot);

		UpdateLootTotals(fakeLootObj, loot);

		_ObjGetModelName(pMsg->archetype, buffer);

		if (buffer[0])
		{
			DbgPrint("Setting model to %s", buffer);

			_ObjSetModelName(fakeLootObj, buffer);
		}

		_InvUIRefreshObj(fakeLootObj);

		if (pMsg->containingPlayerNum == g_pNetMan->MyPlayerNum())
		{
			if (!_stricmp(g_pInputBinder->ProcessCmd("echo $auto_equip"), "1"))
			{
				g_pInventory->Select(fakeLootObj);
			}
		}
	}
	else
		dbgassert(false);

	DbgPrint("Adding loot with archetype %d, art %d, gems %d, gold %d, special %d", pMsg->archetype, pMsg->loot.art, pMsg->loot.gems, pMsg->loot.gold, pMsg->loot.special);
}

void PacketHandler::HandleSetWeapon()
{
	CNetMsg_SetWeapon* pMsg = (CNetMsg_SetWeapon*)m_pRcvData;

	int obj = g_pNetMan->FromNetPlayerID(m_dpnidSender);

	if (obj != 0)
	{
		if (_IsAPlayer(obj) && obj != *_gPlayerObj)
			GhostAttachWeapon(obj, pMsg->weapType);
		else
			Log.Print("Failed to attach weapon to %d: not a player.", obj);
	}
}

void PacketHandler::HandleMissionStart()
{
	CNetMsg_StartMission* pMsg = (CNetMsg_StartMission*)m_pRcvData;

	assert(g_Net == STATE_Client);

	int missionID = pMsg->missionID;
	if (missionID > 0)
	{
		const SessionInfo& info = g_pDarkNet->GetSessionInfo();
		IPtr<IQuestData> pQD = _AppGetAggregated(IID_IQuestData);

		assert(!g_pNetMan->AmDefaultHost());

		pQD->Set("Difficulty", pMsg->difficulty);

		if (info.startingLoot)
			_config_set_int("cash_bonus", info.startingLoot);

		Client.LoadMission(missionID);
		StagingClose();
	}
	else
		dbgassert(false && "invalid mission id");
}

void PacketHandler::HandleMissionEnd()
{
	CNetMsg_EndMission* pMsg = (CNetMsg_EndMission*)m_pRcvData;

	assert(g_Net == STATE_Client);

	_UnwindToMissionLoop();
}

void PacketHandler::HandleLoadGame()
{
	CNetMsg_LoadGame* pMsg = (CNetMsg_LoadGame*)m_pRcvData;

	assert(g_Net == STATE_Client);
	g_pSaveMgr->DoNetworkLoad((int)pMsg->slot);
}

void PacketHandler::HandleSaveGame()
{
	CNetMsg_SaveGame* pMsg = (CNetMsg_SaveGame*)m_pRcvData;

	assert(g_Net == STATE_Client);
	g_pSaveMgr->DoNetworkSave((int)pMsg->slot);
}

void PacketHandler::HandleChat()
{
	CNetMsg_Chat* pMsg = (CNetMsg_Chat*)m_pRcvData;
	const char* playerName;

	if (strlen(pMsg->chatText) > kMaxMessageLength)
		return Log.Print("Message too long!");

	if (pMsg->idSpeaker != g_pNetMan->MyPlayerNum())
		playerName = Players.NameFromIndex(pMsg->idSpeaker);
	else
		playerName = g_pNetMan->GetPlayerName(*_gPlayerObj);

	char buffer[512];
	sprintf(buffer, "%s: %s", playerName, pMsg->chatText);
	Logger::PrintFile("Chat.log", buffer);
	//ConPrintF("first %x next %x", MessageMgr::Get()->GetFirst(), MessageMgr::Get()->GetLast());
	MessageMgr::Get()->AddLine(true, buffer);
	ConPrintF(buffer);

	if (!playerName)
	{
		MessageMgr::Get()->AddLineFormat(true, "Failed to get player name for DPNID %x, obj ID (%s)", pMsg->idSpeaker, _ObjEditName(g_pNetMan->FromNetPlayerID(pMsg->idSpeaker)));
	}

	// If we're the host and this isn't a loopback message, broadcast it to all peers
	if (g_Net == STATE_Host && m_dpnidSender != *cNetManager::gm_PlayerDPID)
	{
		CNetMsg_Chat msg;

		msg.idSpeaker = (BYTE)pMsg->idSpeaker;
		strcpy(msg.chatText, pMsg->chatText);
		DWORD size = (DWORD)sizeof(msg.type) + sizeof(msg.idSpeaker) + (strlen(msg.chatText) + 1) * sizeof(char);

		g_pDarkNet->Send(ALL_PLAYERS, (void*)&msg, size, NULL, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);
	}
}

void PacketHandler::HandleSetTimeLimit()
{
	CNetMsg_TimeLimit* pMsg = (CNetMsg_TimeLimit*)m_pRcvData;

	Client.SetTimeRemaining(pMsg->timer);
}

void PacketHandler::HandleCrcRequest()
{
#if (GAME == GAME_THIEF)
	CNetMsg_CrcRequest* pMsg = (CNetMsg_CrcRequest*)m_pRcvData;
	CNetMsg_CrcResponse msg;

	switch (pMsg->scanFileID)
	{
	case ScanFile_Gamesys:
		{
			if (g_gamesysName[0])
			{
				CNetMsg_CrcResponse msg;

				msg.crc = Crc32::ScanFile(g_gamesysName);
				msg.scanFileID = ScanFile_Gamesys;

				g_pDarkNet->SendToHost((void*)&msg, sizeof(msg), DPNSEND_GUARANTEED);
				DbgPrint("CRC response for gamesys: 0x%8x", msg.crc);
			}
		}
		break;
	case ScanFile_MotionDB:
		{
			msg.crc = Crc32::ScanFile("motiondb.bin");
			msg.scanFileID = ScanFile_MotionDB;
			g_pDarkNet->SendToHost((void*)&msg, sizeof(msg), DPNSEND_GUARANTEED);

			DbgPrint("CRC response for motiondb: 0x%8x", msg.crc);
		}
		break;
	case ScanFile_Mission:
		{
			NString str;

			str.Format("miss%d.mis", _GetMissionData()->missNumber);
			msg.crc = Crc32::ScanFile(str);
			msg.scanFileID = ScanFile_Mission;
			g_pDarkNet->SendToHost((void*)&msg, sizeof(msg), DPNSEND_GUARANTEED);

			DbgPrint("CRC response for mission: 0x%8x", msg.crc);
		}
		break;
	}

#endif
}

void PacketHandler::HandleCrcResponse()
{
	CNetMsg_CrcResponse* pMsg = (CNetMsg_CrcResponse*)m_pRcvData;

	if (!g_pNetMan->AmDefaultHost() || !g_pDarkNet)
		return;

	switch (pMsg->scanFileID)
	{
	case ScanFile_Gamesys:
		{
			if (pMsg->crc != Gamesys.GetGamesysCrc())
			{
				SDestroyReason msg;
				msg.reason = DReason_CrcFailed;

				MessageMgr::Get()->AddLineFormat(true, "Disconnecting %s: gamesys file does not match the server's.", Players.NameFromDPNID(m_dpnidSender));
				g_pDarkNet->DestroyPeer(m_dpnidSender, &msg, sizeof(DestroyReason));
			}
		}
		break;
	case ScanFile_MotionDB:
		{
			if (Crc32::ScanFile("motiondb.bin") != pMsg->crc)
			{
				SDestroyReason msg;
				msg.reason = DReason_CrcFailed;

				MessageMgr::Get()->AddLineFormat(true, "Disconnecting %s: motiondb.bin file does not match the server's.", Players.NameFromDPNID(m_dpnidSender));
				g_pDarkNet->DestroyPeer(m_dpnidSender, &msg, sizeof(DestroyReason));
			}
		}
		break;
	case ScanFile_Mission:
		{
			NString str;
			str.Format("miss%d.mis", _GetMissionData()->missNumber);

			if (Crc32::ScanFile(str) != pMsg->crc)
			{
				SDestroyReason msg;
				msg.reason = DReason_CrcFailed;

				MessageMgr::Get()->AddLineFormat(true, "Disconnecting %s: mission file does not match the server's.", Players.NameFromDPNID(m_dpnidSender));
				g_pDarkNet->DestroyPeer(m_dpnidSender, &msg, sizeof(DestroyReason));
			}
		}
		break;
	}
}

void PacketHandler::HandleEnteredWorld()
{
	CNetMsg_EnteredWorld* pMsg = (CNetMsg_EnteredWorld*)m_pRcvData;
	//m_numEnteredWorld++;
	//int numPlayers = g_pNetMan->NumPlayers();

	////ConPrintF("Enter world: num players %d, num entered", m_numEnteredWorld, numPlayers);

	//if (m_numEnteredWorld == numPlayers)
	//{
	//	*g_AllPlayersFinishedSynch = 1;
	//	*g_NumPlayersNotInGameMode = numPlayers + 1;

	//	GameModeEnterUnpause();

	//	if (g_pMessageMgr)
	//		MessageMgr::Get()->AddLine(false, "All players joined.");
	//	//_SimStateUnpause();
	//}
}

void PacketHandler::HandlePlayerDeath()
{
	const char* objName;
	const char* playerName;
	int playerIndex;
	int culprit;
	CNetMsg_PlayerDeath msg;

	msg.Rebuild(m_pRcvData);

	playerIndex = msg.m_playerIndex;
	culprit = msg.m_culprit;

	if (playerIndex <= 0)
		return;

	if (playerIndex == g_pNetMan->MyPlayerNum())
		playerName = g_pNetMan->GetPlayerName(*_gPlayerObj);
	else
		playerName = Players.NameFromIndex(playerIndex);

	//if (_IsAPlayer(culprit))
	//	MessageMgr::Get()->AddLineFormat(true, "%s was killed by %s.", playerName, Players.NameFromIndex(g_pNetMan->ObjToPlayerNum(culprit)));
	if (g_pMultiNameProp->Get(msg.m_culprit, &objName))
		MessageMgr::Get()->AddLineFormat(true, "%s was %s by %s.", playerName, _RandRange(0, 1) ? "killed" : "slain", objName);
	else
		MessageMgr::Get()->AddLineFormat(true, "%s has died.", playerName);
}

void PacketHandler::HandleSpeechHalt()
{
	CNetMsg_LoopSpeechHalt* pMsg = (CNetMsg_LoopSpeechHalt*)m_pRcvData;
	sSpeech* pSpeech;
	int object = g_pNetMan->FromGlobalObjID(&pMsg->speaker);

	if (OBJ_IS_CONCRETE(object) && g_pObjSys->Exists(object) && !g_pObjNet->ObjHostedHere(object))
	{
		if (g_pSpeechProp->Get(object, (void**)&pSpeech))
		{
			if (pSpeech->flags & 0x1)
			{
				//ConPrintF("Halting speech on %s.", _ObjEditName(object));
				_SchemaPlayHalt(pSpeech->pSchemaPlay);
			}
			//else
			//	DbgPrintEcho("Speech flags not 0x1, 0x%x", pSpeech->flags);

			g_pSpeechProp->Delete(object);
		}
	}
}

void PacketHandler::HandleSpeech()
{
	try
	{
		CNetMsg_AiSpeech* pMsg = (CNetMsg_AiSpeech*)m_pRcvData;

		sSpeech* pSpeech = NULL;
		int object = g_pNetMan->FromGlobalObjID(&pMsg->speaker);
		int schema = (int)pMsg->schemaID;
		int concept = pMsg->concept;
		int sampleNum = pMsg->sampleNum;

		if (OBJ_IS_CONCRETE(object) && g_pObjSys->Exists(object) && !g_pObjNet->ObjHostedHere(object))
		{
			int schemaHandle = g_SpeechHandles->GetIndex(concept);

			// If the sound wasn't played on the sender's end this will be -1, and there's no need to synchronize the samples
			if (sampleNum == -1)
				sampleNum = SchemaChooseSample(schema, -1);

			// If the sample number is still -1, something is wrong
			if (sampleNum == -1)
			{
				dbgassert(false && "bad speech sample num");
				return;
			}

			Label* pConceptLbl = cNameMapFns::NameFromID(g_Domain, NULL, concept);
			const char* name = _SchemaSampleGet(schema, sampleNum);
			ulong time = tm_get_millisec_unrecorded();

			if (Debug.IsFlagSet(DEBUG_SOUNDS))
				ConPrintF("Received speech: obj %d, schema %d, sample %d, schema handle %d, file %s, concept %s.", object, schema, sampleNum, schemaHandle, name, pConceptLbl);

			g_pSpeechProp->Get(object, (void**)&pSpeech);

			if (pSpeech && pSpeech->flags & 0x1 && schemaHandle < pSpeech->schemaHandle)
			{
				//ConPrintF("Schema handle %d for %s < %d", schemaHandle, pConceptLbl->name, pSpeech->schemaHandle);
				return;
			}
				

			sSchemaCallParams callParams;
			ZeroMemory(&callParams, sizeof(sSchemaCallParams));

			callParams.callFlags = 0xB0; // unknown value. used for AI speech
			callParams.P2 = -1;
			callParams.conceptName = pConceptLbl;
			callParams.pfnEndSampleCB = SpeechEndCallback;
			callParams.object1 = object;
			callParams.object2 = object;

			// Halt the object's current speech before we play the new sound
			_SpeechHalt(object);

			// Create the speech property if it doesn't already exist, and get a pointer
			if ((pSpeech = _SpeechPropCreateAndGet(object)))
			{
				sSchemaPlay* pSchemaPlay = NetSchemaPlaySample(schema, sampleNum, &callParams, NULL);

				pSpeech->pSchemaPlay = pSchemaPlay;
				if (pSchemaPlay)
				{
					// Update speech property
					pSpeech->time = time;
					pSpeech->schemaID = schema;
					pSpeech->concept = concept;
					pSpeech->pSchemaPlay = pSchemaPlay;
					pSpeech->flags |= 1; // a flag of 1 indicates that speech is currently playing
					pSpeech->schemaHandle = schemaHandle;
				}

				g_pSpeechProp->Set(object, (void*)pSpeech);
			}
		}
	}

	catch (...) 
	{
		const char* s = "%s: exception.";
		MessageMgr::Get()->AddLineFormat(true, s, __FUNCTION__);
		Log.Print(s, __FUNCTION__);
	}
}

int lastReactionType = 0;

void PacketHandler::HandleGenericReaction()
{
	try
	{
		CNetMsg_GenericReaction msg;

		msg.Rebuild(m_pRcvData);

		DbgPrint("Received generic reaction: type %d, param reacting %d param inst %d param flags %d, num chained events %d, event P2 %d, event reacting %d", 
			msg.m_reactionID, msg.m_reactParams.reactingObj, msg.m_reactParams.instigatorObj, msg.m_reactParams.flags, msg.m_numChainedEvents, msg.m_reactEvent.P2, msg.m_reactEvent.reactingObj);

		DbgPrint("Start received events:");
		PrintEventChain(msg.m_chainedEvents);

		if (!g_pObjSys->Exists(msg.m_reactParams.reactingObj))
		{
			if (Debug.IsFlagSet(DEBUG_GENERAL))
				ConPrintF("Dropping reaction packet: object %d does not exist.", msg.m_reactParams.reactingObj);
			return;
		}
		else if (!g_pObjNet->ObjHostedHere(msg.m_reactParams.reactingObj))
		{
			if (Debug.IsFlagSet(DEBUG_GENERAL))
				ConPrintF("Dropping reaction packet: %s not hosted here.", _ObjEditName(msg.m_reactParams.reactingObj));
			return;
		}

		if (Debug.IsFlagSet(DEBUG_RECEIVES))
			ConPrintF("Received reaction %s.", g_pReactions->DescribeReaction(msg.m_reactionID)->description);

		lastReactionType = msg.m_reactionID;

		NetworkableReactionInfo* pInfo = GetNetworkedReaction(msg.m_reactionID);

		assert(pInfo);

		if (pInfo->netReceiveFilter)
		{
			// Pass this reaction through our receive filter -- if it returns false, we shouldn't pass this reaction on to the game's handler
			if (!pInfo->netReceiveFilter(m_dpnidSender, &msg.m_reactEvent, &msg.m_reactParams, pInfo->handlerData ? *pInfo->handlerData : NULL))
				return;
		}

		pInfo->handler(&msg.m_reactEvent, &msg.m_reactParams, pInfo->handlerData ? *pInfo->handlerData : NULL);
	}

	catch (...) { MessageMgr::Get()->AddLineFormat(true, "Exception occurred while handling reaction type %d. Please report this bug.", lastReactionType); }
	//try
	//{
	//	CDarkMarshalBuffer marshalBuff;
	//	CT2GenericReaction msg;
	//	sChainedEvent* pEvents = NULL;

	//	marshalBuff.SetBuffer((BYTE*)pMsg, sizeof(CT2GenericReaction));
	//	msg.Unmarshal(marshalBuff);

	//	//Log.Print("Generic reaction: type %d, param reacting %d param inst %d param flags %d, event chained %x, event P2 %d, event reacting %d", 
	//	//	msg.m_reactionID, msg.m_reactParams.reactingObj, msg.m_reactParams.instigatorObj, msg.m_reactParams.flags, msg.m_reactEvent.event, msg.m_reactEvent.P2, msg.m_reactEvent.reactingObj);

	//	if (!g_pObjSys->Exists(msg.m_reactParams.reactingObj))
	//	{
	//		if (Debug.FlagSet(DEBUG_GENERAL))
	//			ConPrintF("Dropping reaction packet: object %d does not exist.", msg.m_reactParams.reactingObj);
	//		return;
	//	}
	//	else if (!g_pObjNet->ObjHostedHere(msg.m_reactParams.reactingObj))
	//	{
	//		if (Debug.FlagSet(DEBUG_GENERAL))
	//			ConPrintF("Dropping reaction packet: %s not hosted here.", _ObjEditName(msg.m_reactParams.reactingObj));
	//		return;
	//	}

	//	// Allocate events, if we have any
	//	if (msg.m_numChainedEvents> 0)
	//	{
	//		if (msg.m_numChainedEvents > MAX_CHAINED_EVENTS)
	//		{
	//			msg.m_numChainedEvents = MAX_CHAINED_EVENTS;
	//			MessageMgr::Get()->AddLineFormat(true, "Reaction receive (%s): too many chained events.", g_pReactions->DescribeReaction(msg.m_reactionID)->description);
	//		}

	//		pEvents = UnmarshalChainedEvents(marshalBuff, msg.m_numChainedEvents);
	//		msg.m_reactEvent.event = &pEvents[0];
	//	}

	//	//Log.Print("Generic reaction: type %d, param reacting %d param inst %d param flags %d, event chained %x, event P2 %d, event reacting %d", 
	//	//	msg.m_reactionID, msg.m_reactParams.reactingObj, msg.m_reactParams.instigatorObj, msg.m_reactParams.flags, msg.m_reactEvent.event, msg.m_reactEvent.P2, msg.m_reactEvent.reactingObj);

	//	if (Debug.FlagSet(DEBUG_RECEIVES))
	//		ConPrintF("Received reaction %s.", g_pReactions->DescribeReaction(msg.m_reactionID)->description);

	//	lastReactionType = msg.m_reactionID;

	//	NetworkableReactionInfo* pInfo = GetNetworkedReaction(msg.m_reactionID);

	//	assert(pInfo);

	//	if (pInfo->netReceiveFilter)
	//	{
	//		// Pass this reaction through our receive filter -- if it returns false, we shouldn't pass this reaction on to the game's handler
	//		if (!pInfo->netReceiveFilter(dpnidFrom, &msg.m_reactEvent, &msg.m_reactParams, pInfo->handlerData ? *pInfo->handlerData : NULL))
	//		{
	//			SAFE_DELETE_ARRAY(pEvents);
	//			return;
	//		}
	//	}

	//	pInfo->handler(&msg.m_reactEvent, &msg.m_reactParams, pInfo->handlerData ? *pInfo->handlerData : NULL);
	//}

	//catch (...) { MessageMgr::Get()->AddLineFormat(true, "Exception occurred while handling reaction type %d. Please report this bug.", lastReactionType); }
}

void PacketHandler::HandleTransferAI()
{
	CNetMsg_TransferAI* pMsg = (CNetMsg_TransferAI*)m_pRcvData;
	IPtr<IAINetManager> pAINet = _AppGetAggregated(IID_IAINetManager);

	if (pAINet)
	{
		sAINetTransfer transfer;
		int object = g_pNetMan->FromGlobalObjID(&pMsg->aiObject);
		int voiceArch = (int)pMsg->voiceArch;
		int awareness = (int)pMsg->awarenessLevel;

		transfer.c = 0xC;
		transfer.objPos = &pMsg->position;

		if (Debug.IsFlagSet(DEBUG_SENDS))
			ConPrintF("Making full AI for %s.", _ObjEditName(object));

		HRESULT hRes = pAINet->MakeFullAI(object, &transfer);
		if (hRes != S_OK)
			return ConPrintEchoF("ERROR: AI handoff failed for %s", _ObjEditName(object));

		if (GetAIVoice(object) == -1)
		{
			if (Debug.IsFlagSet(DEBUG_SENDS))
				ConPrintF("Transferee's voice set to %s.", _ObjEditName(voiceArch));

			ppVoiceLink->Add(object, voiceArch);
		}

//		if (awareness > 0)
//		{
//			bool bSetAwareness = AITools::SetAIAwareness(object, *_gPlayerObj, 3);
//#ifndef _RELEASE
//			if (!bSetAwareness)
//				DbgPrintEcho("Failed to set awareness on AI.");
//#endif
//		}
	}
}

void PacketHandler::HandleScriptMsg()
{
	try
	{
		CNetMsg_ScriptMsg msg;
		msg.Rebuild(m_pRcvData);

		sScrMsg* pMsg = msg.ScriptMessageCreate();

		if (pMsg)
		{
			if (Debug.IsFlagSet(DEBUG_SCRIPTS))
				DbgPrint("Handling message %s, dest %d, source %d, parm type %d, parm %x.", msg.m_message, msg.m_dest, msg.m_source, msg.m_parms._type, msg.m_parms._int);

			g_pScriptMan->SendMessageA(pMsg, NULL);

			delete pMsg;
		}
	}

	catch (...) { ConPrintEchoF("Exception handling script message."); }
}

void PacketHandler::HandleObjectDamaged()
{
	CNetMsg_ObjectDamaged* pMsg = (CNetMsg_ObjectDamaged*)m_pRcvData;
	int bloodObj;
	const char* bloodType = NULL;
	BOOL bCausesBlood = FALSE;
	int victim = g_pNetMan->FromGlobalObjID(&pMsg->obj);
	int stimulus = pMsg->stimulus;

	if (stimulus)
	{
		DbgPrint("Handling object damage on %s with stimulus %d at %0.2f %0.2f %0.2f", _ObjEditName(victim), pMsg->stimulus, pMsg->hitPos.x, pMsg->hitPos.y, pMsg->hitPos.z);

		if (!g_pBloodCauseProp->Get(stimulus, &bCausesBlood))
			return;

		if (!g_pBloodTypeProp->Get(victim, &bloodType))
			return;

		DbgPrint("Creating blood...");
		bloodObj = g_pObjSys->BeginCreate(g_pObjSys->GetObjectNamed(bloodType), 1);

		_ObjTranslate(bloodObj, &pMsg->hitPos);

		g_pObjSys->EndCreate(bloodObj);
	}
}

void SendPlayerSnapshot(int playerObj, DPNID dpnidTo)
{
	//CNetMsg_PlayerSnapshot msg;

	//Position* pos = _ObjPosGet(playerObj);

	//if (playerObj == *_gPlayerObj)
	//	msg.avatarArchetype = HookDarkNetServices::MyAvatarArchetype(0);
	//else
	//	msg.avatarArchetype = g_pTraitMan->GetArchetype(playerObj);
	//msg.localObject = playerObj;
	//msg.position = *(mxs_vector*)pos;
	//msg.bank = pos->angle.bank;
	//msg.dpnid = g_pNetMan->ToNetPlayerID(g_pObjNet->ObjHostPlayer(playerObj));
	//msg.playerNum = (BYTE)g_pNetMan->ObjToPlayerNum(playerObj);

	//DbgPrint("Sending snapshot for player %d: archetype %d object %d DPNID %x", msg.playerNum, msg.avatarArchetype, msg.localObject, msg.dpnid);

	//g_pDarkNet->Send(dpnidTo, (void*)&msg, sizeof(CNetMsg_CreatePlayer), NULL, DPNSEND_GUARANTEED);
}

void PacketHandler::HandleSnapshotRequest()
{
	CNetMsg_RequestSnapshot* pMsg = (CNetMsg_RequestSnapshot*)m_pRcvData;
	int playerObj;
	sPropertyObjIter iter;

	DbgPrint("------------------------");
	DbgPrint("Sending snapshot to ID %x", m_dpnidSender);

	SendPlayerSnapshot(*_gPlayerObj, m_dpnidSender);

	g_pNetMan->NetPlayerIterStart(&iter);
	for (int i = 0; i < kMaxPlayers; i++)
	{
		if (g_pNetMan->NetPlayerIterNext(&iter, &playerObj))
		{
			if (playerObj)
			{
				SendPlayerSnapshot(playerObj, m_dpnidSender);
			}
		else
			break;
		}
	}
}
	
void PacketHandler::HandleCreatePlayer()
{
	CNetMsg_CreatePlayer* pMsg = (CNetMsg_CreatePlayer*)m_pRcvData;
	int objId;
	int archetype;
	int playerNum;
	mxs_angvec ang;
	mxs_vector vec;
	sNetPlayer netPlayer;

	archetype = (int)pMsg->avatarArchetype;

	if (g_pNetMan->x20d)
	{
		playerNum = pMsg->playerNum;
		cAvatar* pAvatar = g_pNetMan->m_Avatars[playerNum];

		if (pAvatar)
		{

			objId = pAvatar->PlayerObjID();

			g_pNetMan->m_Avatars[playerNum] = NULL;

			_GhostChangeRemoteConfig(objId, 0x20000, 1);
			_GhostRemRemote(objId);

			float heightOffset = 0;
			vec = pMsg->position;
			gAvatarHeightOffsetProp->Get(objId, &heightOffset);

			vec.z += heightOffset;
			_ObjTranslate(objId, &vec);

			ang.heading = 0;
			ang.pitch = 0;
			ang.bank = pMsg->bank;

			_ObjRotate(objId, &ang);

			_GhostAddRemote(objId, 1.0f, 3);
		}
		else
		{
			__debugbreak();
			return;
		}
	}
	else
	{
		//g_pNetMan->SuspendMessaging();
		objId = g_pObjSys->BeginCreate(archetype, 1);
		
		float heightOffset = 0;
		vec = pMsg->position;
		gAvatarHeightOffsetProp->Get(objId, &heightOffset);

		vec.z += heightOffset;
		_ObjTranslate(objId, &vec);

		ang.heading = 0;
		ang.pitch = 0;
		ang.bank = pMsg->bank;

		_ObjRotate(objId, &ang);

		g_pObjSys->EndCreate(objId);
		//g_pNetMan->ResumeMessaging();

		playerNum = pMsg->playerNum;

		_GhostAddRemote(objId, 1.0f, 3);
	}

	g_pObjNet->ObjRegisterProxy(objId, pMsg->localObject, objId);

	netPlayer.playerDpnid = pMsg->dpnid;
	netPlayer.pAvatar = new cAvatar(objId, playerNum);
	netPlayer.x08 = NULL;
	ZeroMemory(netPlayer.playerName, sizeof(netPlayer.playerName));

	cNetManager::gm_NetPlayerProp->Set(objId, (void*)&netPlayer);

	if (playerNum == 1)
	{
		DbgPrint("Setting default host to %d", objId);
		*cNetManager::gm_DefaultHostPlayer = objId;
	}

	g_pNet->SetPlayerData(pMsg->dpnid, (void*)&objId, 4, 1);

	g_pObjNet->ResolveRemappings((ulong)pMsg->playerNum, objId);
}

// not used yet
void PacketHandler::HandlePlayerSnapshot()
{
	//CNetMsg_PlayerSnapshot* pMsg = (CNetMsg_PlayerSnapshot*)m_pRcvData;

	//HandleCreatePlayer();

	//g_pNetMan->m_numSynched++;
}