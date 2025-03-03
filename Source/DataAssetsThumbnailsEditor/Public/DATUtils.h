// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

/**
 * Template utilities used for obtaining correct objects and structs from assets properties.
 */

#include "CoreMinimal.h"
#include "DATTraits.h"
#include "DATTypes.h"

UE_DISABLE_OPTIMIZATION

class DATAASSETSTHUMBNAILSEDITOR_API FDATUtils
{

private:

	/**
	 * Casts the FProperty into one of the supported FProperty, that stores object that can render thumbnail.
	 * It can be FObjectProperty or FSoftObjectProperty, that stores UTexture2D or UMaterialInterface.
	 * It can also be FStructProperty, that stores FSlateBrush.
	 */
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

		T* ObjectStructProperty = static_cast<T*>(Property);

		if constexpr (TIsStructProperty<T>::Value)
		{
			if (ObjectStructProperty == nullptr)
			{
				return nullptr;
			}

			if (ObjectStructProperty->Struct->IsChildOf<FSlateBrush>())
			{
				return ObjectStructProperty;
			}
		}
		else
		{
			if (ObjectStructProperty->PropertyClass == nullptr) [[unlikely]]
			{
				return nullptr;
			}
	
			if (ObjectStructProperty->PropertyClass->IsChildOf<UTexture2D>())
			{
				return ObjectStructProperty;
			}
	
			if (ObjectStructProperty->PropertyClass->IsChildOf<UMaterialInterface>())
			{
				return ObjectStructProperty;
			}
		}

		return nullptr;
	}

	/**
	 * Find the supported FProperty (can be FObjectProperty, FSoftObjectProperty, or FStructProperty)
	 * inside of the given Object, which supposed to be a DataAsset.
	 * It searches for FProperties with Thumbnail Metadata Tag. 
	 */
	template<typename T>
	static T* FindSupportedThumbnailObjectProperty(const UObject* Object)
	{
		static_assert(TSupportedPropertyCast<T>::Value, "Unsupported Property Cast Type");

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

			T* ObjectStructProperty = FDATUtils::CastPropertyToSupportedObjectProperty<T>(Property);
			if (ObjectStructProperty)
			{
				return ObjectStructProperty;
			}
		}

		return nullptr;
	}

	/**
	 * Gets the object from FProperty. Only supported pairs of FProperty and returned Objects are supported.
	 * They can be:
	 *   FObjectProperty - UObject*
	 *   FSoftObjectProperty - FSoftObjectPtr
	 *   FStructProperty - FSlateBrush
	 */
	template<typename T, typename R>
	static R GetSupportedObjectFromObjectProperty(T* ObjectOrStructProperty, const UObject* ContainerObject)
	{
		static_assert(TSupportedObjectPair<T, R>::Value, "Unsupported Property Type");
		
		if (ObjectOrStructProperty == nullptr)
		{
			return {};
		}

		return TPropertyTypeFundamentals<R>::GetPropertyValue(ObjectOrStructProperty->ContainerPtrToValuePtr<void>(ContainerObject));
	}

public:

	/**
	 * Checks if the given FProperty is supported by DAT renderer.
	 * It runs CastPropertyToSupportedObjectProperty for every supported types and checks if any match.
	 */
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

		FStructProperty* StructProperty = CastPropertyToSupportedObjectProperty<FStructProperty>(Property);
		if (StructProperty)
		{
			return true;
		}

		return false;
	}

	/**
	 * Finds the object that can be rendered using thumbnail renderer inside of the UObject.
	 * It searches for UObject*, FSoftObjectPtr or FSlateBrush from the Object.
	 * The only supported pairs are:
	 *   FObjectProperty - UObject*
	 *   FSoftObjectProperty - FSoftObjectPtr
	 *   FStructProperty - FSlateBrush
	 */
	template<typename T, typename R>
	static R FindSupportedThumbnailObjectInObject(const UObject* Object)
	{
		static_assert(TSupportedObjectPair<T, R>::Value, "Unsupported Property Type");

		return GetSupportedObjectFromObjectProperty<T, R>(FindSupportedThumbnailObjectProperty<T>(Object), Object);
	}
};

UE_ENABLE_OPTIMIZATION