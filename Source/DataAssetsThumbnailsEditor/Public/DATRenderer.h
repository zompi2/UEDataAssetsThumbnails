// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "ThumbnailRendering/TextureThumbnailRenderer.h"
#include "DATUtils.h"
#include "DATRenderer.generated.h"

namespace DAT
{
	const FName& ThumbnailTag = "Thumbnail";
	const FName& HasThumbnailTag = "HasThumbnail";
}

UCLASS()
class DATAASSETSTHUMBNAILSEDITOR_API UDATRenderer : public UThumbnailRenderer
{
	GENERATED_BODY()

public:

	bool CanVisualizeAsset(UObject* Object);
	void GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const override;
	void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, class FRenderTarget* Viewport, class FCanvas* Canvas, bool bAdditionalViewFamily) override;
	EThumbnailRenderFrequency GetThumbnailRenderFrequency(UObject* Object) const;

protected:
	
	template<typename T>
	T* FindSupportedThumbnailObjectProperty(UObject* Object) const
	{
		UDataAsset* DataAsset = Cast<UDataAsset>(Object);
		if (DataAsset == nullptr)
		{
			return nullptr;
		}

		for (TFieldIterator<FProperty> PropertyIt(DataAsset->GetClass()); PropertyIt; ++PropertyIt)
		{
			FProperty* Property = *PropertyIt;
			if (Property == nullptr) [[unlikely]]
			{
				continue;
			}

			if (Property->HasMetaData(DAT::ThumbnailTag) == false)
			{
				continue;
			}

			T* ObjectProperty = FDATUtils::CastPropertyToSupportedObjectProperty<T>(Property);
			if (ObjectProperty)
			{
				return ObjectProperty;
			}
		}

		return nullptr;
	}

	template<typename T, typename R>
	R GetSupportedObjectFromObjectProperty(T* ObjectProperty, UObject* ContainerObject) const
	{
		if (ObjectProperty == nullptr)
		{
			return {};
		}

		if (ContainerObject == nullptr)
		{
			return {};
		}

		return T::GetPropertyValue(ObjectProperty->ContainerPtrToValuePtr<void>(ContainerObject));
	}

	template<typename T, typename R>
	R GetSupportedObjectFromObject(UObject* Object) const
	{
		return GetSupportedObjectFromObjectProperty<T, R>(FindSupportedThumbnailObjectProperty<T>(Object), Object);
	}

private: 

	UThumbnailRenderer* GetThumbnailRenderer(UObject* Object) const;
	bool bThumbnailLock = false;

	bool GetHasThumbnailMetaData(UObject* Object) const;
	void SetHasThumbnailMetaData(UObject* Object, bool bHasThumbnail);
};
