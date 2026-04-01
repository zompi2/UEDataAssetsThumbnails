// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#include "DATRenderer.h"
#include "Engine/Texture2D.h"
#include "UnrealClient.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "AssetViewUtils.h"
#include "DATEditorUtils.h"
#include "DATSettings.h"
#include "IDataAssetWithThumbnail.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "GlobalRenderResources.h"
#include "Texture2DPreview.h"
#include "Slate/WidgetRenderer.h"
#include "SlateMaterialBrush.h"
#include "Styling/CoreStyle.h"

bool UDATRenderer::CanVisualizeAsset(UObject* Object)
{	
	// Prevent the potential inifinite loop
	if (bThumbnailLock)
	{
		return false;
	}
	bThumbnailLock = true;

	const bool bHasThumbnailMetaData = GetHasThumbnailMetaData(Object);

	bool bCanVisualize = false;
	UObject* AssetToVisualize = GetAssetToVisualizeFromObject(Object, bCanVisualize);

	// If this asset has "HasThumbnail" tag, but no correct asset to visualize is available,
	// clear the current custom thumbnail and remove the tag.
	if (bHasThumbnailMetaData && (bCanVisualize == false))
	{
		AssetViewUtils::ClearCustomThumbnails({ FAssetData(Object) });
		SetHasThumbnailMetaData(Object, false);
	}
	// If there is an asset to visualize available, but no "HasThumbnail" tag - add this tag.
	// Thumbnail will be set in the next phase.
	// The "HasThumbnail" tag prevents from Capturing Custom Thumbnail if a custom one from plugin is set.
	else if ((bHasThumbnailMetaData == false) && (bCanVisualize))
	{
		SetHasThumbnailMetaData(Object, true);
	}

	// Store the result now, return true if there is an Asset to Visualize.
	const bool bResult = bCanVisualize;

	// Release the lock
	bThumbnailLock = false;
	
	// Return the result
	return bResult;
}

UE_DISABLE_OPTIMIZATION

UObject* UDATRenderer::GetAssetToVisualizeFromObject(const UObject* Object, bool& bCanVisualize) const
{
	UObject* AssetToVisualize = nullptr;
	bCanVisualize = false;

	// Try to get a UObject from the interface.
	if (Object->Implements<UDataAssetWithThumbnail>())
	{
		AssetToVisualize = IDataAssetWithThumbnail::Execute_GetAssetToRender(Object);
		if (AssetToVisualize)
		{
			bCanVisualize = true;
			return AssetToVisualize;
		}
	}

	// If failed, try to get a UObject directly from the asset.
	if (AssetToVisualize == nullptr)
	{
		AssetToVisualize = FDATEditorUtils::FindSupportedThumbnailObjectInObject<FObjectProperty, UObject*>(Object, bCanVisualize);
	}
	
	if (AssetToVisualize == nullptr)
	{
		// If failed, try to get a Soft Object directly from the asset.
		FSoftObjectPtr AssetToVisualizeSoft = FDATEditorUtils::FindSupportedThumbnailObjectInObject<FSoftObjectProperty, FSoftObjectPtr>(Object, bCanVisualize);
		AssetToVisualize = AssetToVisualizeSoft.LoadSynchronous();
	}

	if (AssetToVisualize == nullptr)
	{
		// If failed, try to get a Slate Brush directly from the asset.
		FSlateBrush SlateBrush = FDATEditorUtils::FindSupportedThumbnailObjectInObject<FStructProperty, FSlateBrush>(Object, bCanVisualize);
		AssetToVisualize = SlateBrush.GetResourceObject();
	}

	if (AssetToVisualize == nullptr)
	{
		// If failed, try to get a Linear Color.
		// Can't work with it directly, so if found just return that information.
		FLinearColor SlateBrush = FDATEditorUtils::FindSupportedThumbnailObjectInObject<FStructProperty, FLinearColor>(Object, bCanVisualize);
	}

	return AssetToVisualize;
}

UE_ENABLE_OPTIMIZATION

