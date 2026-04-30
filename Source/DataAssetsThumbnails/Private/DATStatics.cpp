// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "DATStatics.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInterface.h"
#include "DATUtils.h"

UObject* UDATStatics::TextureToThumbnailRenderObject(UTexture2D* InTexture)
{
	return FDATUtils::GetRenderObjectFromObject(InTexture);
}

UObject* UDATStatics::MaterialToThumbnailRenderObject(UMaterialInterface* InMaterial)
{
	return FDATUtils::GetRenderObjectFromObject(InMaterial);
}

UObject* UDATStatics::SlateBrushToThumbnailRenderObject(const FSlateBrush& InBrush)
{
	return FDATUtils::GetRenderObjectFromObject(InBrush);
}
