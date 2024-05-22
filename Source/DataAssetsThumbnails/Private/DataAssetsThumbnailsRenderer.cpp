// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAssetsThumbnailsRenderer.h"
#include "Engine/Texture2D.h"
#include "UnrealClient.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "AssetViewUtils.h"

UE_DISABLE_OPTIMIZATION

bool UDataAssetsThumbnailsRenderer::CanVisualizeAsset(UObject* Object)
{	
	if (bThumbnailLock)
	{
		return false;
	}
	bThumbnailLock = true;

	bool bResult = false;
	if (GetObject(Object) == nullptr)
	{
		bResult = false;
		if (AssetViewUtils::AssetHasCustomThumbnail(FAssetData(Object)))
		{
			AssetViewUtils::ClearCustomThumbnails({ FAssetData(Object) });
		}
	}
	else
	{
		bResult = true;
	}

	bThumbnailLock = false;
	
	return bResult;
}

void UDataAssetsThumbnailsRenderer::GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const
{
	if (UObject* ThumbnailObject = GetObject(Object))
	{
		if (UThumbnailRenderer* Renderer = GetRenderer(ThumbnailObject))
		{
			Renderer->GetThumbnailSize(ThumbnailObject, Zoom, OutWidth, OutHeight);
			return;
		}
	}

	OutWidth = 256;
	OutHeight = 256;
}

void UDataAssetsThumbnailsRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	if (UObject* ThumbnailObject = GetObject(Object))
	{
		if (ThumbnailObject->IsA<UDataAsset>())
		{
			
		}
		else
		{
			if (UThumbnailRenderer* Renderer = GetRenderer(ThumbnailObject))
			{
				Renderer->Draw(ThumbnailObject, X, Y, Width, Height, Viewport, Canvas, bAdditionalViewFamily);
			}
		}
	}
	else
	{
		//AssetViewUtils::ClearCustomThumbnails({FAssetData(Object)});
	}
}

EThumbnailRenderFrequency UDataAssetsThumbnailsRenderer::GetThumbnailRenderFrequency(UObject* Object) const
{
	return EThumbnailRenderFrequency::OnPropertyChange;
}

UObject* UDataAssetsThumbnailsRenderer::GetObject(UObject* Object) const
{
	if (UDataAsset* DataAsset = Cast<UDataAsset>(Object))
	{
		for (TFieldIterator<FProperty> Property(DataAsset->GetClass()); Property; ++Property)
		{
			if (Property->HasMetaData("Thumbnail"))
			{
				if (Property->GetClass()->IsChildOf(FObjectProperty::StaticClass()))
				{
					TObjectPtr<UObject> ThumbObject = FObjectProperty::GetPropertyValue(Property->ContainerPtrToValuePtr<void>(DataAsset));
					if (ThumbObject)
					{
						if (UPackage* Pck = Object->GetPackage())
						{
							if (UMetaData* Meta = Pck->GetMetaData())
							{
								Meta->SetValue(DataAsset, "HasThumbnail", TEXT("true"));
							}
						}
						return ThumbObject;
					}
				}
			}
		}
		if (UPackage* Pck = Object->GetPackage())
		{
			if (UMetaData* Meta = Pck->GetMetaData())
			{
				Meta->SetValue(DataAsset, "HasThumbnail", TEXT("false"));
			}
		}
	}
	return nullptr;
}

UThumbnailRenderer* UDataAssetsThumbnailsRenderer::GetRenderer(UObject* Object) const
{
	if (FThumbnailRenderingInfo* Info = UThumbnailManager::Get().GetRenderingInfo(Object))
	{
		return Info->Renderer;
	}
	return nullptr;
}

UE_ENABLE_OPTIMIZATION