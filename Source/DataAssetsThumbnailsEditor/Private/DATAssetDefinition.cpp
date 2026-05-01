// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "DATAssetDefinition.h"

FText UDATAssetDefinition::GetAssetDisplayName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_DataAsset", "Data Asset");
}

FLinearColor UDATAssetDefinition::GetAssetColor() const
{
	return DataAssetColor;
}

TSoftClassPtr<UObject> UDATAssetDefinition::GetAssetClass() const
{
	return DataAssetClass;
}

const FSlateBrush* UDATAssetDefinition::GetThumbnailBrush(const FAssetData& InAssetData, const FName InClassName) const
{
	return SlateBrush;
}

#if (ENGINE_MINOR_VERSION >= 6)
bool UDATAssetDefinition::GetThumbnailActionOverlay(const FAssetData& InAssetData, FAssetActionThumbnailOverlayInfo& OutActionOverlayInfo) const
{
	if ((bUseBrushAsCornerIcon == false) || (SlateBrush == nullptr))
	{
		OutActionOverlayInfo.ActionImageWidget = nullptr;
		return false;
	}

	OutActionOverlayInfo.ActionImageWidget = SNew(SImage).Image(SlateBrush);

	auto OnClickedLambda = [this, InAssetData]() -> FReply
	{
		FAssetOpenArgs OpenArgs;
		OpenArgs.OpenMethod = EAssetOpenMethod::Edit;
		OpenArgs.Assets = { InAssetData };
		OpenAssets(OpenArgs);
		return FReply::Handled();
	};

	OutActionOverlayInfo.ActionButtonArgs = SButton::FArguments()
		.OnClicked_Lambda(OnClickedLambda);

	return true;
}
#else
TSharedPtr<SWidget> UDATAssetDefinition::GetThumbnailOverlay(const FAssetData& AssetData) const
{
	if ((bUseBrushAsCornerIcon == false) || (SlateBrush == nullptr))
	{
		return nullptr;
	}

	return SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
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
#endif

