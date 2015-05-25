/*************************************************************
* File: DarkStat.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Hooks DarkStat functions.
*************************************************************/

#include "stdafx.h"

#include "Imports.h"
#include "Client.h"

// Replacement functions that call doSet/doCreate instead of Set/Create so that statistics are not sent over the network

void DarkStatIntSet(char* statName, int statVal)
{
	IPtr<IQuestData> pQuestData = _AppGetAggregated(IID_IQuestData);

	if (pQuestData->Exists(statName))
		pQuestData->doSet(statName, statVal);
	else
		pQuestData->doCreate(statName, statVal, 0);

}

void DarkStatIntAdd(char* statName, int newVal)
{
	IPtr<IQuestData> pQuestData = _AppGetAggregated(IID_IQuestData);

	if (pQuestData->Exists(statName))
	{
		int currVal = pQuestData->Get(statName);
		pQuestData->doSet(statName, currVal + newVal);
	}
	else
		pQuestData->doCreate(statName, newVal, 0);
}