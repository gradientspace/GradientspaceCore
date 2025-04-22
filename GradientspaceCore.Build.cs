// Copyright Gradientspace Corp. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class GradientspaceCore : ModuleRules
{
	public GradientspaceCore(ReadOnlyTargetRules Target) : base(Target)
	{
        //#UEPLUGINTOOL
        bool bIsGSDevelopmentBuild = File.Exists(
            Path.GetFullPath(Path.Combine(PluginDirectory, "..", "GRADIENTSPACE_DEV_BUILD.txt")));

        if (bIsGSDevelopmentBuild) {
			PCHUsage = ModuleRules.PCHUsageMode.NoPCHs;
			bUseUnity = false;
		} else	{
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        }

		// Determine if we have the precompiled-binaries module
		string GSPrecompiledPath = Path.Combine(PluginDirectory, "source", "GradientspaceBinary");
		bool bUsingPrecompiledGSLibs = Directory.Exists(GSPrecompiledPath);

		// module.cpp uses this to determine which dll path to load from
		bool bDebugConfig = (Target.Configuration == UnrealTargetConfiguration.Debug && Target.bDebugBuildsActuallyUseDebugCRT);
		if (bDebugConfig)
			PrivateDefinitions.Add("GSCORE_USING_DEBUG");

		// if we are building inside UE, some UE versions of defines/etc are enabled via this define
		if (bUsingPrecompiledGSLibs == false)
			PublicDefinitions.Add("GS_EMBEDDED_UE_BUILD");

        UnsafeTypeCastWarningLevel = WarningLevel.Error;

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"GeometryCore"
			});
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Projects"
			});

		// add dependency on external-DLLs module if it's in use
		if (bUsingPrecompiledGSLibs)
		{
			PublicDependencyModuleNames.Add("GradientspaceBinary");
		}



    }
}
