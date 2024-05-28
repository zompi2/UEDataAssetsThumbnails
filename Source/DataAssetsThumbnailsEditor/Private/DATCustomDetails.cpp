// Copyright (c) 2023 Damian Nowakowski. All rights reserved.

#include "DATCustomDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailGroup.h"
#include "BlueprintEditorModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "DATSupportedTypes.h"

UE_DISABLE_OPTIMIZATION

TSharedPtr<IDetailCustomization> FDATCustomDetails::MakeInstance(TSharedPtr<IBlueprintEditor> InBlueprintEditor)
{
	static_assert(TDATSupportedType<UTexture2D>::Value, "Unsupported Type");

	const TArray<UObject*>* Objects = (InBlueprintEditor.IsValid() ? InBlueprintEditor->GetObjectsCurrentlyBeingEdited() : nullptr);
	if (Objects && Objects->Num() == 1)
	{
		if (UBlueprint* Blueprint = Cast<UBlueprint>((*Objects)[0]))
		{
			if (Blueprint->ParentClass->IsChildOf<UDataAsset>())
			{
				return MakeShareable(new FDATCustomDetails(InBlueprintEditor, Blueprint));
			}
		}
	}

	return nullptr;
}

// IDetailCustomization interface
void FDATCustomDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailLayout.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	if (ObjectsBeingCustomized.Num() == 1)
	{
		UPropertyWrapper* PropertyWrapper = Cast<UPropertyWrapper>(ObjectsBeingCustomized[0].Get());
		const TWeakFieldPtr<FProperty> PropertyBeingCustomized = PropertyWrapper ? PropertyWrapper->GetProperty() : nullptr;
		if (PropertyBeingCustomized.IsValid())
		{
			FProperty* Property = PropertyBeingCustomized.Get();
			
			if (Property)
			{
				if (Property->GetClass()->IsChildOf(FObjectProperty::StaticClass()))
				{
					FObjectProperty* ObjectProperty = static_cast<FObjectProperty*>(Property);
					const FName varName = PropertyBeingCustomized->GetFName();

					if (ObjectProperty->PropertyClass->IsChildOf<UTexture2D>())
					{
						DetailLayout.EditCategory("Variable")
						.AddCustomRow(INVTEXT("Hello"))
						.NameContent()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.AutoWidth()
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SNew(STextBlock)
								.Text(INVTEXT("Is Thumbnail"))
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
								.IsChecked_Lambda([this, ObjectProperty, varName]() -> ECheckBoxState
								{
									FString tmp;
									bool bResult = FBlueprintEditorUtils::GetBlueprintVariableMetaData(BlueprintPtr.Get(), varName, nullptr, "Thumbnail", tmp);
									return bResult ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
								})
								.OnCheckStateChanged_Lambda([this, ObjectProperty, varName](ECheckBoxState NewState) -> void
								{
									if (NewState == ECheckBoxState::Checked)
									{
										FBlueprintEditorUtils::SetBlueprintVariableMetaData(BlueprintPtr.Get(), varName, nullptr, "Thumbnail", "");
										//ObjectProperty->SetMetaData("Thumbnail", "");
										BlueprintPtr->Modify();
									}
									if (NewState == ECheckBoxState::Unchecked)
									{
										FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(BlueprintPtr.Get(), varName, nullptr, "Thumbnail");
										//ObjectProperty->RemoveMetaData("Thumbnail");
										BlueprintPtr->Modify();
									}
								})
							]
						];
					}
				}
			}
		}
	}
}

UE_ENABLE_OPTIMIZATION