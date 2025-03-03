// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "DATSettings.h"
#include "DATEditorModule.h"

UE_DISABLE_OPTIMIZATION

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

UE_ENABLE_OPTIMIZATION