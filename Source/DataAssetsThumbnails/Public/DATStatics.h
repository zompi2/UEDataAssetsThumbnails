// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Styling/SlateBrush.h"
#include "DATStatics.generated.h"

class UTexture2D;
class UMaterialInterface;

UCLASS()
class DATAASSETSTHUMBNAILS_API UDATStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/**
	 * Converts Texture2D to the object valid for Data Asset thumbnail rendering.
	 */
	UFUNCTION(BlueprintPure, Category = "Data Assets Thumbnails")
	UObject* TextureToThumbnailRenderObject(UTexture2D* InTexture);

	/**
	 * Converts Material or Material Instance to the object valid for Data Asset thumbnail rendering.
	 */
	UFUNCTION(BlueprintPure, Category = "Data Assets Thumbnails")
	UObject* MaterialToThumbnailRenderObject(UMaterialInterface* InMaterial);

	/**
	 * Converts Slate Brush to the object valid for Data Asset thumbnail rendering.
	 */
	UFUNCTION(BlueprintPure, Category = "Data Assets Thumbnails")
	UObject* SlateBrushToThumbnailRenderObject(const FSlateBrush& InBrush);
};
