/*************************************************************
* File: GameSys.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Engine\inc\Lists.h"

#include "Main.h"
#include "Gamesys.h"
#include "NetProperties.h"

CGamesysManager Gamesys;

ArchCache::ArchCache()
{
	ZeroMemory(this, sizeof(ArchCache));
}

void CGamesysManager::CacheArchetype(int& var, const char* archName)
{
	int newarch = g_pObjSys->GetObjectNamed(archName);
	if (!newarch)
		Log.Print("acache: failed to load %s", archName);
	else if (var && var != newarch) 
	{ 
		if (Debug.IsFlagSet(DEBUG_GENERAL))
			ConPrintF("acache: %s changed from %d to %d.", archName, var, newarch); 
	} 
	
	var = newarch;
}

int CGamesysManager::CreateNewArchetype(int donorArch, const char* name)
{
	int newArch = g_pObjSys->BeginCreate(donorArch, 2);

	g_pObjSys->NameObject(newArch, name);
	IProperty* donorProp = g_pPropMan->GetPropertyNamed("DonorType");

	if (donorProp->IsSimplyRelevant(donorArch))
	{
		donorProp->Copy(newArch, donorArch);
	}

	g_pObjSys->EndCreate(newArch);

	if (!newArch)
		Log.Print("acache: Failed to create %s.", name);

	return newArch;
}

void CGamesysManager::InitializeArchCache(DWORD flags)
{
	if (Debug.IsFlagSet(DEBUG_GENERAL))
		DbgPrint("acache: initializing with flags %X.", flags);

	CacheArchetype(Arch.PLYR_WEAPON, "PLYR_WEAPON");
	CacheArchetype(Arch.PLYR_DAMAGE, "PLYR_DAMAGE");
	CacheArchetype(Arch.AI_SPEECH, "AI_SPEECH");
	CacheArchetype(Arch.pinset, "pinset");
	CacheArchetype(Arch.tumblerset, "tumblerset");
	CacheArchetype(Arch.Crystal, "Crystal");
	CacheArchetype(Arch.Cameras, "Cameras");
	CacheArchetype(Arch.Projectile, "Projectile");
	CacheArchetype(Arch.LockPick, "LockPick");
	CacheArchetype(Arch.Marker, "Marker");
	CacheArchetype(Arch.RopeArrowRope, "RopeArrowRope");
	CacheArchetype(Arch.Crystal, "Crystal");
	CacheArchetype(Arch.Sword, "Sword");
	CacheArchetype(Arch.Blackjack, "Blackjack");
	CacheArchetype(Arch.IsLoot, "IsLoot");
	CacheArchetype(Arch.Garrett, "Garrett");
	CacheArchetype(Arch.SecCamera2, "SecCamera2");
	CacheArchetype(Arch.climbable, "climbable");
	CacheArchetype(Arch.Phantom, "Phantom");
	CacheArchetype(Arch.Creature, "Creature");

	SetNetCategoryByName("Odometer", CAT_LocalOnly);
	SetNetCategoryByName("Flinders", CAT_LocalOnly);
	SetNetCategoryByName("TerrPt", CAT_LocalOnly);
	//SetNetCategoryByName("AlarmLights", CAT_LocalOnly);

	CreateProjectilePickups();

	// Temporary fix for some lifts
	//SetNetCategoryByName("Lift", CAT_LocalOnly);

	// Allows clients to activate markers, but TrolPts can remain local
	SetNetCategoryByName("Marker", CAT_Hosted);
	SetNetCategoryByName("TrolPt", CAT_LocalOnly);

	// Fix projectile issues
	_SetNetworkCategory(Arch.Projectile, CAT_LocalOnly);

	SetNetCategoryByName("CritterShots", CAT_LocalOnly);

	// Fix duplicate vine/rope arrows
	_SetNetworkCategory(Arch.RopeArrowRope, CAT_LocalOnly);

	// Prevent frob requests from being sent when projectiles/lockpicks are used
	SetFrobHandler(Arch.Projectile, FROB_LocalOnly);
	SetFrobHandler(Arch.LockPick, FROB_LocalOnly);

	// Fix destroy object spam from tweqs
	SetNetCategoryByName("mosscarpet", CAT_LocalOnly);
	SetNetCategoryByName("SFX", CAT_LocalOnly);

	// Fix torch effect glitch
	SetNetCategoryByName("Extinguishable", CAT_LocalOnly);

	// initialize new archetypes
	assert(!g_pNetMan->Networking() && "shouldn't be networked yet");

	AvatarWeaponsInit();

	FakeLootInit();

	CreateBaseAvatar();
	CreateDerivedAvatars();

	g_pAINoHandoffProperty->Set(Arch.SecCamera2, TRUE);
	g_pAINoGhostProperty->Set(Arch.SecCamera2, TRUE);

	// Remove ghost for climbables; prevents ropes and vines from causing save file corruption
	g_pAINoGhostProperty->Set(Arch.climbable, TRUE);
	_SetNetworkCategory(Arch.climbable, CAT_LocalOnly);

	g_pAINoGhostProperty->Set(Arch.Phantom, TRUE);

	// Base archetype names
	SetObjectName("Spider", "a spider");
	SetObjectName("guard", "a guard");
	SetObjectName("thief", "a thief");
	SetObjectName("Undead", "an undead creature");
	SetObjectName("Apparition", "an apparition");
	SetObjectName("Phantom", "a phantom");
	SetObjectName("Beast", "a beast");
	SetObjectName("Robot", "a robot");
	SetObjectName("KeeperAgent", "a Keeper");

	// Child archetypes
	SetObjectName("HugeSpider", "a huge spider");
	SetObjectName("SewerSpider", "a sewer spider");
	SetObjectName("Mechanist", "a Mechanist");
	SetObjectName("swordsman", "a swordsman");
	SetObjectName("bowman", "a bowman");
	SetObjectName("ZombieTypes", "a zombie");
	SetObjectName("Haunt", "a haunt");
	SetObjectName("Turret", "a turret");
	SetObjectName("ham guard", "a Hammerite guard");
	SetObjectName("hamguard2", "a Hammerite guard");
	SetObjectName("ham priest", "a Hammerite priest");
	SetObjectName("ham novice", "a Hammerite novice");
	SetObjectName("ham worker", "a Hammerite worker");
	SetObjectName("FrogSplat", "an exploding frogbeast");

	if (!g_pNetMan->AmDefaultHost())
	{
		pFrobInfoProp->Delete(Arch.Creature); // disable body carrying for now
	}
}

void CGamesysManager::AvatarWeaponsInit()
{
	Arch.AvatarWeapons = g_pObjSys->GetObjectNamed("Avatar Weapons");
	Arch.AvatarBow = g_pObjSys->GetObjectNamed("Avatar Bow");
	Arch.AvatarBlackjack = g_pObjSys->GetObjectNamed("Avatar Blackjack");
	Arch.AvatarSword = g_pObjSys->GetObjectNamed("Avatar Sword");

	if (Arch.AvatarWeapons)
		AvatarWeaponsTerm();
		
	Arch.AvatarWeapons = CreateNewArchetype(g_pObjSys->GetObjectNamed("SFX"), "Avatar Weapons");
	_SetNetworkCategory(Arch.AvatarWeapons, CAT_LocalOnly);

	Arch.AvatarSword = CreateNewArchetype(Arch.AvatarWeapons, "Avatar Sword");
	_ObjSetModelName(Arch.AvatarSword, "swords");
	gWeaponExposureProp->Set(Arch.AvatarSword, 10);
	g_pESndClassProp->Copy(Arch.AvatarSword, Arch.Sword);

	Arch.AvatarBlackjack = CreateNewArchetype(Arch.AvatarWeapons, "Avatar Blackjack");
	mxs_vector vec = {0.65f, 0.65f, 0.65f};
	_ObjSetModelName(Arch.AvatarBlackjack, "blacjack");
	_ObjSetScale(Arch.AvatarBlackjack, &vec);

	Arch.AvatarBow = CreateNewArchetype(Arch.AvatarWeapons, "Avatar Bow");
	_ObjSetModelName(Arch.AvatarBow, "bow2");
	gWeaponExposureProp->Set(Arch.AvatarBow, 10);

	CacheArchetype(Arch.AvatarWeapons, "Avatar Weapons");
	CacheArchetype(Arch.AvatarBow, "Avatar Bow");
	CacheArchetype(Arch.AvatarBlackjack, "Avatar Blackjack");
	CacheArchetype(Arch.AvatarSword, "Avatar Sword");
}

void CGamesysManager::AvatarWeaponsTerm()
{
	assert(Arch.AvatarWeapons && Arch.AvatarBow && Arch.AvatarBlackjack && Arch.AvatarSword);

	g_pObjSys->Destroy(Arch.AvatarWeapons);
	g_pObjSys->Destroy(Arch.AvatarSword);
	g_pObjSys->Destroy(Arch.AvatarBlackjack);
	g_pObjSys->Destroy(Arch.AvatarBow);
}

void CGamesysManager::FakeLootInit()
{
	sLoot loot;
	//dbgassert(!Arch.FakeLoot);

	ZeroMemory(&loot, sizeof(sLoot));

	Arch.FakeLoot = CreateNewArchetype(g_pObjSys->GetObjectNamed("Swag"), "FakeLoot");

	_SetNetworkCategory(Arch.FakeLoot, CAT_LocalOnly);
	g_pLootProp->Set(Arch.FakeLoot, (void*)&loot);
}

void CGamesysManager::CreateBaseAvatar()
{
	// Create the base MP Avatar archetype, with the Garrett model
	assert(Arch.Garrett);

	int mpAvatar;
	int exists = g_pObjSys->Exists(g_pObjSys->GetObjectNamed("MP Avatar"));
	if (!exists)
		mpAvatar = CreateNewArchetype(Arch.Garrett, "MP Avatar");
	else
		mpAvatar = g_pObjSys->GetObjectNamed("MP Avatar");

	actorTagListProp->Set(mpAvatar, "player");
	_ObjSetCreatureType(mpAvatar, CT_Avatar);
	gAvatarHeightOffsetProp->Set(mpAvatar, 1.41f);

	// 7/16/10 - changed back to copying from Human as in build 210, copying from Garrett will cause players
	// to desynch when they enter water. Don't know why this was ever changed in the first place
	g_pPhysAttrProp->Copy(mpAvatar, g_pObjSys->GetObjectNamed("Human"));
	g_pPhysTypeProp->Copy(mpAvatar, g_pObjSys->GetObjectNamed("Human"));

	//g_pPhysAttrProp->Copy(mpAvatar, Arch.Garrett);
	//g_pPhysTypeProp->Copy(mpAvatar, Arch.Garrett);

	_ObjSetModelName(mpAvatar, "Garrett");

	sFrobInfo frob;
	ZeroMemory(&frob, sizeof(sFrobInfo));

	frob.worldActionFlags = FINFO_MOVE;
	pFrobInfoProp->Set(mpAvatar, &frob);

	cScriptProp* pScrProp;
	int success = g_pScriptProp->Get(mpAvatar, (void**)&pScrProp);
	assert(success && "failed to get script property");

	g_pScriptProp->Delete(mpAvatar);

	assert(g_pTraitMan->GetArchetype(mpAvatar) == Arch.Garrett);
	CacheArchetype(Arch.MPAvatar, "MP Avatar");
}

void CGamesysManager::CreateDerivedAvatars()
{
	// Create new archetypes derived from MP Avatar with differing models
	int newAvatar = CreateNewArchetype(Arch.MPAvatar, "MP Avatar 2");
	dbgassert(newAvatar && "failed to create avatar 2");

	_ObjSetModelName(newAvatar, "ThiefM02"); // ThiefMale

	newAvatar = CreateNewArchetype(Arch.MPAvatar, "MP Avatar 3");
	dbgassert(newAvatar && "failed to create avatar 3");

	_ObjSetModelName(newAvatar, "keeper01"); // KeeperAgent

	newAvatar = CreateNewArchetype(Arch.MPAvatar, "MP Avatar 4");
	dbgassert(newAvatar && "failed to create avatar 4");

	_ObjSetModelName(newAvatar, "malsev01"); // MaleServ1

	newAvatar = CreateNewArchetype(Arch.MPAvatar, "MP Avatar 5");
	dbgassert(newAvatar && "failed to create avatar 5");

	_ObjSetModelName(newAvatar, "basso"); // Basso
}

void CGamesysManager::CreateProjectilePickups()
{
	if (g_pNetMan->IsNetworkGame())
	{
		char buff[256];
		int newArch;
		const char* cycle;
		int currObject;
		SimpleQueue<int> ArchQueue;
		IObjectQuery* pQuery = g_pObjSys->Iter(2);
		int projectile = g_pObjSys->GetObjectNamed("Projectile");

		if (pQuery)
		{
			while (!pQuery->Done())
			{
				currObject = pQuery->Object();

				if (g_pTraitMan->ObjHasDonor(currObject, projectile) && g_pInventory->m_pCycleOrderProp->Get(currObject, &cycle))
				{
					ArchQueue.Push(new int(currObject));
					//ConPrintF("Equippable projectile child found: %s", _ObjEditName(currObject));
				}
				pQuery->Next();
			}
			pQuery->Release();
		}

		m_ProjectileMap.clear();

		while (ArchQueue.GetSize() > 0)
		{

			int* queued = ArchQueue.Pop();
			sprintf(buff, "pickup_%s", g_pObjSys->GetName(*queued));
			newArch = CreateNewArchetype(*queued, buff);

			_SetNetworkCategory(newArch, CAT_Hosted);
			SetFrobHandler(newArch, FROB_Host);
			//gCombineTypeProp->Delete(newArch);
			//g_pPhysInitVelProp->Delete(newArch);
			//g_pPhysInitVelProp->Create(newArch);
			//g_pPhysAttrProp->Delete(newArch);
			//g_pPhysAttrProp->Create(newArch);
			//g_pPhysTypeProp->Delete(newArch);
			//g_pPhysTypeProp->Create(newArch);

			m_ProjectileMap[newArch] = *queued; 

			delete queued;
		}
	}
}

int CGamesysManager::PickupToLocalProjectile(int pickupArch)
{
	ProjMap::iterator it = m_ProjectileMap.find(pickupArch);

	if (it != m_ProjectileMap.end())
	{
		return it->second;
	}

	return 0;
}

int CGamesysManager::LocalToPickupProjectile(int localArch)
{
	for (ProjMap::iterator it = m_ProjectileMap.begin(); it != m_ProjectileMap.end(); it++)
	{
		if (it->second == localArch)
		{
			return it->first;
		}
	}

	return 0;
}

void CGamesysManager::SetNetCategoryByName(const char* objName, eNetworkCategory cat)
{
	int obj = g_pObjSys->GetObjectNamed(objName);
	dbgassert(obj);

	if (obj)
		_SetNetworkCategory(obj, cat);
}

int CGamesysManager::SetObjectName(const char* objName, const char* name)
{
	int obj = g_pObjSys->GetObjectNamed(objName);

	if (obj)
		return SetObjectName(obj, name);
	else
	{
		DbgPrint("%s: couldn't find object %s.", __FUNCTION__, objName);
		return 0;
	}
}

int CGamesysManager::SetObjectName(int object, const char* name)
{
	dbgassert(object);

	if (object)
	{
		return g_pMultiNameProp->Set(object, name);
	}
	else
	{
		DbgPrint("%s: no object specified for name %s.", __FUNCTION__, name);
		return 0;
	}
}

DWORD CGamesysManager::GetGamesysCrc()
{
	return m_GamesysCrc;
}

void CGamesysManager::EventHandler(int eventType, void* eventData)
{
	switch (eventType)
	{
	case ET_MissionLoaded:
		m_GamesysCrc = Crc32::ScanFile(g_gamesysName);
		break;
	}
}

void CGamesysManager::InitListeners()
{
	EventManager::RegisterListener(ET_MissionLoaded, this);
}

void CGamesysManager::TermListeners()
{
	EventManager::UnregisterListener(ET_MissionLoaded, this);
}

void GamesysInit()
{
	Gamesys.InitListeners();
}

void GamesysTerm()
{
	Gamesys.TermListeners();
}
