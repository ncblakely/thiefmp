typedef int BOOL;

char* PlayerAction_GetTagName(int ePlayerAction)
{
	switch (ePlayerAction)
	{
	case 8:
	case 1:
		return "WithSword 0";
	default:
		return "Player";
	}
}

char* PlayerAction_GetAnimName(int ePlayerAction, BOOL bCrouched)
{
	switch (ePlayerAction)
	{
	case 8:
		return "RangedCombat 0, WithBow 0";
	case 4:
		return "attack, meleecombat, direction 0";
	case 2:
		return "receivewound";
	case 1:
		return "Crumple 0, Die 0";
	default:
		break;
	}

	return 0;
}