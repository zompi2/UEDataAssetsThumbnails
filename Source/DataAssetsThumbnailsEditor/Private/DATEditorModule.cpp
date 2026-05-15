// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "DATEditorModule.h"

#include "DATEditorStyle.h"
#include "DATRenderer.h"
#include "DATCustomDetails.h"
#include "DATSettings.h"
#include "DATAssetDefinition.h"

#include "ThumbnailRendering/ThumbnailManager.h"
#include "BlueprintEditorModule.h"
#include "ContentBrowserMenuContexts.h"
#include "ObjectTools.h"
#include "FileHelpers.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ISettingsModule.h"
#include "AssetDefinitionRegistry.h"

IMPLEMENT_MODULE(FDATEditorModule, DataAssetsThumbnailsEditor);

void FDATEditorModule::StartupModule()
{
    // Register Styles.
    FDATEditorStyle::Initialize();
    FDATEditorStyle::ReloadTextures();

    // Register custom thumbnail renderers for all data assets.
    UThumbnailManager::Get().RegisterCustomRenderer(UDataAsset::StaticClass(), UDATRenderer::StaticClass());

    // Register custom details - the HasThumbnail option in details panel.
    FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
    OnGetVariableCustomizationInstanceDelegateHandle = BlueprintEditorModule.RegisterVariableCustomization(FProperty::StaticClass(), FOnGetVariableCustomizationInstance::CreateStatic(&FDATCustomDetails::MakeInstance));

    // Register Menus to add options for refreshing data asset thumbnails in content brower right click menus.
    UToolMenus::Get()->RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDATEditorModule::RegisterMenus));

    // Register plugin settings.
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
    if (SettingsModule)
    {
        SettingsModule->RegisterSettings("Project", "Plugins", "DAT",
            INVTEXT("Data Asset Thumbnails"),
            INVTEXT("Global settings for Data Asset Thumbnails"),
            GetMutableDefault<UDATSettings>()
        );
    }

    // Now when settings are registered, register AssetTypeActions (asset default colors and icons).
    RefreshAssetTypeActions();
}

void FDATEditorModule::ShutdownModule()
{
    // Unregister AssetActiontypes.
    if (UAssetDefinitionRegistry* Registry = UAssetDefinitionRegistry::Get())
    {
        for (auto RegisteredAssetType : CreatedAssetDefinitions)
        {
            Registry->UnregisterAssetDefinition(RegisteredAssetType.Value);
        }
    }
    CreatedAssetDefinitions.Empty();

    // Unregister plugin settings.
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Plugins", "DAT");
    }

    // Unregister Menus from content browser right click menu.
    UToolMenus::UnregisterOwner(this);

    // Unregister custom details.
    FBlueprintEditorModule* BlueprintEditorModule = FModuleManager::GetModulePtr<FBlueprintEditorModule>("Kismet");
    if (BlueprintEditorModule)
    {
        BlueprintEditorModule->UnregisterVariableCustomization(FProperty::StaticClass(), OnGetVariableCustomizationInstanceDelegateHandle);
    }

    // Unregister custom thumbnail renderer.
#if ENGINE_MINOR_VERSION < 8
    if (UThumbnailManager* ThumbnailManager = UThumbnailManager::TryGet())
    {
        if (IsValid(ThumbnailManager) && (ThumbnailManager->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed) == false))
        {
            ThumbnailManager->UnregisterCustomRenderer(UDataAsset::StaticClass());
        }
    }
#endif

    // Unregister Styles.
    FDATEditorStyle::Shutdown();
}

void FDATEditorModule::AddReferencedObjects(FReferenceCollector& Collector)
{
    Collector.AddReferencedObjects(CreatedAssetDefinitions);
}

FString FDATEditorModule::GetReferencerName() const
{
	return TEXT("FDATEditorModule");
}

/**
 * Refreshes AssetTypeActions (which controls icon color and default icon) on startup
 * and everytime when the plugins settings has changed.
 */
