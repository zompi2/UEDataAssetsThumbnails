// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#include "DATEditorModule.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "DATRenderer.h"
#include "DATCustomDetails.h"
#include "BlueprintEditorModule.h"
#include "ContentBrowserMenuContexts.h"
#include "ObjectTools.h"
#include "FileHelpers.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ISettingsModule.h"
#include "DATSettings.h"
#include "DATAssetTypeActions.h"
#include "AssetToolsModule.h"

IMPLEMENT_GAME_MODULE(FDATEditorModule, DataAssetsThumbnailsEditor);

UE_DISABLE_OPTIMIZATION

void FDATEditorModule::StartupModule()
{
    // Register custom thumbnail renderers
    UThumbnailManager::Get().RegisterCustomRenderer(UDataAsset::StaticClass(), UDATRenderer::StaticClass());

    // Register custom details
    FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
    OnGetVariableCustomizationInstanceDelegateHandle = BlueprintEditorModule.RegisterVariableCustomization(FProperty::StaticClass(), FOnGetVariableCustomizationInstance::CreateStatic(&FDATCustomDetails::MakeInstance));

    // Register options for refreshing data asset thumbnails
    UToolMenus::Get()->RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDATEditorModule::RegisterMenus));

    // Register settings
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
    if (SettingsModule)
    {
        SettingsModule->RegisterSettings("Project", "Plugins", "DAT",
            INVTEXT("Data Asset Thumbnails"),
            INVTEXT("Global settings for Data Asset Thumbnails"),
            GetMutableDefault<UDATSettings>()
        );
    }

    // Register asset action types
    RefreshAssetTypeActions();
}

void FDATEditorModule::ShutdownModule()
{
    // Unregister settings
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Plugins", "DAT");
    }

    // Unregister asset action types
    if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
        for (auto RegisteredAssetType : CreatedAssetTypeActions)
        {
            AssetTools.UnregisterAssetTypeActions(RegisteredAssetType.Value);
        }
    }
    CreatedAssetTypeActions.Empty();

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

    // remove menu extension
    UToolMenus::UnregisterOwner(this);
}

void FDATEditorModule::RefreshAssetTypeActions()
{
    UDATSettings* DATSettings = UDATSettings::Get();
    if (DATSettings)
    {
        IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
        TSet<TSubclassOf<UDataAsset>> DesiredClasses;

        for (auto& ColorOverride : DATSettings->GlobalOverrides)
        {
            if (ColorOverride.Key && ColorOverride.Value.bOverrideColor)
            {
                DesiredClasses.Add(ColorOverride.Key);
                if (CreatedAssetTypeActions.Contains(ColorOverride.Key))
                {
                    CreatedAssetTypeActions[ColorOverride.Key]->DataAssetColor = ColorOverride.Value.Color;
                }
                else
                {
                    TSharedRef<FDATAssetTypeActions> NewAction = MakeShared<FDATAssetTypeActions>();
                    NewAction->DataAssetClass = ColorOverride.Key;
                    NewAction->DataAssetColor = ColorOverride.Value.Color;

                    CreatedAssetTypeActions.Add(ColorOverride.Key, NewAction);
                    AssetTools.RegisterAssetTypeActions(NewAction);
                }
            }
        }

        TArray<TSubclassOf<UDataAsset>> ClassesToUnregister;
        for (auto& AssetActionType : CreatedAssetTypeActions)
        {
            if (DesiredClasses.Contains(AssetActionType.Key) == false)
            {
                ClassesToUnregister.Add(AssetActionType.Key);
            }
        }

        for (TSubclassOf<UDataAsset> ClassToUnregister : ClassesToUnregister)
        {
            AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[ClassToUnregister]);
            CreatedAssetTypeActions.Remove(ClassToUnregister);
        }
    }
}

// Check DataValidationModule.cpp for references
void FDATEditorModule::RegisterMenus()
{
    FToolMenuOwnerScoped OwnerScoped(this);

    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.AssetActionsSubMenu");
        FToolMenuSection& Section = Menu->FindOrAddSection("AssetContextAdvancedActions");

        FToolUIAction Action;
        Action.ExecuteAction = FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext& InContext)
        {
            if (const UContentBrowserAssetContextMenuContext* Context = InContext.FindContext<UContentBrowserAssetContextMenuContext>())
            {
                RefreshThumbnails(Context->SelectedAssets);
            }
        });
        Action.IsActionVisibleDelegate = FToolMenuIsActionButtonVisible::CreateLambda([](const FToolMenuContext& InContext)
        {
            if (const UContentBrowserAssetContextMenuContext* Context = InContext.FindContext<UContentBrowserAssetContextMenuContext>())
            {
                if (Context->CommonClass->IsChildOf(UDataAsset::StaticClass()))
                {
                    return true;
                }
            }
            return false;
        });

        Section.AddMenuEntry(
            "RefreshThumbnails",
            INVTEXT("Refresh Thumbnails"),
            INVTEXT("Refresh Thumbnails on these assets"),
            FSlateIcon(),
            Action
        );
    }

    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.FolderContextMenu");
        FToolMenuSection& Section = Menu->FindOrAddSection("PathContextBulkOperations");
        Section.AddMenuEntry(
            "RefreshThumbnailsPath",
            INVTEXT("Refresh Thumbnails"),
            INVTEXT("Refresh Thumbnails on assets in this directory"),
            FSlateIcon(),
            FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext& InContext)
            {
                if (const UContentBrowserFolderContext* Context = InContext.FindContext<UContentBrowserFolderContext>())
                {
                    RefreshThumbnailsInFolders(Context->GetSelectedPackagePaths());
                }
            })
        );
    }
}

void FDATEditorModule::RefreshThumbnails(const TArray<FAssetData>& SelectedAssets)
{
    TArray<UPackage*> PackagesToSave;

    for (const FAssetData& AssetData : SelectedAssets)
    {
        UObject* Asset = AssetData.GetAsset();
        if (Asset && IsValidChecked(Asset))
        {
            ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(Asset);
            PackagesToSave.Add(Asset->GetPackage());
        }
    }

    if (PackagesToSave.Num() > 0)
    {
        FEditorFileUtils::FPromptForCheckoutAndSaveParams SaveParams;
        SaveParams.bCheckDirty = false;
        SaveParams.bPromptToSave = false;
        SaveParams.bIsExplicitSave = true;

        FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, SaveParams);
    }
}

void FDATEditorModule::RefreshThumbnailsInFolders(const TArray<FString>& SelectedPaths)
{
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    FARFilter Filter;
    Filter.ClassPaths.Add(UDataAsset::StaticClass()->GetClassPathName());
    Filter.bRecursivePaths = true;
    Filter.bRecursiveClasses = true;
    for (const FString& Folder : SelectedPaths)
    {
        Filter.PackagePaths.Emplace(*Folder);
    }

    TArray<FAssetData> AssetList;
    AssetRegistryModule.Get().GetAssets(Filter, AssetList);

    RefreshThumbnails(AssetList);
}

UE_ENABLE_OPTIMIZATION