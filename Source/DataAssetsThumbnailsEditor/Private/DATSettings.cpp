// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "DATSettings.h"
#include "DATEditorModule.h"

UDATSettings* UDATSettings::Get()
{
	return GetMutableDefault<UDATSettings>();
}

void UDATSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Refresh Asset Type Actions if any of the Global Overrides has changed.
	if (PropertyChangedEvent.MemberProperty && (PropertyChangedEvent.MemberProperty->GetName() == "GlobalOverrides"))
	{
		FDATEditorModule& DATEditorModule = FModuleManager::LoadModuleChecked<FDATEditorModule>("DataAssetsThumbnailsEditor");
		DATEditorModule.RefreshAssetTypeActions();
	}
}

bool UDATSettings::SolidBackgroundOnTranslucentIcons(TSubclassOf<UDataAsset> AssetClass) const
{
	if (bSolidBackgroundOnTranslucentIcons)
	{
		return true;
	}
	if (const FDATGlobalOverride* Overrides = GlobalOverrides.Find(AssetClass))
	{
		return Overrides->bSolidBackgroundOnTranslucentIcons;
	}
	return false;
}

FLinearColor UDATSettings::GetAssetSolidBGColor(TSubclassOf<UDataAsset> AssetClass) const
{
	if (const FDATGlobalOverride* Overrides = GlobalOverrides.Find(AssetClass))
	{
		if (Overrides->bSolidBackgroundOnTranslucentIcons)
		{
			return Overrides->BackgroundColor;
		}
	}
	return DefaultBackgroundColor;
}
