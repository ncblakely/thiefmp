/*************************************************************
* File: LG.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"
#include "Main.h"

bool IsSimTimePassing()
{
	return _SimStateCheckFlags(8) == 8;
}

const sChainedEvent* sChainedEvent::Find(unsigned long idToFind)
{
	const sChainedEvent* event = this;

	if (event)
	{
		while (true)
		{
			if (event->eventID == idToFind)
				return event;
			else
			{
				if (event->next)
					event = event->next;
				else
					return NULL;
			}
		}
	}

	return NULL;
}

float cPlayerMode::GetTransSpeedScale()
{
	float f = g_ModeBaseSpeeds[m_modeID];

	for (uint i = 0; i < m_SpeedScales.Size(); i++)
	{
		f *= m_SpeedScales[i].transScale;
	}

	return f;
}

void __stdcall NewPickLockPerFrameCallback(int frameTime, DWORD extra)
{
	sLink link;
	ILinkQuery* pQuery = (ILinkQuery*)ppNowPickingRelation->Query(0, 0);

	while (!pQuery->Done())
	{
		pQuery->Link(&link);

		if (_DoPickOperation(*(int*)pQuery->Data(), link.sourceObj, link.destObj, frameTime) != 1)
			PickLockDoStop(link.destObj);

		pQuery->Next();
	}

	if (pQuery)
		pQuery->Release();
}

static bool bowPlayingStrainSound = false;
static float bow_pullback_time = 0.0f;

// configurable bow vars
static const float bow_wobble_start_s = 8.0f;
static const float bow_collapse_time = 11.0f;

// old integer values
static const int bow_renock_time = 900;

bool ArrowObjectExists()
{
	return (*_g_arrowObj != 0);
}

void DestroyArrowObject()
{
	if (ArrowObjectExists())
		g_pObjSys->Destroy(*_g_arrowObj);
}

//void DoBowAttack()
//{
//	if (!_IsBowEquipped())
//		return;
//	if (_GetPlayerMode() == MODE_Normal || _GetPlayerMode() == MODE_Crouching)
//	{
//		if (CreateAttachArrow())
//		{
//			*dword_673CD8 = 1;
//			_PlayerStartAction();
//			_WeaponEvent(1, *_gPlayerObj, 0, 2);
//			*g_TotalBowFrames = 0;
//			*g_BowWobble = 0;
//			return 1;
//		}
//	}
//
//	return 0;
//}

void ForceFinishBowAttack(bool putAwayBow)
{
	_WeaponEvent(WE_BowForceFinished, *_gPlayerObj, 0, 2);
	bowPlayingStrainSound = false;

	if (putAwayBow)
	{
		PutTheBowAway(1);
		*arrowNocked = FALSE;
	}
	else
	{
		_PlayerFinishAction();
		*g_BowIsForceFinished = false;
		*arrowNocked = FALSE;
	}

}

//void BowFrameCallback(unsigned long P1, void* P2)
//{
//	float elapsedTime;
//
//	if (!ArrowObjectExists)
//		return;
//
//	if (!g_pObjSys->Exists(*_g_arrowObj)
//	{
//		DetachArrow();
//		*last_arrow_object = 0;
//		*bow_attack_finished = 0;
//		return;
//	}
//
//	*InBowMode2 = 0;
//	if (GetBowElapsedTime(&elapsedTime))
//	{
//		BowZoomProcess(0, 0);
//
//	}
//}

void __stdcall NewUpdateBowAttack()
{
	DWORD dw;
	float secsframe = g_DeltaFrame.Get();

	if (_IsBowEquipped())
	{
		if (sub_56E0C0(&dw))
		{
			if (_GetPlayerMode() == MODE_Swimming)
			{
				ForceFinishBowAttack(1);
				PlaySchemaPlayer("bow_abort_auto");
				return;
			}

			// Update times
			bow_pullback_time += secsframe;
			*g_TotalBowFrames += _GetSimFrameTime();

			// Start wobbling the player's bow
			if (bow_pullback_time > bow_wobble_start_s)
			{
				//MessageMgr::Get()->AddLineFormat(false, "wobbling %d", g_BowWobbleAmount);
				float wobble = (bow_pullback_time - bow_wobble_start_s) * 1000;
				WobbleBow((int)wobble);
				if (!bowPlayingStrainSound)
				{
					PlaySchemaPlayer("garstrain");
					bowPlayingStrainSound = 1;
				}
			}

			if (bow_pullback_time > bow_collapse_time)
			{
				ForceFinishBowAttack(1);
				PlaySchemaPlayer("bow_abort_auto");
				return;
			}
		}

		if (*g_BowEquippedTime)
		{
			if (*g_BowEquippedTime < bow_renock_time)
			{
				UpdateBowTime(0, 0);
				if (!_RenockBow(*g_BowEquippedTime))
					return;
				if (*g_BowAttackStarted || !*g_BowIsForceFinished)
					return;
				else
					*g_BowEquippedTime = 0;
				return;
			}
		}

		if (*g_BowAttackStarted)
		{
			_ClearPlayerArmFilter();
			ZoomClear();
			if (DoBowAttack())
			{
				bow_pullback_time = 0.0f;
			}
			*g_BowAttackStarted = 0;
			*g_BowIsForceFinished = 0;
			return;
		}

		if (*g_BowIsForceFinished)
		{
			sub_56EB50();
			UpdateBowTime(0, 0);
			*g_BowWobble = 0;
			PutTheBowAway(0);
			*g_BowIsForceFinished = 0;
			return;
		}

		if (*g_BowPutAwayTime)
		{
			*g_BowPutAwayTime += _GetSimFrameTime();

			if (*g_BowPutAwayTime > 500)
				DestroyArrowObject();
		}
	}

	// call overwritten func
	_UpdateWeaponAttack();
}

//__declspec(dllexport) int __stdcall SpeechSpeak(int object, const Label* pLabel, cTagDBInput*, void*)
//{
//	if (!*g_bSpeechDatabaseLoaded)
//		return 0;
//
//	int voice = SpeechGetVoice(object);
//	int conceptIndex = cNameMapFns::IDFromName(g_Domain, NULL, pLabel);
//	if (conceptIndex == 0x0FF676980)
//		return 0;
//
//	int voiceIndex;
//	if (!_ObjGetSpeechVoiceIndex(voice, &voiceIndex))
//		return 0;
//
//	int speechHandle = g_SpeechHandles->GetIndex(conceptIndex);
//	ulong startTime = tm_get_millisec_unrecorded();
//
//	return 0;
//
//	//int speech;
//	//if (g_pSpeechProp->Get(object, &speech))
//}

IDarkDataSource* cDarkPanel::CursorPalette()
{
	IPtr<IResMan> resman = _AppGetAggregated(IID_IResMan);

	IDarkDataSource* pDataSource = (IDarkDataSource*)resman->Bind("cursor", "Palette", NULL, "intrface", NULL);

	return pDataSource;
}

void cDarkPanel::OnLoopMsg(int P1, tLoopMessageData__ *)
{
	switch (P1)
	{
	case kMsgBeginFrame:
		return _SFX_Frame(0, 0);
	case kMsgFrameReserved2:
	case kMsgSuspendMode:
		return RedrawDisplay();
	}
}

char cAnsiStr::ChNil = NULL;

//cAnsiStr::~cAnsiStr()
//{
//	if (m_pString[0] != ChNil)
//		FreeStr(m_pString);
//}

void cAnsiStr::Append(int length, const char* str)
{
	return cAnsiStrFns::Append(this, 0, length, str);
}

void cAnsiStr::Assign(int dataLength, const char* str)
{
	if (!dataLength)
		return dbgassert(false && "Invalid length");

	if (dataLength == 0)
	{
		if (m_pString != NULL)
		{
			m_pString[0] = '\0';
			m_dataLength = 0;
		}
		return;
	}
	if (str && str != m_pString)
	{
		cAnsiStrFns::AllocBuffer(this, NULL, dataLength);
		memcpy(m_pString, str, dataLength);
		m_pString[dataLength] = '\0';
		m_dataLength = dataLength;
	}


	//	if (str || m_pString)
	//	{
	//		if (dataLength > m_allocLength)
	//			cAnsiStrFns::AllocBuffer(this, NULL, dataLength);
	//		if (m_pString && str)
	//		{
	//			memcpy(m_pString, str, dataLength);
	//			m_dataLength = dataLength;

	//			m_pString[dataLength] = NULL;
	//		}
	//		else if (!dataLength)
	//			dbgassert(false && "Null string assignment");
	//	}
}

void cAnsiStr::FreeStr(char* str)
{
	if (str)
	{
		g_pMalloc->Free(str);
	}
}

char* cAnsiStr::ReallocStr(char* str, int len)
{
	return (char*)g_pMalloc->Realloc(str, len);
}

HRESULT __stdcall IUnknownImpl::QueryInterface(REFIID riid, void** ppvObject)
{
	assert(false && "QueryInterface() not implemented");

	return S_OK;
}

ULONG __stdcall IUnknownImpl::AddRef()
{
	m_refCount++;

	return m_refCount;
}

ULONG __stdcall IUnknownImpl::Release()
{
	m_refCount--;

	if (!m_refCount)
	{
		delete this;
		return 0;
	}
	else
		return m_refCount;
}

cAvatar::cAvatar(int objId, unsigned long playerNum)
{
	m_refCount = 1;
	m_playerNum = playerNum;
	m_playerObjId = objId;
}

unsigned long __stdcall cAvatar::PlayerNum()
{
	 return m_playerNum;
}

int __stdcall cAvatar::PlayerObjID()
{
	return m_playerObjId;
}

void __stdcall cAvatar::WriteTagInfo(ITagFile* pTag)
{
	pTag->Write((const char*)&m_playerNum, sizeof(m_playerNum));
	pTag->Write((const char*)&m_playerObjId, sizeof(m_playerObjId));
}