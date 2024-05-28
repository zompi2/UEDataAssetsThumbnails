// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#include "DATEditorModule.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "DATRenderer.h"
#include "DATCustomDetails.h"
#include "BlueprintEditorModule.h"

IMPLEMENT_GAME_MODULE(FDATEditorModule, DataAssetsThumbnailsEditor);

UE_DISABLE_OPTIMIZATION

void FDATEditorModule::StartupModule()
{
    // Register custom thumbnail renderers
    UThumbnailManager::Get().RegisterCustomRenderer(UDataAsset::StaticClass(), UDATRenderer::StaticClass());

    // Register custom details
    FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
    OnGetVariableCustomizationInstanceDelegateHandle = BlueprintEditorModule.RegisterVariableCustomization(FProperty::StaticClass(), FOnGetVariableCustomizationInstance::CreateStatic(&FDATCustomDetails::MakeInstance));
}

void FDATEditorModule::ShutdownModule()
{
    // Unregister thumbnail renderers
    if (UThumbnailManager* ThumbnailManager = UThumbnailManager::TryGet())
    {
        if (IsValid(ThumbnailManager) && (ThumbnailManager->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed) == false))
        {
            ThumbnailManager->UnregisterCustomRenderer(UDataAsset::StaticClass());
        }
    }

    // Unregister custom details
    FBlueprintEditorModule* BlueprintEditorModule = FModuleManager::GetModulePtr<FBlueprintEditorModule>("Kismet");
    if (BlueprintEditorModule)
    {
        BlueprintEditorModule->UnregisterVariableCustomization(FProperty::StaticClass(), OnGetVariableCustomizationInstanceDelegateHandle);

    }
}

UE_ENABLE_OPTIMIZATION