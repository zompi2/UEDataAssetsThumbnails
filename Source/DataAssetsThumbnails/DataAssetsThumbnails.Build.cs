// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

using UnrealBuildTool;

public class DataAssetsThumbnails : ModuleRules
{
	public DataAssetsThumbnails(ReadOnlyTargetRules Target) : base(Target)
	{
        // To ensure IWYU
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // Required modules
        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine"
			}
		);
    }
}
