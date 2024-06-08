// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "ThumbnailRendering/TextureThumbnailRenderer.h"
#include "DATUtils.h"
#include "DATRenderer.generated.h"

UCLASS()
class DATAASSETSTHUMBNAILSEDITOR_API UDATRenderer : public UThumbnailRenderer
{
	GENERATED_BODY()

public:

	bool CanVisualizeAsset(UObject* Object);
	void GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const override;
	void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, class FRenderTarget* Viewport, class FCanvas* Canvas, bool bAdditionalViewFamily) override;
	EThumbnailRenderFrequency GetThumbnailRenderFrequency(UObject* Object) const;

protected:
	
	UObject* GetAssetToVisualizeFromObject(const UObject* Object) const;

private: 

	UThumbnailRenderer* GetThumbnailRenderer(UObject* Object) const;
	bool bThumbnailLock = false;

	bool GetHasThumbnailMetaData(UObject* Object) const;
	void SetHasThumbnailMetaData(UObject* Object, bool bHasThumbnail);
};
