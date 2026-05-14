// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "DATAssetDefinition.h"

#if (ENGINE_MINOR_VERSION >= 7)
#include "AssetDefinitionAssetInfo.h"
#endif

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

#if (ENGINE_MINOR_VERSION >= 7)
void UDATAssetDefinition::GetAssetStatusInfo(const TSharedPtr<IAssetStatusInfoProvider>& InAssetStatusInfoProvider, TArray<FAssetDisplayInfo>& OutStatusInfo) const
{
	Super::GetAssetStatusInfo(InAssetStatusInfoProvider, OutStatusInfo);

	if ((bUseBrushAsCornerIcon) && (SlateBrush))
	{
		FAssetDisplayInfo NewStatus;
		NewStatus.StatusIcon = SlateBrush;
		NewStatus.StatusDescription = FText::FromString(DataAssetClass.GetAssetName());
		NewStatus.IsVisible = EVisibility::HitTestInvisible;
		OutStatusInfo.Add(MoveTemp(NewStatus));
	}
}
#elif (ENGINE_MINOR_VERSION <= 5)
TSharedPtr<SWidget> UDATAssetDefinition::GetThumbnailOverlay(const FAssetData& AssetData) const
{
	if ((bUseBrushAsCornerIcon == false) || (SlateBrush == nullptr))
	{
		return nullptr;
	}

	return SNew(SOverlay)
		+ SOverlay::Slot()
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
#endif

