#pragma once
#include <Windows.h>

typedef BOOL(WINAPI* sig_ReadFile)(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped);

void hook();
void unhook();