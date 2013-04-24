// stdafx.cpp : source file that includes just the standard includes
// HookQQ.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <stdarg.h>
#include <new>
#include <tchar.h>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

void OutputString(const TCHAR *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int len = _vsctprintf(fmt, args);
    LPTSTR buffer = NULL;
    try {
        buffer = (LPTSTR)new TCHAR[len + 1];
    } catch (std::bad_alloc) {
        OutputDebugString(L"failed to new for Print()\n");
        va_end(args);
    }
    _vsntprintf_s(buffer, len + 1, _TRUNCATE, fmt, args);
    va_end(args);
    OutputDebugString(buffer);
    delete []buffer;
}
