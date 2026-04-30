// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#pragma once

/**
 * Template utilities used for obtaining correct objects and structs from assets properties.
 */

#include "CoreMinimal.h"
#include "DATTraits.h"
#include "DATTypes.h"
#include "IDataAssetWithThumbnail.h"
#include "UObject/UnrealType.h"
#include "Templates/AreTypesEqual.h"

UE_DISABLE_OPTIMIZATION

class DATAASSETSTHUMBNAILSEDITOR_API FDATEditorUtils
{

private:

	/**
	 * Casts the FProperty into one of the supported FProperty, that stores object that can render thumbnail.
	 * It can be FObjectProperty or FSoftObjectProperty, that stores UTexture2D or UMaterialInterface.
	 * It can also be FStructProperty, that stores FSlateBrush or FColor or FLinearColor.
	 * When casting to FStructProperty, the ExpectedStructName must be provided, to prevent invalid casting
	 * in next steps.
	 */
	template<typename T>
	static T* CastPropertyToSupportedObjectProperty(FProperty* Property, const FName& ExpectedStructName)
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
		if (ObjectStructProperty == nullptr)
		{
			return nullptr;
		}

		if constexpr (ARE_TYPES_EQUAL(T, FStructProperty))
		{
			if (ObjectStructProperty->Struct->GetName() == ExpectedStructName)
			{
				return ObjectStructProperty;
			}
		}	
		else
		{
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
	 * For FStructProperty it requires ExpectedStructName to be provided, to prevent invalid casting in next steps.
	 * Returns the pointer to the supported object property and sets bFound to true if found.
	 */
	template<typename T>
	static T* FindSupportedThumbnailObjectProperty(const UObject* Object, const FName& ExpectedStructName)
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

			T* ObjectStructProperty = FDATEditorUtils::CastPropertyToSupportedObjectProperty<T>(Property, ExpectedStructName);
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
	 *   FStructProperty - FColor
	 *   FStructProperty - FLinearColor
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
		FObjectProperty* ObjectProperty = CastPropertyToSupportedObjectProperty<FObjectProperty>(Property, "");
		if (ObjectProperty)
		{
			return true;
		}

		FSoftObjectProperty* SoftObjectProperty = CastPropertyToSupportedObjectProperty<FSoftObjectProperty>(Property, "");
		if (SoftObjectProperty)
		{
			return true;
		}

		FStructProperty* StructProperty = CastPropertyToSupportedObjectProperty<FStructProperty>(Property, TStructName<FSlateBrush>::Name);
		if (StructProperty)
		{
			return true;
		}

		StructProperty = CastPropertyToSupportedObjectProperty<FStructProperty>(Property, TStructName<FColor>::Name);
		if (StructProperty)
		{
			return true;
		}

		StructProperty = CastPropertyToSupportedObjectProperty<FStructProperty>(Property, TStructName<FLinearColor>::Name);
		if (StructProperty)
		{
			return true;
		}

		return false;
	}

	/**
	 * Finds the object that can be rendered using thumbnail renderer inside of the UObject.
	 * It searches for UObject*, FSoftObjectPtr or FSlateBrush or FLinearColor from the Object.
	 * The only supported pairs are:
	 *   FObjectProperty - UObject*
	 *   FSoftObjectProperty - FSoftObjectPtr
	 *   FStructProperty - FSlateBrush
	 */
	template<typename T, typename R>
	static R FindSupportedThumbnailObjectInObject(const UObject* Object)
	{
		static_assert(TSupportedObjectPair<T, R>::Value, "Unsupported Property Type");
		static_assert(TSupportedColorType<R>::Value == false, "Can't obtain colors like that. Use FindSupportedThumbnailColorInObject instead!");
		return GetSupportedObjectFromObjectProperty<T, R>(FindSupportedThumbnailObjectProperty<T>(Object, TStructName<R>::Name), Object);
	}

	/**
	 * Finds the color that can be rendered using thumbnail renderer inside of the UObject.
	 * It searches for FColor and FLinearColor from the Object.
	 */
	template<typename T>
	static T FindSupportedThumbnailColorInObject(const UObject* Object, bool& bFoundColor)
	{
		static_assert(TSupportedColorType<T>::Value, "Unsupported Color Type");
		FStructProperty* StructProperty = FindSupportedThumbnailObjectProperty<FStructProperty>(Object, TStructName<T>::Name);
		if (StructProperty)
		{
			bFoundColor = true;
			return GetSupportedObjectFromObjectProperty<FStructProperty, T>(StructProperty, Object);
		}
		bFoundColor = false;
		return T::Black;
	}
};

UE_ENABLE_OPTIMIZATION