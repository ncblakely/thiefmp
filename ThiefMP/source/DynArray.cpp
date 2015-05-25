/*************************************************************
* File: DynArray.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Main.h"
#include "DynArray.h"

void* drk_realloc(void* pData, size_t size)
{
	return g_pMalloc->Realloc(pData, size);
}

void* drk_nh_malloc(size_t size)
{
	return g_pMalloc->Alloc(size);
}

void drk_free(void* pData)
{
	g_pMalloc->Free(pData);
}

int cDABaseSrvFns::DoResize(void** ppData, unsigned int size, unsigned int P2)
{
	void* pData = *ppData;

	if (P2)
	{
		if (*ppData)
			pData = drk_realloc(pData, size * P2);
		else
			pData = drk_nh_malloc(size * P2);
		
		if (pData == NULL)
		{
			return 0;
		}

		*ppData = pData;
		return 1;
	}
	else
	{
		if (*ppData)
		{
			drk_free(*ppData);
			*ppData = NULL;
		}
	}

	return 1;
}