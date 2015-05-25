/*************************************************************
* File: StructDesc.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"
//
#include "Main.h"
#include "StructDesc.h"
//
void StructDumpOnlyNames(const sStructDesc* pStructDesc)
{
	IPtr<IStructDescTools> pTools = _AppGetAggregated(IID_IStructDescTools);

	void* pVoids[100];
	ZeroMemory(pVoids, 100 * 4);

	pTools->Dump(pStructDesc, pVoids);
}

void StructDump(const sStructDesc* pStructDesc, const void* pStruct)
{
	IPtr<IStructDescTools> pTools = _AppGetAggregated(IID_IStructDescTools);

	pTools->Dump(pStructDesc, pStruct);
}

//void __stdcall HookRegisterStructDesc(int, const sStructDesc* pDesc)
//{
//	FILE *f = fopen("StructDescs.txt", "at");
//
//	fprintf(f, "(sStructDesc*)0x%X, // %s\n", pDesc, pDesc->name);
//
//	
//	fclose(f);
//}
//
//void StructDescDumpAllEnums()
//{
//	FILE *f = fopen("GameEnums.txt", "at");
//
//	for (int descidx = 0; descidx < 1024; descidx++)
//	{
//		if (!pDescs[descidx])
//			break;
//
//		for (int fieldidx = 0; fieldidx < pDescs[descidx]->nfields; fieldidx++)
//		{
//			if (pDescs[descidx]->pFieldDesc[fieldidx].type == FDESC_Enum)
//			{
//				Log.Print("Field desc index %d for struct desc %s is an enum.", fieldidx, pDescs[descidx]->name);
//
//				fprintf(f, "enum %s::%s\n{\n", pDescs[descidx]->name, pDescs[descidx]->pFieldDesc[fieldidx].name);
//
//				for (int bitnameidx = 0; bitnameidx < pDescs[descidx]->pFieldDesc[fieldidx].num_bits; bitnameidx++)
//				{
//					fprintf(f, "\t%s = %d,\n", pDescs[descidx]->pFieldDesc[fieldidx].bit_names[bitnameidx], bitnameidx);
//				}
//
//				fprintf(f, "};\n\n");
//			}
//		}
//	}
//
//	fclose(f);
//}