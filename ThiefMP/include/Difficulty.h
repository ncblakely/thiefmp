#pragma once

void Difficulty_OnMeleeAttack();
BOOL __stdcall Difficulty_GetAIAwarenessDelay(IProperty* property, int object, sAIAwareDelay** pDelay);
BOOL __stdcall Difficulty_SetAIAwarenessDelay(IProperty* property, int object, sAIAwareDelay* delay);
BOOL Difficulty_GetAIRatingProperty(IProperty* property, int object, eAIRating* rating);