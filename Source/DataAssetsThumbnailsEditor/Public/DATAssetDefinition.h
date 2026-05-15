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

#if (ENGINE_MINOR_VERSION >= 6)
	void GetAssetStatusInfo(const TSharedPtr<IAssetStatusInfoProvider>& InAssetStatusInfoProvider, TArray<FAssetDisplayInfo>& OutStatusInfo) const override;
#else
	TSharedPtr<SWidget> GetThumbnailOverlay(const FAssetData& AssetData) const override;
#endif


	/** */
	TSoftClassPtr<class UDataAsset> DataAssetClass;
	FColor DataAssetColor;
	const FSlateBrush* SlateBrush = nullptr;
	bool bUseBrushAsCornerIcon = false;
};
