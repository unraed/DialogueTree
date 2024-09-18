// Copyright Zachary Brett, 2024. All rights reserved.


#include "Events/DialogueEventBase.h"

bool UDialogueEventBase::HasAllRequirements() const
{
	return true;
}

FText UDialogueEventBase::GetGraphDescription_Implementation() const
{
	return FText();
}

bool UDialogueEventBase::GetIsBlocking() const
{
	return bBlocking;
}

void UDialogueEventBase::StartBlocking()
{
	bBlocking = true;
}

void UDialogueEventBase::StopBlocking()
{
	//Set us to no longer block
	bBlocking = false;

	//Broadcast to all interested parties that we are not blocking
	OnStoppedBlocking.ExecuteIfBound();

	//Clear the list of interested parties
	OnStoppedBlocking.Unbind();
}

void UDialogueEventBase::PlayEvent()
{
}

void UDialogueEventBase::SetDialogue(UDialogue* InDialogue)
{
	if (!InDialogue)
	{
		return;
	}

	Dialogue = InDialogue;
}
