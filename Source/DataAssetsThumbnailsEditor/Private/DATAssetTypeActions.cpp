// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "DATAssetTypeActions.h"

FText FDATAssetTypeActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_DataAsset", "Data Asset");
}

uint32 FDATAssetTypeActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

FColor FDATAssetTypeActions::GetTypeColor() const
{
	return DataAssetColor;
}

UClass* FDATAssetTypeActions::GetSupportedClass() const
{
	return DataAssetClass;
}

const FSlateBrush* FDATAssetTypeActions::GetThumbnailBrush(const FAssetData& InAssetData, const FName InClassName) const
{
	return SlateBrush;
}

TSharedPtr<SWidget> FDATAssetTypeActions::GetThumbnailOverlay(const FAssetData& AssetData) const
{
    if ((bUseBrushAsCornerIcon == false) || (SlateBrush == nullptr))
    {
        return nullptr;
    }

    return SNew(SOverlay)
        +SOverlay::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Bottom)
        .Padding(FMargin(4.0f))
        [
            SNew(SBox)
                .WidthOverride(28.f)
                .HeightOverride(28.f)
                [
                    SNew(SImage).Image(SlateBrush)
                ]
        ];
}