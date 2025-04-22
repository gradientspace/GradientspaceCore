// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

// assume this means we are compiling inside UE (is there a better option?)
#ifdef WITH_ENGINE

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FGradientspaceCoreModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void* PrecompiledDLLHandle = nullptr;
};


#endif
