// HookQQ.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "HookQQ.h"
#include "HookLib.h"

BOOL InitHook()
{
	oldSendto = (MYSENDTO)HookFunction(TEXT("ws2_32.dll"), "sendto", (PVOID)MySendto);
	oldMoveFileW = (MYMOVEFILEW)HookFunction(TEXT("kernel32.dll"), "MoveFileW", (PVOID)MyMoveFileW);
	
	return TRUE;
}



BOOL APIENTRY DllMain( HINSTANCE hinstDLL,
					  DWORD fdwReason,
					  LPVOID lpvReserved
					  )
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		if (!InitHook())
			return FALSE;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}