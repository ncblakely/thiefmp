#pragma once

#include "Engine\inc\EventManager.h"
#include "NetProperties.h"

void SetFrobHandler(int schema, enum eFrobHandler handlerType);

class ArchCache
{
public:
	ArchCache();

	int AvatarWeapons, AvatarBow, AvatarSword, AvatarBlackjack, PLYR_WEAPON, PLYR_DAMAGE, AI_SPEECH, 
		pinset, tumblerset, Cameras, Projectile, LockPick, Marker, RopeArrowRope, Crystal, Sword, Blackjack, IsLoot,
		Garrett, MPAvatar, SecCamera2, climbable, Phantom, Creature, FakeLoot;
};

typedef std::map<int, int> ProjMap;
class CGamesysManager : public IEventListener
{
public:

	int CreateNewArchetype(int donorArch, const char* name);
	void CacheArchetype(int& var, const char* archName);
	void InitializeArchCache(DWORD flags);

	void AvatarWeaponsInit();
	void AvatarWeaponsTerm();
	void FakeLootInit();
	void CreateBaseAvatar();
	void CreateDerivedAvatars();
	void CreateProjectilePickups();

	DWORD GetGamesysCrc();
	void InitListeners();
	void TermListeners();

	int PickupToLocalProjectile(int pickupArch);
	int LocalToPickupProjectile(int localArch);

	void SetNetCategoryByName(const char* objName, eNetworkCategory cat);
	int SetObjectName(const char* object, const char* name);
	int SetObjectName(int object, const char* name);

	virtual void EventHandler(int eventType, void* eventData);

	ArchCache Arch;

protected:
	ProjMap m_ProjectileMap;
	DWORD m_GamesysCrc;
};

extern CGamesysManager Gamesys;