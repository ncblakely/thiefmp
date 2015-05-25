#pragma once

enum eFieldDescType
{
	FDESC_Int = 0,
	FDESC_Bool = 1,
	FDESC_Short = 2,
	FDESC_Bitfield = 3,
	FDESC_Enum = 4,
	FDESC_CharArray = 5,
	FDESC_CharPtr,
	FDESC_VoidPtr ,
	FDESC_Point,
	FDESC_Vector,
	FDESC_Float,
	FDESC_Fix,
	FDESC_FixVec,
	FDESC_Double,
	FDESC_RGBA,
	FDESC_DoubleVec,
	FDESC_Ang,
	FDESC_AngVec,
};

struct sFieldDesc
{
	char name[32]; // 0x0
	eFieldDescType type;

	// Telliamed's defs
	unsigned long size;
	unsigned long offset;
	int  flags;
	int  min, max;
	int num_bits;
	const char* * bit_names;
};

struct sStructDesc
{
	char name[32]; // 0x0
	int size; // 0x20
	DWORD flags; // 0x24
	int nfields; // 0x28
	const sFieldDesc* pFieldDesc;
};

void __stdcall HookRegisterStructDesc(int, const sStructDesc* pDesc);
void StructDumpOnlyNames(const sStructDesc* pStructDesc);
void StructDump(const sStructDesc* pStructDesc, const void* pStruct);
