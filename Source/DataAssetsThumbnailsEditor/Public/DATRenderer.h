// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ThumbnailRendering/TextureThumbnailRenderer.h"
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
	
	virtual UObject* GetObject(UObject* Object) const;

private: 

	UThumbnailRenderer* GetRenderer(UObject* Object) const;
	bool bThumbnailLock = false;
};
