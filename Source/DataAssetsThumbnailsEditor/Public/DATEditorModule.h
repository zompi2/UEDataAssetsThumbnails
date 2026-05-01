// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * Core module of the DAT (Data Asset Thumbnails) plugin.
 */
class DATAASSETSTHUMBNAILSEDITOR_API FDATEditorModule : public IModuleInterface, public FGCObject
{

public:
	
	/**
	 * Refreshes AssetTypeActions (which controls icon color and default icon) on startup
	 * and everytime when the plugins settings has changed.
	 */
	void RefreshAssetTypeActions();

	// Begin of FGCObject implementation
	void AddReferencedObjects(FReferenceCollector& Collector) override;
	FString GetReferencerName() const override;
	// End of FGCObject implementation

protected:

	// Begin of IModuleInterface implementation
	void StartupModule() override;
	void ShutdownModule() override;
	// End of IModuleInterface implementation

private:
	
	/**
	 * Helper function that registers "refresh thumbnails" buttons in content browser right click menus.
	 */
	void RegisterMenus();

	/**
	 * Utility function that refreshes thumbnails in given assets.
	 */
	void RefreshThumbnails(const TArray<FAssetData>& SelectedAssets);

	/**
	 * Utility function that refreshes thumbnails in the assets inside of the given directories.
	 */
	void RefreshThumbnailsInFolders(const TArray<FString>& SelectedPaths);

	// Delegate handle for registering Variable Customization (HasThumbnail checkbox in details panel).
	FDelegateHandle OnGetVariableCustomizationInstanceDelegateHandle;
	
	// Map of AssetDefinitions registered for specific data assets.
	TMap<TSubclassOf<class UDataAsset>, TObjectPtr<class UDATAssetDefinition>> CreatedAssetDefinitions;
};