void UDATRenderer::GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const
{
	OutWidth = 256;
	OutHeight = 256;

	bool bCanVisualize = false;
	UObject* AssetToVisualize = GetAssetToVisualizeFromObject(Object, bCanVisualize);
	if (AssetToVisualize == nullptr)
	{
		return;
	}

	UThumbnailRenderer* Renderer = GetThumbnailRenderer(AssetToVisualize);
	if (Renderer == nullptr)
	{
		return;
	}

	Renderer->GetThumbnailSize(AssetToVisualize, Zoom, OutWidth, OutHeight);
}

bool UDATRenderer::TryDrawUITextureWithSolidBGColor(UObject* Object, FCanvas* Canvas, FLinearColor BGColor, uint32 Width, uint32 Height)
{
	if (Canvas == nullptr)
	{
		return false;
	}

	UTexture2D* Texture = Cast<UTexture2D>(Object);
	if (Texture == nullptr)
	{
		return false;
	}
	
	if ((Texture->LODGroup != TEXTUREGROUP_UI) && (Texture->LODGroup != TEXTUREGROUP_Pixels2D))
	{
		return false;
	}
	
	Canvas->DrawTile(0, 0, Width, Height, 0, 0, 1, 1, BGColor, GWhiteTexture);

	FCanvasTileItem CanvasTile(FVector2D(0, 0), Texture->GetResource(), FVector2D(Width, Height), FLinearColor::White);
	TRefCountPtr<FBatchedElementParameters> BatchedElementParameters = new FBatchedElementTexture2DPreviewParameters(
		-1.f,
		0,
		-1.f,
		Texture->IsNormalMap(),
		Texture->CompressionSettings == TC_Grayscale || Texture->CompressionSettings == TC_Alpha,
		Texture->IsVirtualTexturedWithSinglePhysicalSpace(),
		false,
		false,
		false);

	CanvasTile.BlendMode = (ESimpleElementBlendMode)(SE_BLEND_RGBA_MASK_START + 0xF);
	CanvasTile.BatchedElementParameters = BatchedElementParameters;
	CanvasTile.Draw(Canvas);

	return true;
}

bool UDATRenderer::TryDrawUIMaterialWithSolidBGColor(UObject* Object, FRenderTarget* Viewport, FLinearColor BGColor, uint32 Width, uint32 Height)
{
	if (Viewport == nullptr)
	{
		return false;
	}

	UMaterialInterface* MaterialInterf = Cast<UMaterialInterface>(Object);
	if (MaterialInterf == nullptr)
	{
		return false;
	}

	UMaterial* Material = MaterialInterf->GetMaterial();
	if (Material == nullptr)
	{
		return false;
	}

	if (Material->IsUIMaterial() == false)
	{
		return false;
	}

	if ((Material->GetBlendMode() != BLEND_Masked) && (Material->GetBlendMode() != BLEND_Translucent))
	{
		return false;
	}	
	
	if (WidgetRenderer == nullptr)
	{
		WidgetRenderer = new FWidgetRenderer(true);
	}

	if (UIMaterialBrush == nullptr)
	{
		UIMaterialBrush = new FSlateMaterialBrush(FVector2D(SlateBrushDefs::DefaultImageSize, SlateBrushDefs::DefaultImageSize));
	}

	FSlateBrush SolidBrush = *FAppStyle::GetBrush("WhiteBrush");
	SolidBrush.ImageSize = FVector2D(GWhiteTexture->GetSizeX(), GWhiteTexture->GetSizeY());
	SolidBrush.Tiling = ESlateBrushTileType::Both;
	SolidBrush.TintColor = BGColor;

	UIMaterialBrush->SetMaterial(MaterialInterf);
	
	TSharedRef<SWidget> Thumbnail =
		SNew(SOverlay)
			+SOverlay::Slot()
			[
				SNew(SImage).Image(&SolidBrush)
			]
			+SOverlay::Slot()
			[
				SNew(SImage).Image(UIMaterialBrush)
			];

	WidgetRenderer->DrawWidget(Viewport, Thumbnail, FVector2D(Width, Height), 0.f);
	UIMaterialBrush->SetMaterial(nullptr);

	return true;
}

void UDATRenderer::DrawColor(FCanvas* Canvas, FLinearColor Color, uint32 Width, uint32 Height)
{
	if (Canvas)
	{
		Canvas->DrawTile(0, 0, Width, Height, 0, 0, 1, 1, Color, GWhiteTexture);
	}
}

void UDATRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	// Obtain the actual asset (Texture2D, MaterialInterface, SlateBrush resource object) used to draw the thumbnail.
	bool bCanVisualize = false;
	UObject* AssetToVisualize = GetAssetToVisualizeFromObject(Object, bCanVisualize);
	if (AssetToVisualize == nullptr && bCanVisualize)
	{
		// If there is no asset to visualize, but we can visualize, try to find a color struct property inside the object and use it as a thumbnail.
		FLinearColor LColor = FDATEditorUtils::FindSupportedThumbnailObjectInObject<FStructProperty, FLinearColor>(Object, bCanVisualize);
		if (bCanVisualize)
		{
			DrawColor(Canvas, LColor, Width, Height);
		}
		return;
	}
	
	// If the solid color on icons with translucency is enabled, try to draw the asset with the solid background.
	if (UDATSettings::Get()->SolidBackgroundOnTranslucentIcons(Object->GetClass()))
	{
		const FLinearColor BGColor = UDATSettings::Get()->GetAssetBGColor(Object->GetClass());

		// For Textures.
		if (TryDrawUITextureWithSolidBGColor(AssetToVisualize, Canvas, BGColor, Width, Height))
		{
			return;
		}

		// For Materials.
		if (TryDrawUIMaterialWithSolidBGColor(AssetToVisualize, Viewport, BGColor, Width, Height))
		{
			return;
		}
	}
		
	// The thumbnail hasn't been yet rendered. Try the standard render path.
	// Get the actual renderer for this object.
	UThumbnailRenderer* Renderer = GetThumbnailRenderer(AssetToVisualize);
	if (Renderer == nullptr)
	{
		return;
	}

	// Draw this object
	Renderer->Draw(AssetToVisualize, X, Y, Width, Height, Viewport, Canvas, bAdditionalViewFamily);
}

EThumbnailRenderFrequency UDATRenderer::GetThumbnailRenderFrequency(UObject* Object) const
{
	return EThumbnailRenderFrequency::OnPropertyChange;
}

bool UDATRenderer::GetHasThumbnailMetaData(UObject* Object) const
{
	if (Object == nullptr)
	{
		return false;
	}

	UPackage* Package = Object->GetPackage();
	if (Package == nullptr)
	{
		return false;
	}

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 6))
	FString* HasThumbnailMetaValue = Package->GetMetaData().RootMetaDataMap.Find(DAT::HasThumbnailTag);
#else
	UMetaData* MetaData = Package->GetMetaData();
	if (MetaData == nullptr)
	{
		return false;
	}
	FString* HasThumbnailMetaValue = MetaData->RootMetaDataMap.Find(DAT::HasThumbnailTag);
#endif

	if (HasThumbnailMetaValue)
	{
		if (*HasThumbnailMetaValue == TEXT("true"))
		{
			return true;
		}
	}

	return false;
}

void UDATRenderer::SetHasThumbnailMetaData(UObject* Object, bool bHasThumbnail)
{
	if (Object == nullptr)
	{
		return;
	}

	UPackage* Package = Object->GetPackage();
	if (Package == nullptr)
	{
		return;
	}

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 6))
	FMetaData& MetaData = Package->GetMetaData();
	if (bHasThumbnail)
	{
		MetaData.RootMetaDataMap.Add(DAT::HasThumbnailTag, TEXT("true"));
	}
	else
	{
		MetaData.RootMetaDataMap.Add(DAT::HasThumbnailTag, TEXT("false"));
	}
#else
	UMetaData* MetaData = Package->GetMetaData();
	if (MetaData == nullptr)
	{
		return;
	}

	if (bHasThumbnail)
	{
		MetaData->RootMetaDataMap.Add(DAT::HasThumbnailTag, TEXT("true"));
	}
	else
	{
		MetaData->RootMetaDataMap.Add(DAT::HasThumbnailTag, TEXT("false"));
	}
#endif

	Object->Modify();
	Object->MarkPackageDirty();
}

UThumbnailRenderer* UDATRenderer::GetThumbnailRenderer(UObject* Object) const
{
	if (FThumbnailRenderingInfo* Info = UThumbnailManager::Get().GetRenderingInfo(Object))
	{
		return Info->Renderer;
	}
	return nullptr;
}
