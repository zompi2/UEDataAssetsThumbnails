// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"

template<typename T> struct TDATSupportedType { enum { Value = false }; };

template<> struct TDATSupportedType<class UTexture2D> { enum { Value = true }; };
template<> struct TDATSupportedType<class UMaterialInterface> { enum { Value = true }; };