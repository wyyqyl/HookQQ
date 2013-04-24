#include "stdafx.h"
#include "HookQQ.h"
#include <WinInet.h>

#pragma comment(lib, "wininet.lib")

MYSENDTO oldSendto = NULL;
MYMOVEFILEW oldMoveFileW = NULL;

BOOL GetPosition(LPSTR lpIPAddress, LPWSTR lpPosition, size_t length)
{
	HINTERNET hInt  = NULL;
	HINTERNET hUrl = NULL;
	TCHAR tzUrl[100] = {0};
	CHAR szFile[0x6000] = {0};
	DWORD dwFileLen = 0;

	hInt = InternetOpen(TEXT("HookQQ"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hInt == NULL)
	{
		DbgPrint((TEXT("InternetOpen error code: %x\n"), GetLastError()));
		return FALSE;
	}

	wsprintf(tzUrl, TEXT("http://ip.chinaz.com/?IP=%hs"), lpIPAddress);
	hUrl = InternetOpenUrl(hInt, tzUrl, NULL, 0, INTERNET_FLAG_NEED_FILE, 0);
	if (hUrl == NULL)
	{
		DbgPrint((TEXT("InternetOpenUrl error code: %x\n"), GetLastError()));
		InternetCloseHandle(hInt);
		return FALSE;
	}

	while (InternetReadFile(hUrl, szFile, sizeof(szFile), &dwFileLen))
	{
		LPSTR lpStart = NULL;
		LPSTR lpEnd = NULL;
		WCHAR wzPosition[100] = {0};

		if (dwFileLen < 0x5000)
			continue;

		lpStart = strstr(szFile, "==>>");
		lpStart = strstr(lpStart + 1, "==>") + 5;
		lpEnd = strstr(lpStart, "</strong>");
		*lpEnd = 0;
		MultiByteToWideChar(CP_UTF8, 0, lpStart, lpEnd - lpStart + 1, lpPosition, length);
		break;
	}

	InternetCloseHandle(hUrl);
	InternetCloseHandle(hInt);

	return TRUE;
}

int WINAPI MySendto(SOCKET s, const char FAR * buf, int len,
					int flags, const struct sockaddr FAR * to, int tolen)
{
	CHAR *QQIP = NULL;
	DWORD QQID = 0;
	int index = 0;
	WCHAR wzPosition[100] = {0};
	TCHAR tzTemp[0x800] = {0};

	QQIP = inet_ntoa(((sockaddr_in *)to)->sin_addr);
	wsprintf(tzTemp, TEXT("Len: %d, IP: %hs\n"), len, QQIP);
	for (index = 0; index < len; ++index)
		wsprintf(tzTemp, TEXT("%s%02X"), tzTemp, buf[index]);
	OutputString((tzTemp));

	if (len == 27 && *buf == 3)
	{
		QQID = (BYTE)buf[26] | (((BYTE)buf[25] | (((BYTE)buf[24] | ((BYTE)buf[23] << 8)) << 8)) << 8);
		GetPosition(QQIP, wzPosition, sizeof(wzPosition));
		OutputString(TEXT("Position: %ls, QQID: %d\n"), wzPosition, QQID);
	}
	OutputString(TEXT("\n"));
	return oldSendto(s, buf, len, flags, to, tolen);
}

BOOL WINAPI MyMoveFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName)
{
	BOOL ret = FALSE;

	if (wcsstr(lpExistingFileName, L"msimg32.dll"))
		DbgPrint((TEXT("Calling MoveFileW with msimg32.dll\n")));
	else
		ret = oldMoveFileW(lpExistingFileName, lpNewFileName);

	return ret;
}