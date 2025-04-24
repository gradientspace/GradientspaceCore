// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once


#ifdef GS_EMBEDDED_UE_BUILD

#include "HAL/Platform.h"


#else   // ! GS_EMBEDDED_UE_BUILD


#ifdef __linux__
	#ifdef GRADIENTSPACECORE_EXPORTS
	#define GRADIENTSPACECORE_API __attribute__((visibility("default")))
	#else
	#define GRADIENTSPACECORE_API 
	#endif
#else
	#ifdef GRADIENTSPACECORE_EXPORTS
	#define GRADIENTSPACECORE_API __declspec(dllexport)
	#else
	#define GRADIENTSPACECORE_API __declspec(dllimport)
	#endif
#endif


// disable some warnings (UE disables these warnings too)
#pragma warning(disable: 4251)		// X needs to have dll-interface to be used by clients of class X. 
									// This warning happens when an exported class/struct/etc has template members
									// conceivably it's possible to export the template instantiations...but messy

// disable windows min/max functions
#define NOMINMAX

#endif


// UE builds Editor with exceptions enabled, and Game with exceptions disabled... 
#ifdef GS_DISABLE_EXCEPTIONS
	#define _HAS_EXCEPTIONS 0
#endif




#ifdef GEOMETRYCORE_API
	#define GS_ENABLE_UE_TYPE_COMPATIBILITY
#endif

// this will make mesh stuff use LHS coordinate system which is appropriate for unreal (ie triangle normals will come out correctly)
#define GS_USE_LEFT_HANDED_COORDINATE_SYSTEM


// disable secure CRT warnings on Windows, because it warns about the std functions that work cross-platform
#if defined(_WIN64)
	#define _CRT_SECURE_NO_WARNINGS
#endif

// standard library includes that we use everywhere
#include <string.h>
#include <cstdint>


// linux does not have memcpy_s so just forward to memcpy for now...
#ifdef __linux__
	#define memcpy_s(a,b,c,d) memcpy((a),(c),(d))
#endif

