// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Nodes/DialogueEventNode.h"
//Plugin
#include "Dialogue.h"
#include "Events/DialogueEventBase.h"

void UDialogueEventNode::EnterNode()
{
	//Play all events
	PlayEvents();

	//Trigger the next node in line if no event is blocking
	TransitionIfNotBlocking();
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

void UDialogueEventNode::Skip()
{
	for (UDialogueEventBase* Event : Events)
	{
		Event->OnSkipped();
	}
}

void UDialogueEventNode::SetEvents(TArray<UDialogueEventBase*>& InEvents)
{
	Events = InEvents;
}

bool UDialogueEventNode::GetIsBlocking() const
{
	for (UDialogueEventBase* Event : Events)
	{
		if (Event->GetIsBlocking())
		{
			return true;
		}
	}

	return false;
}

void UDialogueEventNode::PlayEvents()
{
	for (UDialogueEventBase* Event : Events)
	{
		// Subscribe to the event's callback for stopping blocking
		Event->OnStoppedBlocking.BindUFunction(
			this,
			"TransitionIfNotBlocking"
		);

		// Play the event
		Event->PlayEvent();
	}
}

void UDialogueEventNode::TransitionIfNotBlocking() const
{
	if (GetIsBlocking())
	{
		return;
	}

	if (!Children.IsEmpty())
	{
		Dialogue->TraverseNode(Children[0]);
	}
	else
	{
		Dialogue->EndDialogue();
	}
}
