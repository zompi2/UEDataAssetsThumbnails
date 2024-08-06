// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

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

    // Overriding the data asset color
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle))
    bool bOverrideColor = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bOverrideColor"))
    FColor Color = DAT::DefaultDataAssetColor;

    // Overriding the data asset default icon using Slate Brush
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle))
    bool bOverrideDefaultIcon = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bOverrideDefaultIcon"))
    FSlateBrush DefaultIconBrush;

    // Setup if displaying small icon in the corner
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle))
    bool bOverrideDisplaySmallIcon = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bOverrideDisplaySmallIcon"))
    UTexture2D* SmallIcon = nullptr;

    // Is there any type of override
    bool OverridesAnything() const
    {
        return bOverrideColor || bOverrideDefaultIcon || bOverrideDisplaySmallIcon;
    }

    FColor GetColor() const
    {
        return bOverrideColor ? Color : DAT::DefaultDataAssetColor;
    }

    const FSlateBrush* GetDefaultIconBrush() const
    {
        return bOverrideDefaultIcon ? &DefaultIconBrush : nullptr;
    }

    UTexture2D* GetSmallIcon() const
    {
        return bOverrideDisplaySmallIcon ? SmallIcon : nullptr;
    }
};

UCLASS(config = Editor, defaultconfig)
class DATAASSETSTHUMBNAILSEDITOR_API UDATSettings : public UObject
{
    GENERATED_BODY()

public:

    static UDATSettings* Get();

    UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
    TMap<TSubclassOf<class UDataAsset>, FDATGlobalOverride> GlobalOverrides;

    class UTexture2D* GetSmallIconForObject(const UObject* Object) const;

    void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:

};
