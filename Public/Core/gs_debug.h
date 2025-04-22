// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

//#include "GradientspacePlatform.h"
//// gross...why do we need this again?
//#ifdef WITH_ENGINE
//#include "Windows/AllowWindowsPlatformTypes.h"
//#endif
////#include "windows.h"
//// only on WIN32 platform...
//#include <debugapi.h>
//
//#ifdef WITH_ENGINE
//#include "Windows/HideWindowsPlatformTypes.h"
//#endif

// seems like this is all we need on windows, and does not bring in windows.h garbage
#include <intrin.h>
#define PLATFORM_BREAK() (__nop(), __debugbreak())

#define gs_debug_assert( expr )   { if (!(expr)) PLATFORM_BREAK(); }
#define gs_runtime_assert( expr ) { if (!(expr)) PLATFORM_BREAK(); }

#define GS_DISABLE_OPTIMIZATION __pragma(optimize("",off))
#define GS_ENABLE_OPTIMIZATION __pragma(optimize("",on))


#ifdef GSUE_LINKING_INSIDE_UE
#define GS_LOG(Message, ...) UE_LOG(LogTemp, Warning, TEXT(Message), ##__VA_ARGS__)
#else
#define GS_LOG(Message, ...) printf(Message, ##__VA_ARGS__)
#endif
