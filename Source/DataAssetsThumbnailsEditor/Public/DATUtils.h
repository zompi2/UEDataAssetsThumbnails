// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DATTraits.h"
#include "DATTypes.h"

UE_DISABLE_OPTIMIZATION

class DATAASSETSTHUMBNAILSEDITOR_API FDATUtils
{

public:

	static FStructProperty* CastPropertyToSupportedStructProperty(FProperty* Property)
	{
		if (Property == nullptr)
		{
			return nullptr;
		}

		if (Property->GetClass() == nullptr) [[unlikely]]
		{
			return nullptr;
		}

		if (Property->GetClass()->IsChildOf(FStructProperty::StaticClass()) == false)
		{
			return nullptr;
		}

		FStructProperty* StructProperty = static_cast<FStructProperty*>(Property);
		if (StructProperty == nullptr)
		{
			return nullptr;
		}

		if (StructProperty->Struct->IsChildOf<FSlateBrush>())
		{
			return StructProperty;
		}

		return nullptr;
	}

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

			T* ObjectStructProperty = FDATUtils::CastPropertyToSupportedObjectProperty<T>(Property);
			if (ObjectStructProperty)
			{
				return ObjectStructProperty;
			}
		}

		return nullptr;
	}

	template<typename T, typename R>
	static R GetSupportedObjectFromObjectProperty(T* ObjectStructProperty, const UObject* ContainerObject)
	{
		static_assert(TSupportedObjectPair<T, R>::Value, "Unsupported Property Type");
		
		if (ObjectStructProperty == nullptr)
		{
			return {};
		}

		if (ObjectStructProperty == nullptr)
		{
			return {};
		}

		return TPropertyTypeFundamentals<R>::GetPropertyValue(ObjectStructProperty->ContainerPtrToValuePtr<void>(ContainerObject));
	}

	template<typename T, typename R>
	static R GetSupportedObjectFromObject(const UObject* Object)
	{
		return GetSupportedObjectFromObjectProperty<T, R>(FindSupportedThumbnailObjectProperty<T>(Object), Object);
	}
};

UE_ENABLE_OPTIMIZATION