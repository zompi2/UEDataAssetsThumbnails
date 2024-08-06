// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "AssetTypeActions_Base.h"

class DATAASSETSTHUMBNAILSEDITOR_API FDATAssetTypeActions : public FAssetTypeActions_Base
{
public:

	FText GetName() const override;
	uint32 GetCategories() override;
	FColor GetTypeColor() const override;
	UClass* GetSupportedClass() const override;
	const FSlateBrush* GetThumbnailBrush(const FAssetData& InAssetData, const FName InClassName) const override;

	TSubclassOf<class UDataAsset> DataAssetClass;
	FColor DataAssetColor;
	const FSlateBrush* SlateBrush = nullptr;
};
