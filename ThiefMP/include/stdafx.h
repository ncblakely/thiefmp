#pragma once

// Windows includes with some useless stuff removed
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#define NOKANJI
#define NOSOUND
#define NOPROFILER
#include <windows.h>
#include "dplay8.h"
#include <dplay.h>

// STL includes
#include <string>
#include <map>
#include <vector>
#include <list>
#include <assert.h>
#include <direct.h>
#include <cmath>
// Note: this is the only dependency on the Boost library. If compiling under VS 2010, all instances of BOOST_STATIC_ASSERT can be 
// replaced with static_assert and this header removed
#include "boost\static_assert.hpp"	

// Other
#include "Engine\inc\DataTypes.h"
#include "Engine\inc\NString.h"
#include "Defines.h"
#include "Interface.h"

#ifndef _RELEASE
#define dbgassert(a) assert(a)
#else
#define dbgassert(a)  ((void)0)
#endif

#define SAFE_DELETE(p)       { delete (p);     (p)=NULL; } 
#define SAFE_DELETE_ARRAY(p) { delete[] (p);   (p)=NULL; } 
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

using namespace std;