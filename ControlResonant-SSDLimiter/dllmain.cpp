// dllmain.cpp : Definiuje punkt wejścia dla aplikacji DLL.
#include <Windows.h>
#include <stdio.h>

#include "hooks.h"

void* Real_DwmSetWindowAttribute = NULL;

bool hooked = false;
void LoadHooks() {
    if (!hooked) {
        Real_DwmSetWindowAttribute = GetProcAddress(LoadLibraryW(L"C:\\Windows\\System32\\dwmapi.dll"), "DwmSetWindowAttribute");
        hook();
        hooked = true;
    }
    //FILE* f = NULL;
    //fopen_s(&f, "out.txt", "w");
    //fwrite("--cr-ssdlimiter loaded", 1, 22, f);
    //fclose(f);
}

extern "C" {
    __declspec(dllexport) HRESULT WINAPI DwmSetWindowAttribute(HWND hwnd, DWORD a, LPCVOID b, DWORD c) {
        if (Real_DwmSetWindowAttribute == NULL) {
            return E_FAIL;
        }
        return ((HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD))Real_DwmSetWindowAttribute)(hwnd, a, b, c);
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        LoadHooks();
        break;
    case DLL_PROCESS_DETACH:
        unhook();
        break;
    case DLL_THREAD_DETACH:
    case DLL_THREAD_ATTACH:
        break;
    }
    return TRUE;
}

