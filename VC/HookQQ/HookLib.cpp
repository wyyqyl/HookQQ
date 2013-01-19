#include "stdafx.h"
#include "HookLib.h"
#include "Ldasm.h"

BYTE JumpCode[] = {0x68,0x00,0x00,0x00,0x00,0xC3};
BYTE JumpbackCode[] = {0x68,0x00,0x00,0x00,0x00,0xC3};

#define JumpCodeSize sizeof(JumpCode)
#define JumpbackCodeSize sizeof(JumpbackCode)

PVOID HookFunction(LPTSTR ModuleName, LPCSTR FunctionName, PVOID MyFunction)
{
	PVOID oldFunction = NULL;
	PVOID proxyFunction = NULL;
	LPBYTE opCode = NULL;
	DWORD backupLen = 0;
	DWORD oldProtect = 0;
	TCHAR tzTemp[MAX_PATH] = {0};

	// Get original function address
	oldFunction = GetProcAddress(GetModuleHandle(ModuleName), FunctionName);
	if (!oldFunction)
	{
		wsprintf(tzTemp, TEXT("Failed to find the function: %hs\n"), FunctionName);
		OutputDebugText(tzTemp);
		return NULL;
	}

	// Get the exact length
	while (backupLen < JumpCodeSize)
		backupLen += size_of_code((LPBYTE)((DWORD)oldFunction + backupLen), &opCode);

	// Fill the data
	*(DWORD *)(JumpCode + 1) = (DWORD)MyFunction;
	*(DWORD *)(JumpbackCode + 1) = (DWORD)oldFunction + backupLen;

	// Allocate space for proxy function
	proxyFunction = VirtualAlloc(NULL, backupLen + JumpCodeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!proxyFunction)
	{
		wsprintf(tzTemp, TEXT("Failed to allocate space for the function: %hs\n"), FunctionName);
		OutputDebugText(tzTemp);
		return NULL;
	}
	 // Fill proxy function and flush instructions
	RtlCopyMemory(proxyFunction, oldFunction, backupLen);
	RtlCopyMemory((PVOID)((DWORD)proxyFunction + backupLen), JumpbackCode, JumpbackCodeSize);
	FlushInstructionCache(GetModuleHandle(NULL), proxyFunction, backupLen + JumpCodeSize);

	// Modify original function
	VirtualProtect(oldFunction, JumpCodeSize, PAGE_EXECUTE_READWRITE, &oldProtect);
	RtlCopyMemory(oldFunction, JumpCode, JumpCodeSize);
	VirtualProtect(oldFunction, JumpCodeSize, oldProtect, &oldProtect);
	FlushInstructionCache(GetModuleHandle(NULL), oldFunction, JumpCodeSize);

	return proxyFunction;
}

BOOL UnhookFunction(LPTSTR ModuleName, LPCSTR FunctionName, PVOID proxyFunction)
{
	PVOID oldFunction = NULL;
	DWORD oldProtect = 0;
	TCHAR tzTemp[MAX_PATH] = {0};

	oldFunction = GetProcAddress(GetModuleHandle(ModuleName), FunctionName);
	if (!oldFunction)
	{
		wsprintf(tzTemp, TEXT("Failed to find the function: %hs\n"), FunctionName);
		OutputDebugText(tzTemp);
		return FALSE;
	}

	// Recover the function
	VirtualProtect(oldFunction, JumpCodeSize, PAGE_EXECUTE_READWRITE, &oldProtect);
	RtlCopyMemory(oldFunction, proxyFunction, JumpCodeSize);
	VirtualProtect(oldFunction, JumpCodeSize, oldProtect, &oldProtect);
	FlushInstructionCache(GetModuleHandle(NULL), oldFunction, JumpCodeSize);

	if (!VirtualFree(proxyFunction, 0, MEM_RELEASE))
	{
		wsprintf(tzTemp, TEXT("Failed to free memory for the function: %hs\n"), FunctionName);
		OutputDebugText(tzTemp);
	}

	return TRUE;
}

BOOL IsFunctionHooked(LPTSTR ModuleName, LPCSTR FunctionName)
{
	PVOID oldFunction = NULL;
	DWORD OldProtect = 0;
	DWORD index = 0;
	TCHAR tzTemp[MAX_PATH] = {0};

	oldFunction = GetProcAddress(GetModuleHandle(ModuleName), FunctionName);
	if (!oldFunction)
	{
		wsprintf(tzTemp, TEXT("Failed to find the function: %hs\n"), FunctionName);
		OutputDebugText(tzTemp);
		return FALSE;
	}

	for (index = 0; index < JumpCodeSize; ++index)
	{
		if (((LPBYTE)oldFunction)[index] != JumpCode[index])
		{
			wsprintf(tzTemp, TEXT("Function: %hs is not hooked\n"), FunctionName);
			OutputDebugText(tzTemp);
			return FALSE;
		}
	}

	return TRUE;
}