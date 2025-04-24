// Copyright Gradientspace Corp. All Rights Reserved.

// only need this code if we are building a standalone DLL
#ifdef GRADIENTSPACECORE_EXPORTS

// only have this on windows...
#if defined(_WIN64)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

#endif  // _WIN64
#endif  // GRADIENTSPACECORE_EXPORTS


