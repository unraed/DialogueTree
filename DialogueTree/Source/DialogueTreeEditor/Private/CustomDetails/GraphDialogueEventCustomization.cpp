// Copyright Zachary Brett, 2024. All rights reserved.

#include "CustomDetails/GraphDialogueEventCustomization.h"
#include "DetailWidgetRow.h"
#include "Graph/Nodes/GraphNodeDialogueEvent.h"
#include "Events/DialogueEventBase.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"


#define LOCTEXT_NAMESPACE "FGraphDialogueEventCustomization"

void FGraphDialogueEventCustomization::CustomizeHeader(
	TSharedRef<IPropertyHandle> PropertyHandle, 
	FDetailWidgetRow& HeaderRow, 
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	//Only deal with one obj at a time
	if (CustomizationUtils.GetPropertyUtilities()
		->GetSelectedObjects().Num() > 1)
	{
		return;
	}

	//Text to display 
	FText EventDescription = LOCTEXT("DefaultDescription", "Invalid Event");

	//Get the target event
	UDialogueEventBase* TargetEvent = GetTargetEvent(PropertyHandle);
	if (TargetEvent 
		&& TargetEvent->HasAllRequirements())
	{
		EventDescription = TargetEvent->GetGraphDescription();
	}

	//Display text for header
	HeaderRow.NameContent()
	[
		SNew(SSpacer)
		.Size(FVector2D(5.f, 0.f))
	];

	HeaderRow.ValueContent()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Fill)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(EventDescription)
			.Justification(ETextJustify::Left)
		]
	];
}

void FGraphDialogueEventCustomization::CustomizeChildren(
	TSharedRef<IPropertyHandle> PropertyHandle, 
	IDetailChildrenBuilder& ChildBuilder, 
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	//Only deal with one obj at a time
	if (CustomizationUtils.GetPropertyUtilities()
		->GetSelectedObjects().Num() > 1)
	{
		return;
	}

	//Get property utilities
	TSharedPtr<IPropertyUtilities> Utils =
		CustomizationUtils.GetPropertyUtilities();
	check(Utils.IsValid());

	//Get num child properties
	uint32 NumChildProperties;
	PropertyHandle->GetNumChildren(NumChildProperties);

	//Add child properties unchanged
	for (uint32 ChildIndex = 0; ChildIndex < NumChildProperties; ChildIndex++)
	{
		TSharedPtr<IPropertyHandle> ChildHandle = 
			PropertyHandle->GetChildHandle(ChildIndex);
		
		if (ChildHandle.IsValid() && ChildHandle->IsValidHandle())
		{
			ChildBuilder.AddProperty(ChildHandle.ToSharedRef());

			ChildHandle->SetOnPropertyValueChanged(
				FSimpleDelegate::CreateLambda(
					[Utils]()
					{
						Utils->ForceRefresh();
					}
				)
			);
			ChildHandle->SetOnChildPropertyValueChanged(
				FSimpleDelegate::CreateLambda(
					[Utils]()
					{
						Utils->ForceRefresh();
					}
				)
			);
		}
	}
}

TSharedRef<IPropertyTypeCustomization> 
	FGraphDialogueEventCustomization::MakeInstance()
{
	return MakeShareable(new FGraphDialogueEventCustomization);
}

UDialogueEventBase* FGraphDialogueEventCustomization::GetTargetEvent(
	TSharedRef<IPropertyHandle> PropertyHandle)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return nullptr;
	}

	//Retrieve the target event handle
	TSharedPtr<IPropertyHandle> EventHandle = PropertyHandle->GetChildHandle(
		GET_MEMBER_NAME_CHECKED(FGraphDialogueEvent, Event)
	);

	if (!EventHandle.IsValid() || !EventHandle->IsValidHandle())
	{
		return nullptr;
	}

	//Retrieve the event object 
	UObject* OutValue;
	if (EventHandle->GetValue(OutValue) == FPropertyAccess::Fail
		|| !OutValue)
	{
		return nullptr;
	}

	return Cast<UDialogueEventBase>(OutValue);
}

#undef LOCTEXT_NAMESPACE