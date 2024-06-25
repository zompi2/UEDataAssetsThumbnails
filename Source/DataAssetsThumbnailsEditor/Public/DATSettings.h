// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DATSettings.generated.h"

/**
 * Storing settings in DefaultEditor.ini file.
 */

USTRUCT(BlueprintType)
struct FDATGlobalOverride
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle))
    bool bOverrideColor = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bOverrideColor"))
    FColor Color = FColor(201, 29, 85, 255);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle))
    bool bOverrideDefaultIcon = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bOverrideDefaultIcon"))
    class UTexture2D* DefaultIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FSlateBrush SlateBrush;
};

UCLASS(config = Editor, defaultconfig)
class DATAASSETSTHUMBNAILSEDITOR_API UDATSettings : public UObject
{
    GENERATED_BODY()

public:

    static UDATSettings* Get();

    UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
    TMap<TSubclassOf<class UDataAsset>, FDATGlobalOverride> GlobalOverrides;

    class UTexture2D* GetDefaultIconForObject(const UObject* Object) const;

    void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:


};
