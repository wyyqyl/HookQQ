#include "stdafx.h"

#define EXTERNC extern "C" __declspec(naked) void

#pragma comment(linker, "/EXPORT:vSetDdrawflag=_MyvSetDdrawflag,@1")
#pragma comment(linker, "/EXPORT:AlphaBlend=_MyAlphaBlend,@2")
#pragma comment(linker, "/EXPORT:DllInitialize=_MyDllInitialize,@3")
#pragma comment(linker, "/EXPORT:GradientFill=_MyGradientFill,@4")
#pragma comment(linker, "/EXPORT:TransparentBlt=_MyTransparentBlt,@5")

HMODULE hModule;
FARPROC vSetDdrawflagAddr;
FARPROC AlphaBlendAddr;
FARPROC DllInitializeAddr;
FARPROC GradientFillAddr;
FARPROC TransparentBltAddr;

FARPROC WINAPI GetFuncAddress(PCSTR pszFuncName)
{
	FARPROC fpAddress;
	TCHAR tzTemp[MAX_PATH];

	fpAddress = GetProcAddress(hModule, pszFuncName);
	if (fpAddress == NULL)
	{
		wsprintf(tzTemp, TEXT("Function %hs is not found\n"), pszFuncName);
		OutputDebugText(tzTemp);
		ExitProcess(1);
	}
	//wsprintf(tzTemp, TEXT("Function %hs address: %p\n"), pszFuncName, fpAddress);
	//OutputDebugText(tzTemp);

	return fpAddress;
}

inline BOOL WINAPI Load()
{
	TCHAR tzPath[MAX_PATH];
	TCHAR tzTemp[MAX_PATH * 2];

	GetSystemDirectory(tzPath, MAX_PATH);
	lstrcat(tzPath, TEXT("\\msimg32.dll"));
	hModule = LoadLibrary(tzPath);
	if (hModule == NULL)
	{
		wsprintf(tzTemp, TEXT("Failed to load %s\n"), tzPath);
		OutputDebugText(tzTemp);
		ExitProcess(1);
	}

	GetModuleFileName(NULL, tzTemp, sizeof(tzTemp));
	wsprintf(tzTemp, TEXT("%s is loading msimg32.dll\n"), tzTemp);
	OutputDebugText(tzTemp);

	vSetDdrawflagAddr = GetFuncAddress("vSetDdrawflag");
	AlphaBlendAddr = GetFuncAddress("AlphaBlend");
	DllInitializeAddr = GetFuncAddress("DllInitialize");
	GradientFillAddr = GetFuncAddress("GradientFill");
	TransparentBltAddr = GetFuncAddress("TransparentBlt");

	return TRUE;
}

inline void WINAPI Free()
{
	if (hModule)
		FreeLibrary(hModule);
}

BOOL APIENTRY DllMain( HINSTANCE hinstDLL,
					  DWORD fdwReason,
					  LPVOID lpvReserved
					  )
{
	HMODULE hHookQQ = NULL;
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		Load();

		hHookQQ = LoadLibrary(TEXT("HookQQ.dll"));
		if (hHookQQ == NULL)
		{
			OutputDebugText(TEXT("Failed to load HookQQ.dll\n"));
			return FALSE;
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		Free();
		break;
	}
	return TRUE;
}

EXTERNC MyvSetDdrawflag()
{
	__asm jmp vSetDdrawflagAddr;
}

EXTERNC MyAlphaBlend()
{
	__asm jmp AlphaBlendAddr;
}

EXTERNC MyDllInitialize()
{
	__asm jmp DllInitializeAddr;
}

EXTERNC MyGradientFill()
{
	__asm jmp GradientFillAddr;
}

EXTERNC MyTransparentBlt()
{
	__asm jmp TransparentBltAddr;
}