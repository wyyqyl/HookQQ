#pragma once

PVOID HookFunction(LPTSTR ModuleName, LPCSTR FunctionName, PVOID MyFunction);
BOOL UnhookFunction(LPTSTR ModuleName, LPCSTR FunctionName, PVOID proxyFunction);
BOOL IsFunctionHooked(LPTSTR ModuleName, LPCSTR FunctionName);