// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

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
/*
    // Return a simple Slate image tinted to your color
    return SNew(SImage)
        // You can use a built-in white brush and tint it
        .Image(FAppStyle::GetBrush("WhiteBrush"))
        .ColorAndOpacity(FSlateColor(DataAssetColor));
*/

    return SNew(SOverlay)
        + SOverlay::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Bottom)
        .Padding(FMargin(4.0f)) // Adds a small gap from the thumbnail edges
        [
            // SBox forces the SImage to stay exactly this size
            SNew(SBox)
                .WidthOverride(12.0f)
                .HeightOverride(12.0f)
                [
                    SNew(SImage)
                        // "WhiteBrush" is a built-in UE5 untextured white square, perfect for tinting
                        .Image(FAppStyle::GetBrush("WhiteBrush"))
                        .ColorAndOpacity(FSlateColor(DataAssetColor))
                ]
        ];
}