void FDATEditorModule::RefreshAssetTypeActions()
{
    // Obtain settings used to register AssetTypeActions.
    UDATSettings* DATSettings = UDATSettings::Get();
    if (DATSettings == nullptr)
    {
        return;
    }
   
    // Save the types of DataAssets that should have custom AssetTypeActions 
    // (used later to diff and unregister the old ones).
    TSet<TSubclassOf<UDataAsset>> DesiredClasses;

    // Go though all settings.
    for (auto& GlobalOverride : DATSettings->GlobalOverrides)
    {
        // Obtain key (data asset class) and value (settings) for cleaner code.
        TSubclassOf<UDataAsset> OverrideKey = GlobalOverride.Key;
        const FDATGlobalOverride& OverrideValue = GlobalOverride.Value;

        // If there is any kind of override...
        if (OverrideKey && OverrideValue.OverridesAnyAssetTypeActionData())
        {
            // This data asset class is overriden.
            DesiredClasses.Add(OverrideKey);

            // If the AssetDefinition already exists just change it's settings.
            if (CreatedAssetDefinitions.Contains(OverrideKey))
            {
                CreatedAssetDefinitions[OverrideKey]->DataAssetColor = OverrideValue.GetColor();
                CreatedAssetDefinitions[OverrideKey]->SlateBrush = OverrideValue.GetDefaultIconBrush();
                CreatedAssetDefinitions[OverrideKey]->bUseBrushAsCornerIcon = OverrideValue.bUseIconAsCornerIcon;
            }
            // Otherwise add new AssetDefinition and register it.
            else
            {
                UDATAssetDefinition* NewDefinition = NewObject<UDATAssetDefinition>();
                NewDefinition->DataAssetClass = OverrideKey;
                NewDefinition->DataAssetColor = OverrideValue.GetColor();
                NewDefinition->SlateBrush = OverrideValue.GetDefaultIconBrush();
                NewDefinition->bUseBrushAsCornerIcon = OverrideValue.bUseIconAsCornerIcon;
                CreatedAssetDefinitions.Add(GlobalOverride.Key, NewDefinition);
                if (UAssetDefinitionRegistry* Registry = UAssetDefinitionRegistry::Get())
                {
                    Registry->RegisterAssetDefinition(NewDefinition);
                }
            }
        }
    }

    // Diff DesiredClasses with CreatedAssetDefinitions to get data asset classess which should not
    // have overriden AssetTypeAction.
    TArray<TSubclassOf<UDataAsset>> ClassesToUnregister;
    for (auto& AssetActionType : CreatedAssetDefinitions)
    {
        if (DesiredClasses.Contains(AssetActionType.Key) == false)
        {
            ClassesToUnregister.Add(AssetActionType.Key);
        }
    }

    // Unregister unwanted AssetDefinitions.
    for (TSubclassOf<UDataAsset> ClassToUnregister : ClassesToUnregister)
    {
        if (UAssetDefinitionRegistry* Registry = UAssetDefinitionRegistry::Get())
        {
            Registry->UnregisterAssetDefinition(CreatedAssetDefinitions[ClassToUnregister]);
        }
        CreatedAssetDefinitions.Remove(ClassToUnregister);
    }
}

/**
 * Helper function that registers "refresh thumbnails" buttons in content browser right click menus.
 */
