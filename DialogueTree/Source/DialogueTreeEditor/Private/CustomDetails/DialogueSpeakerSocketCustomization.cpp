// Copyright Zachary Brett, 2024. All rights reserved.

#include "CustomDetails/DialogueSpeakerSocketCustomization.h"
#include "DetailWidgetRow.h"
#include "DialogueSpeakerSocket.h"
#include "Graph/DialogueEdGraph.h"
#include "Graph/Nodes/GraphNodeDialogue.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"
#include "SDialogueObjectPicker.h"

#define LOCTEXT_NAMESPACE "FDialogueSpeakerSocketCustomization"

TSharedRef<IPropertyTypeCustomization> 
FDialogueSpeakerSocketCustomization::MakeInstance()
{
	return MakeShareable(new FDialogueSpeakerSocketCustomization);
}

void FDialogueSpeakerSocketCustomization::CachePrerequisites(TSharedRef<IPropertyHandle> PropertyHandle)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	TargetProperty = PropertyHandle;
}

FText FDialogueSpeakerSocketCustomization::GetSelectionText() const
{
	FText SelectionText =
		LOCTEXT("EmptySelectionText", "None");

	//Retrieve targetted value 
	const UObject* RetrievedObj;

	if (TargetProperty->IsValidHandle()
		&& TargetProperty->GetValue(RetrievedObj) == FPropertyAccess::Success
		&& RetrievedObj != nullptr)
	{
		const UDialogueSpeakerSocket* SocketValue =
			Cast<UDialogueSpeakerSocket>(RetrievedObj);
		if (SocketValue)
		{
			SelectionText = FText::FromName(SocketValue->GetSpeakerName());
		}
	}

	return SelectionText;
}

TArray<UObject*> FDialogueSpeakerSocketCustomization::GetOptions() const
{
	TArray<UObject*> OptionArray;

	if (!TargetGraph.IsValid())
	{
		return OptionArray;
	}

	for (UDialogueSpeakerSocket* SocketOption : TargetGraph->GetAllSpeakers())
	{
		OptionArray.Add(SocketOption);
	}

	return OptionArray;
}

FName FDialogueSpeakerSocketCustomization::GetOptionName(UObject* InObj) const
{
	check(InObj);
	UDialogueSpeakerSocket* Socket =
		CastChecked<UDialogueSpeakerSocket>(InObj);
	return Socket->GetSpeakerName();
}

FText FDialogueSpeakerSocketCustomization::GetButtonTooltip() const
{
	return LOCTEXT(
		"ComboButtonTooltip",
		"Select a target speaker from the provided options"
	);
}

void FDialogueSpeakerSocketCustomization::SetUpPropertyChangeEvent(TSharedRef<IPropertyHandle> PropertyHandle)
{
	if (!TargetGraph.IsValid() || !PropertyHandle->IsValidHandle())
	{
		return;
	}

	//Set up the property change event to refresh node visuals
	UGraphNodeDialogue* TargetGraphNodePtr = TargetGraphNode.Get();
	PropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda(
		[TargetGraphNodePtr]()
		{
			if (TargetGraphNodePtr)
			{
				TargetGraphNodePtr->UpdateDialogueNode();
			}
		}
	));
}

void FDialogueSpeakerSocketCustomization::SetSelection(UObject* InSelection)
{
	if (!InSelection
		|| !TargetProperty.IsValid()
		|| !TargetProperty->IsValidHandle())
	{
		return;
	}

	UDialogueSpeakerSocket* SelectedSpeaker =
		Cast<UDialogueSpeakerSocket>(InSelection);

	if (!SelectedSpeaker)
	{
		return;
	}

	TargetProperty->SetValue(SelectedSpeaker);
	RefreshEditor();
}

bool FDialogueSpeakerSocketCustomization::HasPrerequisites() const
{
	return TargetProperty.IsValid() 
		&& TargetProperty->IsValidHandle() 
		&& TargetGraph.IsValid();
}

#undef LOCTEXT_NAMESPACE