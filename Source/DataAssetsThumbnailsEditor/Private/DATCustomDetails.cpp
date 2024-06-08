// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#include "DATCustomDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "IDetailGroup.h"
#include "BlueprintEditorModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "DATUtils.h"
#include "DATTypes.h"

UE_DISABLE_OPTIMIZATION

FDATCustomDetails::FDATCustomDetails(UBlueprint* InBlueprintPtr) 
: BlueprintPtr(InBlueprintPtr)
{
}

TSharedPtr<IDetailCustomization> FDATCustomDetails::MakeInstance(TSharedPtr<IBlueprintEditor> InBlueprintEditor)
{
	if (InBlueprintEditor.IsValid() == false)
	{
		return nullptr;
	}

	const TArray<UObject*>* Objects = InBlueprintEditor->GetObjectsCurrentlyBeingEdited();
	if (Objects == nullptr || Objects->Num() != 1)
	{
		return nullptr;
	}

	UBlueprint* Blueprint = Cast<UBlueprint>((*Objects)[0]);
	if (Blueprint == nullptr)
	{
		return nullptr;
	}
	
	if (Blueprint->ParentClass && Blueprint->ParentClass->IsChildOf<UDataAsset>())
	{
		return MakeShareable(new FDATCustomDetails(Blueprint));
	}

	return nullptr;
}

// IDetailCustomization interface
void FDATCustomDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	FProperty* Property = GetSupportedCustomizableProperty(DetailLayout);
	if (Property == nullptr)
	{
		return;
	}

	const FName PropertyName = Property->GetFName();

	DetailLayout.EditCategory("Variable")
		.AddCustomRow(INVTEXT("Hello"))
		.NameContent()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(INVTEXT("Is Thumbnail"))
				.ToolTipText(INVTEXT("Some Tooltip"))
			]
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SCheckBox)
				.ToolTipText(INVTEXT("Some Tooltip"))
				.IsChecked_Lambda([this, PropertyName]() -> ECheckBoxState
				{
					FString TmpResult;
					const bool bResult = FBlueprintEditorUtils::GetBlueprintVariableMetaData(BlueprintPtr.Get(), PropertyName, nullptr, DAT::ThumbnailTag, TmpResult);
					return bResult ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
				})
				.OnCheckStateChanged_Lambda([this, PropertyName](ECheckBoxState NewState) -> void
				{
					if (NewState == ECheckBoxState::Checked)
					{
						FBlueprintEditorUtils::SetBlueprintVariableMetaData(BlueprintPtr.Get(), PropertyName, nullptr, DAT::ThumbnailTag, TEXT(""));
						BlueprintPtr->Modify();
					}
					if (NewState == ECheckBoxState::Unchecked)
					{
						FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(BlueprintPtr.Get(), PropertyName, nullptr, DAT::ThumbnailTag);
						BlueprintPtr->Modify();
					}
				})
			]
		];
}

FProperty* FDATCustomDetails::GetSupportedCustomizableProperty(IDetailLayoutBuilder& DetailLayout)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailLayout.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	if (ObjectsBeingCustomized.Num() != 1)
	{
		return nullptr;
	}
	UPropertyWrapper* PropertyWrapper = Cast<UPropertyWrapper>(ObjectsBeingCustomized[0].Get());
	if (PropertyWrapper == nullptr)
	{
		return nullptr;
	}

	const TWeakFieldPtr<FProperty> PropertyBeingCustomized = PropertyWrapper ? PropertyWrapper->GetProperty() : nullptr;
	FProperty* Property = PropertyBeingCustomized.IsValid() ? PropertyBeingCustomized.Get() : nullptr;

	if (FDATUtils::IsPropertySupported(Property) == false)
	{
		return nullptr;
	}

	return Property;
}

UE_ENABLE_OPTIMIZATION