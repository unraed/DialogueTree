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
