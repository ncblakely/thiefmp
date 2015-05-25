#pragma once

#pragma warning (disable: 4312)

// deletion macros
#define SAFE_DELETE(p)       { delete (p);     (p)=NULL; } 
#define SAFE_DELETE_ARRAY(p) { delete[] (p);   (p)=NULL; } 
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#include ".\DataTypes.h"
#include ".\Utilities.h"
#include ".\Logger.h"

// callbacks
extern void (*AppLogCallback)(const char*);

// exception filter
LONG __stdcall RecordExceptionInfo(PEXCEPTION_POINTERS data);