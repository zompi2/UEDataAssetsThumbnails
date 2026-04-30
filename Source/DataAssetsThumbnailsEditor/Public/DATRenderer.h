// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#pragma once

/**
 * Custom renderer class used for rendering Data Asset thumbnails.
 */

#include "ThumbnailRendering/TextureThumbnailRenderer.h"
#include "DATUtils.h"
#include "DATRenderer.generated.h"

UCLASS()
class DATAASSETSTHUMBNAILSEDITOR_API UDATRenderer : public UThumbnailRenderer
{
	GENERATED_BODY()

public:

	/**
	 * Returns true if this renderer can be used for visualizing the asset for the given Object.
	 */
	bool CanVisualizeAsset(UObject* Object);

	/**
	 * Gets the desired thumbnail size for the given asset.
	 */
	void GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const override;
	
	/**
	 * Draws the asset for this given Object.
	 */
	void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, class FRenderTarget* Viewport, class FCanvas* Canvas, bool bAdditionalViewFamily) override;
	
	/**
	 * Returns the thumbnail refresh frequency.
	 */
	EThumbnailRenderFrequency GetThumbnailRenderFrequency(UObject* Object) const;

protected:
	
	/**
	 * Searches for the object with "Thumbnail" meta data tag inside of the given Object.
	 * This object will be used to visualize the thumbnail.
	 */
	UObject* GetAssetToVisualizeFromObject(const UObject* Object) const;

	/**
	 * Searches for the color struct with "Thumbnail" meta tag inside.
	 * This color will be used to visualize the thumbnail.
	 * Check bOutFoundColor to see if the color struct was found.
	 */
	FLinearColor GetColorToVisualizeFromObject(UObject* Object, bool& bOutFoundColor) const;

private: 

	/** Used to prevent infinite loop when calling CanVisualizeAsset. */
	bool bThumbnailLock = false;

	/**
	 * Gets the correct thumbnail renderer for the Object used to render the thumbnail.
	 */
	UThumbnailRenderer* GetThumbnailRenderer(UObject* Object) const;

	/**
	 * Checks if the given asset has "HasThumbnail" tag.
	 */
	bool GetHasThumbnailMetaData(UObject* Object) const;

	/**
	 * Sets the "HasThumbnail" tag for this asset.
	 */
	void SetHasThumbnailMetaData(UObject* Object, bool bHasThumbnail);

	/**
	 * Tries to draw the translucent UI Texture with the solid background.
	 * Returns true if success.
	 */
	bool TryDrawUITextureWithSolidBGColor(UObject* Object, class FCanvas* Canvas, FLinearColor BGColor, uint32 Width, uint32 Height);

	/**
	 * Tries to draw the tranlucent UI Material with the solid background.
	 * Returns true if success.
	 */
	bool TryDrawUIMaterialWithSolidBGColor(UObject* Object, class FRenderTarget* Viewport, FLinearColor BGColor, uint32 Width, uint32 Height);

	/**
	 * Draws the given color on the whole thumbnail.
	 */
	void DrawColor(class FCanvas* Canvas, FLinearColor Color, uint32 Width, uint32 Height);

	/** Widget renderer required for Material thumbnails custom drawing. */
	class FWidgetRenderer* WidgetRenderer = nullptr;

	/** Slate Material Brush required for Material thumbnails custom drawing. */
	struct FSlateMaterialBrush* UIMaterialBrush = nullptr;
};
