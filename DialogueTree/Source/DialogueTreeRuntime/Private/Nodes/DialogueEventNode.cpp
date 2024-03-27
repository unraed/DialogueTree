// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Nodes/DialogueEventNode.h"
//Plugin
#include "Dialogue.h"
#include "Events/DialogueEventBase.h"

void UDialogueEventNode::EnterNode()
{
	//Call super
	Super::EnterNode();

	//Play all events
	for (UDialogueEventBase* Event : Events)
	{
		Event->PlayEvent();
	}

	//Trigger the next node in line
	if (!Children.IsEmpty())
	{
		Dialogue->TraverseNode(Children[0]);
	}
	else
	{
		Dialogue->EndDialogue();
	}
}

FDialogueOption UDialogueEventNode::GetAsOption()
{
	if (!Children.IsEmpty() && Children[0] != nullptr)
	{
		FSpeechDetails OptionDetails = Children[0]->GetAsOption().Details;
		return FDialogueOption{ OptionDetails, this };
	}

	return FDialogueOption();
}

void UDialogueEventNode::SetEvents(TArray<UDialogueEventBase*>& InEvents)
{
	Events = InEvents;
}

void UDialogueEventNode::PlayEvents()
{
	for (UDialogueEventBase* Event : Events)
	{
		Event->PlayEvent();
	}
}
