// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "UObject/UnrealType.h"
#include "UObject/WeakFieldPtr.h"

class DATAASSETSTHUMBNAILSEDITOR_API FDATCustomDetails : public IDetailCustomization
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedPtr<IDetailCustomization> MakeInstance(TSharedPtr<IBlueprintEditor> InBlueprintEditor);

	FDATCustomDetails(UBlueprint* InBlueprintPtr);

	// IDetailCustomization interface
	void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

private:

	// Gets the supported property so we can get their meta data later
	FProperty* GetSupportedCustomizableProperty(class IDetailLayoutBuilder& DetailLayout);

	/** The blueprint we are editing */
	TWeakObjectPtr<UBlueprint> BlueprintPtr;
};
