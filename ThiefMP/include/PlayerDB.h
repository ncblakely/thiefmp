#include "Main.h"

struct PlayerData
{
	NString playerName;
	int object;
	DPNID dpnid;
	bool connected;
};

class PlayerDB
{
public:
	PlayerDB();
	PlayerData& operator[] (int index)
	{
		index--;
		assert(index >= 0 && index < kMaxPlayers);
		return m_PlayerData[index];
	}

	const char* NameFromIndex(int index);
	const char* NameFromDPNID(DPNID dpnid);
	const char* NameFromObject(int object);

	void SetPlayerName(int clientID, const char* name);
	void SetPlayerInfo(int index, const char* name, int object);
	void ClearPlayerInfo();
protected:
	PlayerData m_PlayerData[kMaxPlayers];
};

extern PlayerDB Players;