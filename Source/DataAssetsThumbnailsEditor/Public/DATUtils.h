// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"

template<typename T> struct TSupportedPropertyCast { enum { Value = false }; };
template<> struct TSupportedPropertyCast<FObjectProperty> { enum { Value = true }; };
template<> struct TSupportedPropertyCast<FSoftObjectProperty> { enum { Value = true }; };

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

};
