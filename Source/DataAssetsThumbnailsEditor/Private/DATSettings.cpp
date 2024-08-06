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

	if (PropertyChangedEvent.MemberProperty && (PropertyChangedEvent.MemberProperty->GetName() == "GlobalOverrides"))
	{
		FDATEditorModule& DATEditorModule = FModuleManager::LoadModuleChecked<FDATEditorModule>("DataAssetsThumbnailsEditor");
		DATEditorModule.RefreshAssetTypeActions();
	}
}

UTexture2D* UDATSettings::GetSmallIconForObject(const UObject* Object) const
{
	if (Object == nullptr)
	{
		return nullptr;
	}

	if (Object->HasAnyFlags(RF_ArchetypeObject))
	{
		return nullptr;
	}

	const FDATGlobalOverride* Overrides = GlobalOverrides.Find(Object->GetClass());
	if (Overrides == nullptr)
	{
		return nullptr;
	}

	return Overrides->GetSmallIcon();
}

UE_ENABLE_OPTIMIZATION