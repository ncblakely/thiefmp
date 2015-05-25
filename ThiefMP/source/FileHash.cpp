/*************************************************************
* File: FileHash.cpp
* License: GPL (see license.txt in root directory)
* Copyright: 2010 Nick Blakely
*************************************************************/

#include "stdafx.h"

#include "Engine\inc\Engine.h"
#include "Engine\inc\Crc32.h"

#include "Main.h"

#define GEN_OSM_CRC 0xC78D7A81
#define GEN_OSM_REBASED_CRC 0xBBDE276B

#define CONVICT_OSM_CRC 0x47995E49
#define CONVICT_OSM_REBASED_CRC 0x658F9C23

const char* scriptWarning = "Could not find original version of a required script module. Please ensure that you have applied the Thief 2 version 1.18 patch."
			"\r\n\nIf Thief 2 has already been patched, reinstall Thief 2 Multiplayer with the \"Original Script Modules\" option checked during setup.";

void ValidateScriptModules()
{
	DWORD result;

	result = Crc32::ScanFilePart("gen.osm", 0x1000, 0x3DA00);
	if (result != GEN_OSM_CRC && result != GEN_OSM_REBASED_CRC)
	{
		MessageBox(NULL, scriptWarning, "Thief 2 Multiplayer Fatal Error", MB_OK);
		ExitProcess(-1);
	}

	result = Crc32::ScanFilePart("convict.osm", 0x1000, 0x12A00);
	if (result != CONVICT_OSM_CRC && result != CONVICT_OSM_REBASED_CRC)
	{
		MessageBox(NULL, scriptWarning, "Thief 2 Multiplayer Fatal Error", MB_OK);
		ExitProcess(-1);
	}
}