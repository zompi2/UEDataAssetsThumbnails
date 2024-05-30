// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#include "DATRenderer.h"
#include "Engine/Texture2D.h"
#include "UnrealClient.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "AssetViewUtils.h"

UE_DISABLE_OPTIMIZATION

bool UDATRenderer::CanVisualizeAsset(UObject* Object)
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

void UDATRenderer::GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const
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

void UDATRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily)
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

EThumbnailRenderFrequency UDATRenderer::GetThumbnailRenderFrequency(UObject* Object) const
{
	return EThumbnailRenderFrequency::OnPropertyChange;
}

UObject* UDATRenderer::GetObject(UObject* Object) const
{
	if (UDataAsset* DataAsset = Cast<UDataAsset>(Object))
	{
		for (TFieldIterator<FProperty> PropertyIt(DataAsset->GetClass()); PropertyIt; ++PropertyIt)
		{
			FProperty* Property = *PropertyIt;
			if (Property->HasMetaData("Thumbnail"))
			{
				if (Property->GetClass()->IsChildOf(FObjectProperty::StaticClass()))
				{
					FObjectProperty* ObjectProperty = static_cast<FObjectProperty*>(Property);
					if (ObjectProperty)
					{
						if (ObjectProperty->PropertyClass && ObjectProperty->PropertyClass->IsChildOf<UTexture2D>())
						{
							TObjectPtr<UObject> ThumbObject = FObjectProperty::GetPropertyValue(ObjectProperty->ContainerPtrToValuePtr<void>(DataAsset));
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

UThumbnailRenderer* UDATRenderer::GetRenderer(UObject* Object) const
{
	if (FThumbnailRenderingInfo* Info = UThumbnailManager::Get().GetRenderingInfo(Object))
	{
		return Info->Renderer;
	}
	return nullptr;
}

UE_ENABLE_OPTIMIZATION