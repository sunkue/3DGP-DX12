// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

#define FRAMEBUFFER_WIDTH	640
#define FRAMEBUFFER_HEIGHT	480

#define DegreeToRadian(x)	float((x)*3.141592654f/180.0f)