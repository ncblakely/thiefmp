#pragma once

void SchemaPlayAtObj(int schemaID, int objectID);
void DoEquipSound(sGhostRemote* ghost);
void DoUnequipSound(sGhostRemote* ghost);

int __stdcall OnGenerateSoundVec(mxs_vector* vec, int P1, int P2, char* soundName, float attenuation, sfx_parm* parms, int P3, int P4);
int __stdcall  OnGenerateSoundObj(int objectID, int schemaID, char* soundName,float atten, struct sfx_parm* parms, int P3, int P4);
int __stdcall OnGenerateSound(int schemaID, char* soundName, sfx_parm* parms);
void OnSpeechHalt(int object);
sSchemaPlay* SchemaIDPlay(int schemaID, sSchemaCallParams* pParams, void* P3);
sSchemaPlay* NetSchemaPlaySample(int schemaID, int sample, sSchemaCallParams* pParams, void* P3);