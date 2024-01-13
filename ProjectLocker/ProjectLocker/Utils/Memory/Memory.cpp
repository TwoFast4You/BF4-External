#include "Memory.h"

bool Memory::Init()
{
    // Find BF4 Window
    GameHwnd = FindWindowA(NULL, "Battlefield 4");
    if (!GameHwnd)
    {
        MessageBoxA(nullptr, "Can't find Battlefield 4", "ERROR", MB_TOPMOST | MB_OK);
        return false;
    }

    GetWindowThreadProcessId(GameHwnd, &PID);

    // Get pHandle
    pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    if (!pHandle)
    {
        MessageBoxA(nullptr, "Can't get ProcessHandle", "ERROR", MB_TOPMOST | MB_OK);
        return false;
    }

    return true;
}

Memory m;