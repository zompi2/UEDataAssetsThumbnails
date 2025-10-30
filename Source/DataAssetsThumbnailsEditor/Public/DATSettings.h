// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DATTypes.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "DATSettings.generated.h"

/**
 * Storing settings in DefaultEditor.ini file.
 */

USTRUCT(BlueprintType)
struct FDATGlobalOverride
{
    GENERATED_BODY()

    // Overriding the Data Asset color
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Data Assets Thumbnails")
    bool bOverrideColor = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bOverrideColor"), Category = "Data Assets Thumbnails")
    FColor Color = DAT::DefaultDataAssetColor;

    // Overriding the data asset default icon using Slate Brush
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Data Assets Thumbnails")
    bool bOverrideDefaultIcon = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bOverrideDefaultIcon"), Category = "Data Assets Thumbnails")
    FSlateBrush DefaultIconBrush;

    // Overriding if the data asset should draw solid background if with transulency (instead of standard checkerboard)
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Data Assets Thumbnails")
    bool bSolidBackgroundOnTranslucentIcons = false;

    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Data Assets Thumbnails", meta = (EditCondition = "bUseSolidColorOnTranslucentIcons && !bUseAssetColorAsBackgroundColor"))
    FLinearColor BackgroundColor = DAT::DefaultBGColor;

    // Is there any type of override that changes Asset Type Action Data
    bool OverridesAnyAssetTypeActionData() const
    {
        return bOverrideColor || bOverrideDefaultIcon;
    }

    // Returns the Data Asset color for this asset
    FColor GetColor() const
    {
        return bOverrideColor ? Color : DAT::DefaultDataAssetColor;
    }

    // Returns the Data Asset color for this asset
    const FSlateBrush* GetDefaultIconBrush() const
    {
        return bOverrideDefaultIcon ? &DefaultIconBrush : nullptr;
    }
};

UCLASS(config = Editor, defaultconfig)
class DATAASSETSTHUMBNAILSEDITOR_API UDATSettings : public UObject
{
    GENERATED_BODY()

public:

    /**
     * Convenient static getter of the settings.
     */
    static UDATSettings* Get();

    /**
     * List of global overrides (color and default icon for assets)
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Data Assets Thumbnails")
    TMap<TSubclassOf<class UDataAsset>, FDATGlobalOverride> GlobalOverrides;

    /**
     * Tells if the background under the icons with translucency should be solid or a standard checkerboard.
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Data Assets Thumbnails")
    bool bSolidBackgroundOnTranslucentIcons = false;

    /**
     * Color of the solid background under the transparent icons.
     */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Data Assets Thumbnails", meta = (EditCondition = "bUseSolidColorOnTranslucentIcons"))
    FLinearColor DefaultBackgroundColor = DAT::DefaultBGColor;

    /**
     * Checks if this asset type should render solid background color if icon has translucency.
     */
    bool SolidBackgroundOnTranslucentIcons(TSubclassOf<class UDataAsset> AssetClass) const;

    /**
     * Checks this asset background color to be rendered if icon is transparent.
     */
    FLinearColor GetAssetBGColor(TSubclassOf<class UDataAsset> AssetClass) const;

protected:

    /**
     * Called when any of the settings has changed. Used to refresh assets when this happens.
     */
    void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};
