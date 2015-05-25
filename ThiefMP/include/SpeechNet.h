#pragma once

void SpeechNetHandler(int schemaID, bool bPlayedHere, const sSchemaCallParams* pParams);
void SpeechBroadcast(CNetMsg_AiSpeech& msg);
int GetAIVoice(int object);