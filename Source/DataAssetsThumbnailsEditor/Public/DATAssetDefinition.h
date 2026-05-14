// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "AssetDefinitionDefault.h"
#include "Styling/SlateBrush.h"
#include "DATAssetDefinition.generated.h"

UCLASS()
class DATAASSETSTHUMBNAILSEDITOR_API UDATAssetDefinition : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:

	FText GetAssetDisplayName() const override;
	FLinearColor GetAssetColor() const override;
	TSoftClassPtr<UObject> GetAssetClass() const override;
	const FSlateBrush* GetThumbnailBrush(const FAssetData& InAssetData, const FName InClassName) const override;

#if (ENGINE_MINOR_VERSION >= 7)
	void GetAssetStatusInfo(const TSharedPtr<IAssetStatusInfoProvider>& InAssetStatusInfoProvider, TArray<FAssetDisplayInfo>& OutStatusInfo) const override;
#elif (ENGINE_MINOR_VERSION <= 5)
	TSharedPtr<SWidget> GetThumbnailOverlay(const FAssetData& AssetData) const override;
#endif


	/** */
	TSoftClassPtr<class UDataAsset> DataAssetClass;
	FColor DataAssetColor;
	const FSlateBrush* SlateBrush = nullptr;

#if (ENGINE_MINOR_VERSION != 6)
	bool bUseBrushAsCornerIcon = false;
#endif
};
