// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class DATAASSETSTHUMBNAILSEDITOR_API FDATEditorModule : public IModuleInterface
{

public:

	void RefreshAssetTypeActions();

protected:

	void StartupModule() override;
	void ShutdownModule() override;

private:
	
	void RegisterMenus();
	void RefreshThumbnails(const TArray<FAssetData>& SelectedAssets);
	void RefreshThumbnailsInFolders(const TArray<FString>& SelectedPaths);
	FDelegateHandle OnGetVariableCustomizationInstanceDelegateHandle;
	TMap<TSubclassOf<class UDataAsset>, TSharedRef<class FDATAssetTypeActions>> CreatedAssetTypeActions;
};
