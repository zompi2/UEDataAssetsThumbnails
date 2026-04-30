// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "DATEditorStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

// Null declaration of static variable (for linker needs)
TSharedPtr<FSlateStyleSet> FDATEditorStyle::StyleInstance = nullptr;

void FDATEditorStyle::Initialize()
{
	if (StyleInstance.IsValid() == false)
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FDATEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FDATEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

FName FDATEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("DATEditorStyle"));
	return StyleSetName;
}

TSharedRef<FSlateStyleSet> FDATEditorStyle::Create()
{
	// Create a new Style Set with a content root set to Resources directory of the plugin.
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("DataAssetsThumbnails")->GetBaseDir() / TEXT("Resources"));

	// Create a new Slate Image Brush, which is Icon16.svg from Resources directory.
	FSlateVectorImageBrush* Brush = new FSlateVectorImageBrush(Style->RootToContentDir(TEXT("Icon16"), TEXT(".svg")), { 16.f, 16.f });
	
	// Add newly created Brush to the Style Set.
	Style->Set("DATEditorStyle.MenuIcon", Brush);
	
	// Result is a Style Set with menu icon in it.
	return Style;
}