void FDATEditorModule::RegisterMenus()
{
    // In order to extend the content brower context menu we need to enter ToolMenuOwnerScope
    FToolMenuOwnerScoped OwnerScoped(this);

    // Add "Refresh Thumbnails" to the "Asset Actions" submenu for DataAssets in Content Browser.
    // Keep it in the separate scopes to not worry about variable names later.
    {
        // Obtain the "Advanced" section from the "Asset Actions" submenu.
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.AssetActionsSubMenu");
        FToolMenuSection& Section = Menu->FindOrAddSection("AssetContextAdvancedActions");

        // Prepare the UI Action.
        FToolUIAction Action;

        // Run RefreshThumbnails function when the option in menu is clicked.
        Action.ExecuteAction = FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext& InContext)
        {
            if (const UContentBrowserAssetContextMenuContext* Context = InContext.FindContext<UContentBrowserAssetContextMenuContext>())
            {
                RefreshThumbnails(Context->SelectedAssets);
            }
        });

        // Show it only for Data Assets.
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

        // Add the Action to the obtained section
        Section.AddMenuEntry(
            "RefreshThumbnails",
            INVTEXT("Refresh Thumbnails"),
            INVTEXT("Refresh Thumbnails on these assets"),
            FSlateIcon(FDATEditorStyle::GetStyleSetName(), "DATEditorStyle.MenuIcon"),
            Action
        );
    }

    // Add "Refresh Thumbnails" to the "Bulk Operations" section to the directory menu in Content Browser.
    {
        // Obtain the "Bulk Operations" section from the directory menu.
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.FolderContextMenu");
        FToolMenuSection& Section = Menu->FindOrAddSection("PathContextBulkOperations");

        // Prepare the UI Action.
        FToolUIAction Action;

        // Run RefreshThumbnailsInFolders function when the option in menu is clicked.
        Action.ExecuteAction = FToolMenuExecuteAction::CreateLambda([this](const FToolMenuContext& InContext)
        {
            if (const UContentBrowserFolderContext* Context = InContext.FindContext<UContentBrowserFolderContext>())
            {
                RefreshThumbnailsInFolders(Context->GetSelectedPackagePaths());
            }
        });

        // Add the Action to the obtained section
        Section.AddMenuEntry(
            "RefreshThumbnailsPath",
            INVTEXT("Refresh Thumbnails"),
            INVTEXT("Refresh Thumbnails on assets in this directory"),
            FSlateIcon(FDATEditorStyle::GetStyleSetName(), "DATEditorStyle.MenuIcon"),
            Action
        );
    }
}

/**
 * Utility function that refreshes thumbnails in given assets.
 */
void FDATEditorModule::RefreshThumbnails(const TArray<FAssetData>& SelectedAssets)
{
    // Used to store packages to be resaved.
    TArray<UPackage*> PackagesToSave;

    // Go through all selected assets.
    for (const FAssetData& AssetData : SelectedAssets)
    {
        UObject* Asset = AssetData.GetAsset();
        if (IsValid(Asset))
        {
            // GenerateThumbnailForObjectToSaveToDisk should refresh the thumbnail for this asset.
            ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(Asset);

            // Ensure it's dirty.
            Asset->MarkPackageDirty();

            // Remember this asset to save it later.
            PackagesToSave.Add(Asset->GetPackage());
        }
    }

    // Force save all the touched assets.
    if (PackagesToSave.Num() > 0)
    {
        FEditorFileUtils::FPromptForCheckoutAndSaveParams SaveParams;
        SaveParams.bCheckDirty = true;
        SaveParams.bPromptToSave = false;
        SaveParams.bIsExplicitSave = false;

        FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, SaveParams);
    }
}

/**
 * Utility function that refreshes thumbnails in the assets inside of the given directories.
 */
void FDATEditorModule::RefreshThumbnailsInFolders(const TArray<FString>& SelectedPaths)
{
    // Prepare a filter to search for all Data Assets.
    FARFilter Filter;
    Filter.ClassPaths.Add(UDataAsset::StaticClass()->GetClassPathName());
    Filter.bRecursivePaths = true;
    Filter.bRecursiveClasses = true;
    for (const FString& Folder : SelectedPaths)
    {
        Filter.PackagePaths.Emplace(*Folder);
    }

    // Get the list of assets from these directories.
    TArray<FAssetData> AssetList;
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    AssetRegistryModule.Get().GetAssets(Filter, AssetList);

    // Run RefreshThumbnails on all these obtained assets.
    RefreshThumbnails(AssetList);
}
