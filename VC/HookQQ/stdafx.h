// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

void OutputString(const TCHAR *fmt, ...);
#ifdef _DEBUG
#define DbgPrint(_x_) OutputString _x_
#else
#define DbgPrint(_x_)
#endif

// TODO: reference additional headers your program requires here
