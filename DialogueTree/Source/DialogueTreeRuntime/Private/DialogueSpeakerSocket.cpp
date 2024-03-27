// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "DialogueSpeakerSocket.h"
//Plugin
#include "Dialogue.h"

void UDialogueSpeakerSocket::SetSpeakerName(FName InName)
{
	SpeakerName = InName;
}

FName UDialogueSpeakerSocket::GetSpeakerName() const
{
	return SpeakerName;
}

UDialogueSpeakerComponent* UDialogueSpeakerSocket::GetSpeakerComponent(
	UDialogue* InDialogue) const
{
	if (!InDialogue || SpeakerName.IsNone())
	{
		return nullptr;
	}

	return InDialogue->GetSpeaker(SpeakerName);
}

bool UDialogueSpeakerSocket::IsValidSocket() const
{
	if (SpeakerName.IsNone())
	{
		return false;
	}

	return true;
}
