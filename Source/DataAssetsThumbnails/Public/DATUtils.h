// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/AreTypesEqual.h"
#include "Templates/UnrealTypeTraits.h"

class DATAASSETSTHUMBNAILS_API FDATUtils
{
public:

	template<typename T>
	static UObject* GetRenderObjectFromObject(T InObject)
	{
		static constexpr bool IsTexture2D = TIsDerivedFrom<typename TRemovePointer<T>::Type, UTexture2D>::IsDerived;
		static constexpr bool IsMaterialInterface = TIsDerivedFrom<typename TRemovePointer<T>::Type, UMaterialInterface>::IsDerived;
		static constexpr bool IsSlateBrush = ARE_TYPES_EQUAL(T, FSlateBrush);

		static_assert(IsTexture2D || IsMaterialInterface || IsSlateBrush, "[FDATUtils::GetRenderObject] Invalid object type. Can be either UTexture2D, UMaterialInterface or FSlateBrush!");
	
		if constexpr (IsSlateBrush)
		{
			return InObject.GetResourceObject();
		}
		else if constexpr (IsTexture2D || IsMaterialInterface)
		{
			return InObject;
		}
		else
		{
			return nullptr;
		}
	}
};

