// Copyright Gradientspace Corp. All Rights Reserved.
#include "GradientspaceCoreModule.h"

#ifdef WITH_ENGINE

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

#define LOCTEXT_NAMESPACE "FGradientspaceCoreModule"


void FGradientspaceCoreModule::StartupModule()
{
#ifndef GS_EMBEDDED_UE_BUILD
	FString BaseDir = IPluginManager::Get().FindPlugin("GradientspaceUEToolbox")->GetBaseDir();
#ifdef GSCORE_USING_DEBUG
	FString DLLPath = FPaths::Combine(*BaseDir, "gradientspace_distrib/Win64/Debug/gradientspace_core.dll");
#else
	FString DLLPath = FPaths::Combine(*BaseDir, "gradientspace_distrib/Win64/Release/gradientspace_core.dll");
#endif
	if ( FPaths::FileExists(*DLLPath) )
		PrecompiledDLLHandle = FPlatformProcess::GetDllHandle(*DLLPath);
	else
		UE_LOG(LogTemp, Error, TEXT("Could not find GradientspaceCore DLL at %s"), *DLLPath);
#endif
}

void FGradientspaceCoreModule::ShutdownModule()
{
	if (PrecompiledDLLHandle != nullptr)
		FPlatformProcess::FreeDllHandle(PrecompiledDLLHandle);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGradientspaceCoreModule, GradientspaceCore)

#endif
