// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#include "DATRenderer.h"
#include "Engine/Texture2D.h"
#include "UnrealClient.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "AssetViewUtils.h"
#include "DATEditorUtils.h"
#include "DATSettings.h"
#include "IDataAssetWithThumbnail.h"

UE_DISABLE_OPTIMIZATION

bool UDATRenderer::CanVisualizeAsset(UObject* Object)
{	
	// Prevent the potential inifinite loop
	if (bThumbnailLock)
	{
		return false;
	}
	bThumbnailLock = true;

	const bool bHasThumbnailMetaData = GetHasThumbnailMetaData(Object);
	UObject* AssetToVisualize = GetAssetToVisualizeFromObject(Object);

	// If this asset has "HasThumbnail" tag, but no correct asset to visualize is available,
	// clear the current custom thumbnail and remove the tag.
	if (bHasThumbnailMetaData && (AssetToVisualize == nullptr))
	{
		AssetViewUtils::ClearCustomThumbnails({ FAssetData(Object) });
		SetHasThumbnailMetaData(Object, false);
	}
	// If there is an asset to visualize available, but no "HasThumbnail" tag - add this tag.
	// Thumbnail will be set in the next phase.
	// The "HasThumbnail" tag prevents from Capturing Custom Thumbnail if a custom one from plugin is set.
	else if ((bHasThumbnailMetaData == false) && (AssetToVisualize != nullptr))
	{
		SetHasThumbnailMetaData(Object, true);
	}

	// Store the result now, return true if there is an Asset to Visualize.
	const bool bResult = (AssetToVisualize != nullptr);

	// Release the lock
	bThumbnailLock = false;
	
	// Return the result
	return bResult;
}

UObject* UDATRenderer::GetAssetToVisualizeFromObject(const UObject* Object) const
{
	UObject* AssetToVisualize = nullptr;

	// Try to get a UObject from the interface.
	if (Object->Implements<UDataAssetWithThumbnail>())
	{
		AssetToVisualize = IDataAssetWithThumbnail::Execute_GetAssetToRender(Object);
	}

	// If failed, try to get a UObject directly from the asset.
	if (AssetToVisualize == nullptr)
	{
		AssetToVisualize = FDATEditorUtils::FindSupportedThumbnailObjectInObject<FObjectProperty, UObject*>(Object);
	}
	
	if (AssetToVisualize == nullptr)
	{
		// If failed, try to get a Soft Object directly from the asset.
		FSoftObjectPtr AssetToVisualizeSoft = FDATEditorUtils::FindSupportedThumbnailObjectInObject<FSoftObjectProperty, FSoftObjectPtr>(Object);
		AssetToVisualize = AssetToVisualizeSoft.LoadSynchronous();
	}

	if (AssetToVisualize == nullptr)
	{
		// If failed, try to get a Slate Brush directly from the asset.
		FSlateBrush SlateBrush = FDATEditorUtils::FindSupportedThumbnailObjectInObject<FStructProperty, FSlateBrush>(Object);
		AssetToVisualize = SlateBrush.GetResourceObject();
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
	// Obtain the actual asset (Texture2D, MaterialInterface, SlateBrush resource object) used to draw the thumbnail.
	UObject* AssetToVisualize = GetAssetToVisualizeFromObject(Object);
	if (AssetToVisualize == nullptr)
	{
		return;
	}

	// Get the actual renderer for this object
	UThumbnailRenderer* Renderer = GetThumbnailRenderer(AssetToVisualize);
	if (Renderer == nullptr)
	{
		return;
	}

	// Draw this object
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
	Object->MarkPackageDirty();
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