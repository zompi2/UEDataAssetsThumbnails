// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DATDataAsset.generated.h"

UCLASS(abstract, Blueprintable)
class DATAASSETSTHUMBNAILS_API UDATDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Thumbnail))
	TSoftObjectPtr<class UTexture2D> ThumbnailIcon = nullptr;
#endif
};

