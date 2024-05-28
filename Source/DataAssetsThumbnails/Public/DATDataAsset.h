// Copyright Epic Games, Inc. All Rights Reserved.

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
	TObjectPtr<class UTexture2D> ThumbnailIcon = nullptr;
#endif
};

