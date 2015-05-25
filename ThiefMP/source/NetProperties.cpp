/*************************************************************
* File: NetProperties.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Defines new multiplayer-specific properties.
*************************************************************/

#include "stdafx.h"

#include "Main.h"

const sPropertyDesc g_NamePropDesc = { "T2MP_Name", 0, 0, 0, 2, 1, "Multiplayer", "Multiplayer object name", 1};

IStringProperty* g_pMultiNameProp = NULL;

void MakeNonNetworkable(sPropertyDesc* pDesc)
{
	FlagSet(pDesc->netFlags, PROP_NONETWORK);
}

void PatchPropDescs()
{
	// Some properties need to be made non-networkable for various reasons
	// Changes in state for these properties will not be broadcasted to other players

	MakeNonNetworkable(g_ModelNameDesc);
	MakeNonNetworkable(g_MeshAttachDesc);
	MakeNonNetworkable(g_PickStateDesc);
	MakeNonNetworkable(g_CurWpnDmgDesc);
	MakeNonNetworkable(g_InvBeingTakenDesc);
	MakeNonNetworkable(g_SelfIllumDesc);
	MakeNonNetworkable(g_AmbientHackedDesc);
	MakeNonNetworkable(g_StackCountDesc);
	MakeNonNetworkable(g_DeathStageDesc);
}

void NetPropertiesInit()
{
#if (GAME == GAME_THIEF)
	g_pMultiNameProp = _CreateStringProperty(&g_NamePropDesc, 2);
#endif
}

void NetPropertiesTerm()
{
#if (GAME == GAME_THIEF)
	g_pMultiNameProp->Release();
#endif
}