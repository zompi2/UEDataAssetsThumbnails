// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * Handles the styling of the editor.
 * Currently used only for menu icon.
 */

class DATAASSETSTHUMBNAILSEDITOR_API FDATEditorStyle
{
public:

	/**
	 * Initialize the style container.
	 */
	static void Initialize();

	/**
	 * Shutdown/cleanup style container.
	 */
	static void Shutdown();
	
	/** 
	 * Reloads textures used by slate renderer.
	 */
	static void ReloadTextures();

	/**
	 * Returns the ELT style set name. 
	 */
	static FName GetStyleSetName();

private:

	/**
	 * Creates an instance of the style set.
	 */
	static TSharedRef< class FSlateStyleSet > Create();

	// The static instance of style set.
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};