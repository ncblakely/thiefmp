#pragma once

#include "Main.h"
#include "Engine\inc\Crc32.h"
#include "Engine\inc\ConfigManager.h"

#define DEFAULT_PORT 5198
#define DEFAULT_PORTSTR "5198"

namespace Config
{
	void ApplySettings();
	void GetSettings();
	void Reload();
}

extern ConfigManager Cfg;