#pragma once

#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

typedef int (WINAPI *MYSENDTO)(SOCKET, const char FAR *, int, int, const struct sockaddr FAR *, int);
typedef BOOL (WINAPI *MYMOVEFILEW)(LPCWSTR, LPCWSTR);

int WINAPI MySendto(SOCKET s, const char FAR * buf, int len,
					int flags, const struct sockaddr FAR * to, int tolen);
BOOL WINAPI MyMoveFileW(LPCWSTR, LPCWSTR);

extern MYSENDTO oldSendto;
extern MYMOVEFILEW oldMoveFileW;