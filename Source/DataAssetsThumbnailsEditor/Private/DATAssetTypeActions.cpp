// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

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