// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"

template<typename T> struct TSupportedPropertyCast { enum { Value = false }; };
template<> struct TSupportedPropertyCast<FObjectProperty> { enum { Value = true }; };
template<> struct TSupportedPropertyCast<FSoftObjectProperty> { enum { Value = true }; };

template<typename T, typename R> struct TSupportedObjectPair { enum { Value = false }; };
template<> struct TSupportedObjectPair<FObjectProperty, UObject*> { enum { Value = true }; };
template<> struct TSupportedObjectPair<FSoftObjectProperty, FSoftObjectPtr> { enum { Value = true }; };