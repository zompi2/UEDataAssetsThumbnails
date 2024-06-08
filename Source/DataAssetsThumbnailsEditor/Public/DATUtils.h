// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DATTraits.h"
#include "DATTypes.h"

class DATAASSETSTHUMBNAILSEDITOR_API FDATUtils
{

public:

	template<typename T>
	static T* CastPropertyToSupportedObjectProperty(FProperty* Property)
	{
		static_assert(TSupportedPropertyCast<T>::Value, "Unsupported Property Cast Type");

		if (Property == nullptr)
		{
			return nullptr;
		}

		if (Property->GetClass() == nullptr) [[unlikely]]
		{
			return nullptr;
		}

		if (Property->GetClass()->IsChildOf(T::StaticClass()) == false)
		{
			return nullptr;
		}

		T* ObjectProperty = static_cast<T*>(Property);
		if (ObjectProperty == nullptr)
		{
			return nullptr;
		}

		if (ObjectProperty->PropertyClass == nullptr) [[unlikely]]
		{
			return nullptr;
		}
	
		if (ObjectProperty->PropertyClass->IsChildOf<UTexture2D>())
		{
			return ObjectProperty;	
		}
	
		if (ObjectProperty->PropertyClass->IsChildOf<UMaterialInterface>())
		{
			return ObjectProperty;
		}

		return nullptr;
	}

	static bool IsPropertySupported(FProperty* Property)
	{
		FObjectProperty* ObjectProperty = CastPropertyToSupportedObjectProperty<FObjectProperty>(Property);
		if (ObjectProperty)
		{
			return true;
		}

		FSoftObjectProperty* SoftObjectProperty = CastPropertyToSupportedObjectProperty<FSoftObjectProperty>(Property);
		if (SoftObjectProperty)
		{
			return true;
		}

		return false;
	}

	template<typename T>
	static T* FindSupportedThumbnailObjectProperty(const UObject* Object)
	{
		const UDataAsset* DataAsset = Cast<UDataAsset>(Object);
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
	static R GetSupportedObjectFromObjectProperty(T* ObjectProperty, const UObject* ContainerObject)
	{
		static_assert(TSupportedObjectPair<T, R>::Value, "Unsupported Property Type");
		
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
	static R GetSupportedObjectFromObject(const UObject* Object)
	{
		return GetSupportedObjectFromObjectProperty<T, R>(FindSupportedThumbnailObjectProperty<T>(Object), Object);
	}
};
