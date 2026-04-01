// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#pragma once

/**
 * Traits used for compile time verifying if property getters makes sense...
 */

#include "CoreMinimal.h"

/**
 * Checks if the CastPropertyToSupportedObjectProperty can perform on the given type.
 */
template<typename T> struct TSupportedPropertyCast { enum { Value = false }; };
template<> struct TSupportedPropertyCast<FObjectProperty> { enum { Value = true }; };
template<> struct TSupportedPropertyCast<FSoftObjectProperty> { enum { Value = true }; };
template<> struct TSupportedPropertyCast<FStructProperty> { enum { Value = true }; };

/**
 * Checks if the FProperty and Object type are correctly paired.
 */
template<typename T, typename R> struct TSupportedObjectPair { enum { Value = false }; };
template<> struct TSupportedObjectPair<FObjectProperty, UObject*> { enum { Value = true }; };
template<> struct TSupportedObjectPair<FSoftObjectProperty, FSoftObjectPtr> { enum { Value = true }; };
template<> struct TSupportedObjectPair<FStructProperty, FSlateBrush> { enum { Value = true }; };
template<> struct TSupportedObjectPair<FStructProperty, FLinearColor> { enum { Value = true }; };

/**
 * Gets the name of the struct type for the given struct property.
 */
template<typename R> struct TStructName { static constexpr const TCHAR* Name = TEXT(""); };
template<> struct TStructName<FSlateBrush> { static constexpr const TCHAR* Name = TEXT("SlateBrush"); };
template<> struct TStructName<FLinearColor> { static constexpr const TCHAR* Name = TEXT("LinearColor"); };