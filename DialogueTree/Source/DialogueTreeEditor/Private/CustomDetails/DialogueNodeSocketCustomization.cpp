// Copyright Zachary Brett, 2024. All rights reserved.

#include "CustomDetails/DialogueNodeSocketCustomization.h"
#include "DetailWidgetRow.h"
#include "Dialogue.h"
#include "DialogueNodeSocket.h"
#include "Graph/DialogueEdGraph.h"
#include "Graph/Nodes/GraphNodeDialogue.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"
#include "SDialogueObjectPicker.h"

#define LOCTEXT_NAMESPACE "DialogueNodeSocketCustomization"

TSharedRef<IPropertyTypeCustomization> FDialogueNodeSocketCustomization::MakeInstance()
{
	return MakeShareable(new FDialogueNodeSocketCustomization);
}

void FDialogueNodeSocketCustomization::CachePrerequisites(TSharedRef<IPropertyHandle> PropertyHandle)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	//Get outer
	TArray<UObject*> OuterObjs;
	PropertyHandle->GetOuterObjects(OuterObjs);
	if (OuterObjs.IsEmpty())
	{
		return;
	}
	UDialogue* TargetDialogue = OuterObjs[0]->GetTypedOuter<UDialogue>();
	if (!TargetDialogue)
	{
		return;
	}

	//Get the target object
	UObject* PropertyValue;
	PropertyHandle->GetValue(PropertyValue);

	//If target object doesn't exist, create it
	if (PropertyValue == nullptr)
	{
		UDialogueNodeSocket* NewSocket = 
			NewObject<UDialogueNodeSocket>(TargetDialogue);
		TargetSocket = NewSocket;
		if (PropertyHandle->SetValue(NewSocket) == FPropertyAccess::Fail)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to access property"));
			return;
		}
	}
	//fetch the target object
	else
	{
		UDialogueNodeSocket* SocketValue =
			Cast<UDialogueNodeSocket>(PropertyValue);
		if (!SocketValue)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to retrieve socket"));
			return;
		}
		TargetSocket = SocketValue;
	}
}

FText FDialogueNodeSocketCustomization::GetSelectionText() const
{
	FText SelectionText =
		LOCTEXT("EmptySelectionText", "None");

	if (TargetSocket.IsValid() && TargetSocket->GetGraphNode())
	{
		const UGraphNodeDialogue* NodeValue = 
			Cast<UGraphNodeDialogue>(TargetSocket->GetGraphNode());

		if (NodeValue)
		{
			SelectionText = FText::FromName(NodeValue->GetID());
		}
	}

	return SelectionText;
}

TArray<UObject*> FDialogueNodeSocketCustomization::GetOptions() const
{
	TArray<UObject*> OptionArray;

	if (!TargetGraph.IsValid())
	{
		return OptionArray;
	}

	for (UGraphNodeDialogue* Node : TargetGraph->GetAllNodes())
	{
		OptionArray.Add(Node);
	}

	return OptionArray;
}

FName FDialogueNodeSocketCustomization::GetOptionName(UObject* InObj) const
{
	check(InObj);
	UGraphNodeDialogue* Node =
		CastChecked<UGraphNodeDialogue>(InObj);
	return Node->GetID();
}

FText FDialogueNodeSocketCustomization::GetButtonTooltip() const
{
	return LOCTEXT(
		"ButtonTooltip", 
		"Select a target node from the listed options"
	);
}

void FDialogueNodeSocketCustomization::SetUpPropertyChangeEvent(TSharedRef<IPropertyHandle> PropertyHandle)
{
	//Todo: Can I replace this with a blanket version on all nodes? UObject has something like this...
	if (!TargetSocket.IsValid() || !PropertyHandle->IsValidHandle())
	{
		return;
	}
	//Set up the property change event to refresh node visuals
	UDialogueNodeSocket* TargetSocketPtr = TargetSocket.Get();

	PropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda(
		[TargetSocketPtr]()
		{
			if (!TargetSocketPtr || !TargetSocketPtr->GetGraphNode())
			{
				return;
			}

			UGraphNodeDialogue* GraphNode = 
				Cast<UGraphNodeDialogue>(TargetSocketPtr->GetGraphNode());
			if (!GraphNode)
			{
				TargetSocketPtr->SetDisplayID(FText());
				return;
			}

			GraphNode->UpdateDialogueNode();
			TargetSocketPtr->SetDisplayID(
				FText::FromName(GraphNode->GetID())
			);
		}
	));
}

void FDialogueNodeSocketCustomization::SetSelection(UObject* InSelection)
{
	if (!InSelection || !TargetSocket.IsValid())
	{
		return;
	}

	UGraphNodeDialogue* SelectedNode = Cast<UGraphNodeDialogue>(InSelection);

	if (SelectedNode)
	{
		TargetSocket->SetGraphNode(SelectedNode);
		TargetSocket->SetDisplayID(FText::FromName(SelectedNode->GetID()));
	}

	RefreshEditor();
}

bool FDialogueNodeSocketCustomization::HasPrerequisites() const
{
	return TargetSocket.IsValid() && TargetGraph.IsValid();
}

#undef LOCTEXT_NAMESPACE