

#include "DataAssetsThumbnailsModule.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "DataAssetsThumbnailsRenderer.h"
#include "CustomTextureDetails.h"
#include "BlueprintEditorModule.h"

IMPLEMENT_GAME_MODULE(FDataAssetsThumbnailsModule, DataAssetsThumbnails);

UE_DISABLE_OPTIMIZATION
void FDataAssetsThumbnailsModule::StartupModule()
{
    // Register custom thumbnail renderers
    UThumbnailManager::Get().RegisterCustomRenderer(UDataAsset::StaticClass(), UDataAssetsThumbnailsRenderer::StaticClass());

    FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
    GetVariableCustomizationInstanceDelegateHandle = BlueprintEditorModule.RegisterVariableCustomization(FProperty::StaticClass(), FOnGetVariableCustomizationInstance::CreateStatic(&FCustomTextureDetails::MakeInstance));
}

void FDataAssetsThumbnailsModule::ShutdownModule()
{
    // Unregister thumbnail renderers?
    if (UThumbnailManager* ThumbnailManager = UThumbnailManager::TryGet())
    {
        if (IsValid(ThumbnailManager) && (ThumbnailManager->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed) == false))
        {
            ThumbnailManager->UnregisterCustomRenderer(UDataAsset::StaticClass());
        }
    }

    FBlueprintEditorModule* BlueprintEditorModule = FModuleManager::GetModulePtr<FBlueprintEditorModule>("Kismet");
    if (BlueprintEditorModule)
    {
        BlueprintEditorModule->UnregisterVariableCustomization(FProperty::StaticClass(), GetVariableCustomizationInstanceDelegateHandle);

    }
}
UE_ENABLE_OPTIMIZATION