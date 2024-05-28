// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DATPrimaryDataAsset.generated.h"

UCLASS(abstract, Blueprintable)
class DATAASSETSTHUMBNAILS_API UDATPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Thumbnail))
	TObjectPtr<class UTexture2D> ThumbnailIcon = nullptr;
#endif
};

