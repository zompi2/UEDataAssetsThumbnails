// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

using UnrealBuildTool;

public class DataAssetsThumbnailsEditor : ModuleRules
{
	public DataAssetsThumbnailsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        // To ensure IWYU
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // Required modules
        PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "DataAssetsThumbnails"
			}
		);

        // Required modules
        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Engine",
				"CoreUObject",
                "Slate",
                "SlateCore",
                "UnrealEd",
				"EditorStyle",
				"Projects",
				"LevelEditor",
				"ApplicationCore",
				"ToolMenus",
				"AssetTools",
				"Kismet",
				"ContentBrowser",
				"AssetRegistry",
				"Settings"
			}
		);
	}
}
