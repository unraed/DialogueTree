// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Events/DialogueEvent.h"
//Plugin
#include "Dialogue.h"
#include "DialogueSpeakerComponent.h"
#include "DialogueSpeakerSocket.h"
#include "LogDialogueTree.h"

void UDialogueEvent::PlayEvent()
{
	check(Dialogue && Speaker);

	UDialogueSpeakerComponent* SpeakerComponent =
		Speaker->GetSpeakerComponent(Dialogue);

	if (!SpeakerComponent)
	{
		UE_LOG(
			LogDialogueTree, 
			Warning,
			TEXT("Failed to play event because the target speaker component was not supplied. Verify that the dialogue name property matches the speaker's role in the dialogue.")
		);
		return;
	}

	
	FSpeakerActorEntry TargetSpeaker = SpeakerComponent->ToSpeakerActorEntry();
	
	TArray<FSpeakerActorEntry> OtherSpeakers;
	for (UDialogueSpeakerSocket* Socket : AdditionalSpeakers)
	{
		UDialogueSpeakerComponent* SocketComponent = 
			Socket->GetSpeakerComponent(Dialogue);
		if (!SocketComponent)
		{
			UE_LOG(
				LogDialogueTree,
				Warning,
				TEXT("Failed to play event because the target speaker component was not supplied. Verify that the dialogue name property matches the speaker's role in the dialogue.")
			);
			return;
		}

		FSpeakerActorEntry SocketEntry = 
			SocketComponent->ToSpeakerActorEntry();
		OtherSpeakers.Add(SocketEntry);
	}

	OnPlayEvent(TargetSpeaker, OtherSpeakers);
}

bool UDialogueEvent::HasAllRequirements() const
{
	return Speaker != nullptr && IsValidEvent();
}

FText UDialogueEvent::GetGraphDescription_Implementation() const
{
	return FText::FromString(GetClass()->GetName());
}

bool UDialogueEvent::IsValidEvent_Implementation() const
{
	return true;
}

void UDialogueEvent::SetSpeaker(UDialogueSpeakerSocket* InSpeaker)
{
	Speaker = InSpeaker;
}

UDialogueSpeakerSocket* UDialogueEvent::GetSpeakerSocket() const
{
	return Speaker;
}

TArray<UDialogueSpeakerSocket*> UDialogueEvent::GetAdditionalSpeakerSockets() const
{
	return AdditionalSpeakers;
}
