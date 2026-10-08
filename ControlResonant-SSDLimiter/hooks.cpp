#include <atomic>
#include <stdint.h>
#include <fstream>
#include <string>

#include "hooks.h"
#include "detours/detours.h"

uint64_t sleepThreshold = 1000000;
uint32_t sleepDuration = 1;

static sig_ReadFile Real_ReadFile = ReadFile;
BOOL WINAPI Hook_ReadFile(
    HANDLE       hFile,
    LPVOID       lpBuffer,
    DWORD        nNumberOfBytesToRead,
    LPDWORD      lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
) {
    static std::atomic<uint64_t> readCount = 0;
    readCount += nNumberOfBytesToRead;
    if (readCount >= sleepThreshold) {
        readCount = 0;
        Sleep(sleepDuration);
    }
    return Real_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}


void loadConfig() {

    std::ifstream f("ssdlimiter.cfg");
    if (f.good()) {
        
        for (std::string line; std::getline(f, line); ) {
            if (line.starts_with("wait_threshold=")) {
                try {
                    sleepThreshold = std::stoull(line.substr(15));
                }
                catch (std::exception&) { }
            }
            else if (line.starts_with("wait_duration=")) {
                try {
                    sleepDuration = std::stoull(line.substr(14));
                }
                catch (std::exception&) {}
            }
        }
    }
    else {
        std::ofstream f("ssdlimiter.cfg");
        f << "[SSDLimiter]\n";
        f << "; Wait every <n> bytes read:\n";
        f << "wait_threshold=" << sleepThreshold << "\n";
        f << "; Wait <n> MS:\n";
        f << "wait_duration=" << sleepDuration << "\n";
        f.close();
    }
}

void hook()
{
    loadConfig();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach((PVOID*)&Real_ReadFile, Hook_ReadFile);

    DetourTransactionCommit();
}

void unhook()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourDetach((PVOID*)&Real_ReadFile, Hook_ReadFile);

    DetourTransactionCommit();
}
