// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

using UnrealBuildTool;

public class DataAssetsThumbnailsEditor : ModuleRules
{
	public DataAssetsThumbnailsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("DataAssetsThumbnailsEditor/Private");

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"InputCore"
			}
		);
		
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
				"AssetRegistry"
			}
		);
	}
}
