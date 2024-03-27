// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Nodes/GraphNodeDialogueBase.h"

#define LOCTEXT_NAMESPACE "GraphNodeDialogueBase"

UGraphNodeDialogueBase::UGraphNodeDialogueBase()
{
	bCanRenameNode = false;
}

void UGraphNodeDialogueBase::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr)
	{
		//Get the node's input and output pin
		TArray<UEdGraphPin*> OutputPins = GetOutputPins();
		TArray<UEdGraphPin*> InputPins = GetInputPins();

		UEdGraphPin* OutputPin = nullptr;
		UEdGraphPin* InputPin = nullptr;

		if (OutputPins.IsEmpty() == false)
		{
			OutputPin = OutputPins[0];
		}

		if (InputPins.IsEmpty() == false)
		{
			InputPin = InputPins[0];
		}

		//Try to make a connection 
		if (InputPin 
			&& GetSchema()->TryCreateConnection(FromPin, InputPin))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
		else if (OutputPin
			&& GetSchema()->TryCreateConnection(OutputPin, FromPin))
		{
			NodeConnectionListChanged();
		}
	}
}

EDialogueConnectionLimit UGraphNodeDialogueBase::GetInputConnectionLimit() const
{
	return EDialogueConnectionLimit::Unlimited;
}

EDialogueConnectionLimit UGraphNodeDialogueBase::GetOutputConnectionLimit() 
	const
{
	return EDialogueConnectionLimit::Single;
}

TArray<UEdGraphPin*> UGraphNodeDialogueBase::GetInputPins() const
{
	return GetPinsWithDirection(EGPD_Input);
}

TArray<UEdGraphPin*> UGraphNodeDialogueBase::GetOutputPins() const
{
	return GetPinsWithDirection(EGPD_Output);
}

const TArray<UGraphNodeDialogueBase*> 
	UGraphNodeDialogueBase::GetDirectParents() const
{
	TArray<UGraphNodeDialogueBase*> ParentNodes;

	for (UEdGraphPin* InputPin : GetInputPins())
	{
		ParentNodes.Append(GetPinConnectedNodes(InputPin));
	}

	return ParentNodes;
}

const TArray<UGraphNodeDialogueBase*> 
	UGraphNodeDialogueBase::GetDirectChildren() const
{
	TArray<UGraphNodeDialogueBase*> ChildNodes;

	for (UEdGraphPin* OutputPin : GetOutputPins())
	{
		ChildNodes.Append(GetPinConnectedNodes(OutputPin));
	}

	return ChildNodes;
}

FText UGraphNodeDialogueBase::GetPinMenuLabel(int32 PinIndex) const
{
	check(Pins.IsValidIndex(PinIndex));
	UEdGraphPin* TargetPin = Pins[PinIndex];
	check(TargetPin);

	if (TargetPin->Direction == EGPD_Input)
	{
		return LOCTEXT("InputPinMenuLabel", "Input Pin");
	}
	else
	{
		return LOCTEXT("OutputPinMenuLabel", "Output Pin");
	}
}

const TArray<UGraphNodeDialogueBase*>
	UGraphNodeDialogueBase::GetPinConnectedNodes(UEdGraphPin* SourcePin) const
{
	TArray<UGraphNodeDialogueBase*> ConnectedNodes;
	TArray<UEdGraphPin*> ConnectedPins = SourcePin->LinkedTo;
	
	for (UEdGraphPin* Pin : ConnectedPins)
	{
		UGraphNodeDialogueBase* Node = 
			Cast<UGraphNodeDialogueBase>(Pin->GetOwningNode());
		if (Node)
		{
			ConnectedNodes.Add(Node);
		}
	}

	return ConnectedNodes;
}

TArray<UEdGraphPin*> UGraphNodeDialogueBase::GetPinsWithDirection(
	EEdGraphPinDirection Direction) const
{
	TArray<UEdGraphPin*> FoundPins;

	for (UEdGraphPin* CurrentPin : Pins)
	{
		if (CurrentPin->Direction == Direction)
		{
			FoundPins.Add(CurrentPin);
		}
	}

	return FoundPins;
}

FText UGraphNodeDialogueBase::GetContextMenuName() const
{
	return FText::GetEmpty();
}

#undef LOCTEXT_NAMESPACE