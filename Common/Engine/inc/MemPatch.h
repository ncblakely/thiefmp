#pragma once

enum PatchType
{
	PatchType_Normal = 0, // *Must* be 0 for backwards compat.
	PatchType_RedirectCall,
};

struct PtrPatchList
{
	DWORD pAddress;
	void *pFunction;
	PatchType type;
	DWORD oldAddress;
};

struct RawPatchList
{
	DWORD pAddress;
	__int64 patchData;
	uint dataSize;
	__int64 oldData;
};

struct PatchDataListInfo
{
	uint size;
	RawPatchList* address;
	bool bPatched;
};

enum CallbackType
{
	CBTYPE_Call,
	CBTYPE_Jump,
};

#define PATCHARRAY_END {0xFFFFFFFF, NULL}

class MemPatcher
{
public:
	static void WritePtr(PtrPatchList* pList);
	static void WritePtr(PtrPatchList* pList, HMODULE hModPatch, ulong baseAddress) ;
	static void WriteRaw(RawPatchList* pList, HMODULE hModPatch, ulong baseAddress);
	static void RestorePtr(PtrPatchList* pList);
	static void RestoreRaw(RawPatchList* pList);
};

void	SafeWrite(void *pAddress, void *pData, int size);
void	SafeWriteData(void *pAddress, __int64 hexData, int size);
bool SafeWriteRelative(DWORD offset, HMODULE hMod, void* pMem, int size);
bool SafeWriteRelative(DWORD offset, const char* moduleName, void* pMem, int size);
void	SafeRead(void *pAddress, void *pData, int size);
void InstallCallback(unsigned long patchAddress, unsigned long ptrAddress, CallbackType type, int nopsBefore, int nopsAfter);