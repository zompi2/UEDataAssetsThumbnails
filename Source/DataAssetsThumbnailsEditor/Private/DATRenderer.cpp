// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#include "DATRenderer.h"
#include "Engine/Texture2D.h"
#include "UnrealClient.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "AssetViewUtils.h"
#include "DATUtils.h"
#include "DATSettings.h"

UE_DISABLE_OPTIMIZATION

bool UDATRenderer::CanVisualizeAsset(UObject* Object)
{	
	if (bThumbnailLock)
	{
		return false;
	}
	bThumbnailLock = true;

	bool bResult = false;

	const bool bHasThumbnailMetaData = GetHasThumbnailMetaData(Object);
	
	UObject* AssetToVisualize = GetAssetToVisualizeFromObject(Object);
	if (bHasThumbnailMetaData && (AssetToVisualize == nullptr))
	{
		AssetViewUtils::ClearCustomThumbnails({ FAssetData(Object) });
		SetHasThumbnailMetaData(Object, false);
	}
	else if ((bHasThumbnailMetaData == false) && (AssetToVisualize != nullptr))
	{
		SetHasThumbnailMetaData(Object, true);
	}

	bResult = (AssetToVisualize != nullptr);

	bThumbnailLock = false;
	
	return bResult;
}

UObject* UDATRenderer::GetAssetToVisualizeFromObject(const UObject* Object) const
{
	UObject* AssetToVisualize = FDATUtils::GetSupportedObjectFromObject<FObjectProperty, UObject*>(Object);
	if (AssetToVisualize == nullptr)
	{
		FSoftObjectPtr AssetToVisualizeSoft = FDATUtils::GetSupportedObjectFromObject<FSoftObjectProperty, FSoftObjectPtr>(Object);
		AssetToVisualize = AssetToVisualizeSoft.LoadSynchronous();
	}
	if (AssetToVisualize == nullptr)
	{
		UDATSettings* DATSettings = UDATSettings::Get();
		if (DATSettings)
		{
			AssetToVisualize = DATSettings->GetDefaultIconForObject(Object);
		}
	}

	return AssetToVisualize;
}

void UDATRenderer::GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const
{
	OutWidth = 256;
	OutHeight = 256;

	UObject* AssetToVisualize = GetAssetToVisualizeFromObject(Object);
	if (AssetToVisualize == nullptr)
	{
		return;
	}

	UThumbnailRenderer* Renderer = GetThumbnailRenderer(AssetToVisualize);
	if (Renderer == nullptr)
	{
		return;
	}

	Renderer->GetThumbnailSize(AssetToVisualize, Zoom, OutWidth, OutHeight);
}

void UDATRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	UObject* AssetToVisualize = GetAssetToVisualizeFromObject(Object);
	if (AssetToVisualize == nullptr)
	{
		return;
	}

	UThumbnailRenderer* Renderer = GetThumbnailRenderer(AssetToVisualize);
	if (Renderer == nullptr)
	{
		return;
	}

	Renderer->Draw(AssetToVisualize, X, Y, Width, Height, Viewport, Canvas, bAdditionalViewFamily);
}

EThumbnailRenderFrequency UDATRenderer::GetThumbnailRenderFrequency(UObject* Object) const
{
	return EThumbnailRenderFrequency::OnPropertyChange;
}

bool UDATRenderer::GetHasThumbnailMetaData(UObject* Object) const
{
	if (Object == nullptr)
	{
		return false;
	}

	UPackage* Package = Object->GetPackage();
	if (Package == nullptr)
	{
		return false;
	}

	UMetaData* MetaData = Package->GetMetaData();
	if (MetaData == nullptr)
	{
		return false;
	}

	FString* HasThumbnailMetaValue = MetaData->RootMetaDataMap.Find(DAT::HasThumbnailTag);
	if (HasThumbnailMetaValue)
	{
		if (*HasThumbnailMetaValue == TEXT("true"))
		{
			return true;
		}
	}

	return false;
}

void UDATRenderer::SetHasThumbnailMetaData(UObject* Object, bool bHasThumbnail)
{
	if (Object == nullptr)
	{
		return;
	}

	UPackage* Package = Object->GetPackage();
	if (Package == nullptr)
	{
		return;
	}

	UMetaData* MetaData = Package->GetMetaData();
	if (MetaData == nullptr)
	{
		return;
	}

	if (bHasThumbnail)
	{
		MetaData->RootMetaDataMap.Add(DAT::HasThumbnailTag, TEXT("true"));
	}
	else
	{
		MetaData->RootMetaDataMap.Add(DAT::HasThumbnailTag, TEXT("false"));
	}

	Object->Modify();
}

UThumbnailRenderer* UDATRenderer::GetThumbnailRenderer(UObject* Object) const
{
	if (FThumbnailRenderingInfo* Info = UThumbnailManager::Get().GetRenderingInfo(Object))
	{
		return Info->Renderer;
	}
	return nullptr;
}

UE_ENABLE_OPTIMIZATION