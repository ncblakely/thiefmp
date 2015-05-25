/*************************************************************
* File: Difficulty.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
* Purpose: Adjusts game difficulty depending on status of the
* "IncreaseDifficulty" option.
*************************************************************/

#include "stdafx.h"

#include "Main.h"

static const float kAwareDelayDecreasePct = 0.75f;

bool IsDifficultyIncreased()
{
	if (!g_pDarkNet)
		return false;
	
	return g_pDarkNet->GetSessionInfo().increaseDifficulty;
}

void Difficulty_OnMeleeAttack()
{
	if (IsDifficultyIncreased())
	{	
		// Force standing motion to prevent attacking while leaning
		_SetLeaningState(0);
		_PlayerMotionActivate(kMoNormal);
	}
}

// Called when getting awareness delay for an AI. If AI is using default delay, returns default awareness delay
// scaled by kAwareDelayDecreasePct.
// If the AI isn't using the default delay, it's already been scaled by Difficulty_SetAIAwarenessDelay.
BOOL __stdcall Difficulty_GetAIAwarenessDelay(IProperty* property, int object, sAIAwareDelay** pDelay)
{
	static const float kAwareDelayDecreasePct = 0.5f;
	static sAIAwareDelay s_DecreasedDefaultAwareDelay = 
	{
		(int)(((float)g_pAIDefAwareDelay->reactTimeToTwo) * kAwareDelayDecreasePct),
		(int)(((float)g_pAIDefAwareDelay->reactTimeToThree) * kAwareDelayDecreasePct),
		g_pAIDefAwareDelay->retriggerTwoDelay,
		g_pAIDefAwareDelay->retriggerThreeDelay,
		g_pAIDefAwareDelay->ignoreDelayRange,
	};

	BOOL success = cStoredProperty_GetVoid(property, object, (void**)pDelay); // Get AI's original awareness delay

	if (!success && IsDifficultyIncreased())
	{
		*pDelay = &s_DecreasedDefaultAwareDelay; // return decreased delay
	}

	return success;
}

// Called when setting awareness delay for an AI. Scales delay values by kAwareDelayDecreasePct.
BOOL __stdcall Difficulty_SetAIAwarenessDelay(IProperty* property, int object, sAIAwareDelay* delay)
{
	if (IsDifficultyIncreased())
	{
		// decrease delay
		delay->reactTimeToTwo = (int)(((float)delay->reactTimeToTwo) * kAwareDelayDecreasePct);
		delay->reactTimeToThree = (int)(((float)delay->reactTimeToThree) * kAwareDelayDecreasePct);
	}

	BOOL success = cStoredProperty_Set(property, object, delay); // Get AI's original awareness delay

	return success;
}

BOOL Difficulty_GetAIRatingProperty(IProperty* property, int object, eAIRating* rating)
{
	BOOL success = cStoredProperty_GetInt(property, object, (int*)rating); // Get AI's original defensiveness
	//if (!success && IsDifficultyIncreased())
	//{
	//	if (property == g_pAIDefensiveStatProp)
	//	{
	//		*rating = AIRATING_AboveAverage; // default to Above Average instead of Average
	//	}
	//}

	return success;
}