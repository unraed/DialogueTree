// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Events/DialogueEvent.h"
//Unreal
#include "Engine/World.h"
//Plugin
#include "Dialogue.h"
#include "DialogueSpeakerComponent.h"
#include "DialogueSpeakerSocket.h"
#include "LogDialogueTree.h"

void UDialogueEvent::PlayEvent()
{
	check(Dialogue && Speaker);

	bBlocking = false;
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

AActor* UDialogueEvent::SpawnActorToCurrentWorld(
	TSubclassOf<AActor> ActorClass, const FVector Location, 
	const FRotator Rotation,
	AActor* Owner,
	ESpawnActorCollisionHandlingMethod CollisionHandlingMethod)
{
	if (!Speaker || !Dialogue || !ActorClass)
	{
		return nullptr;
	}

	UDialogueSpeakerComponent* SpeakerComponent = 
		Speaker->GetSpeakerComponent(Dialogue);
	if (!SpeakerComponent)
	{
		return nullptr;
	}

	UWorld* World = SpeakerComponent->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.SpawnCollisionHandlingOverride =
		CollisionHandlingMethod;
	return World->SpawnActor(
		ActorClass, 
		&Location, 
		&Rotation, 
		SpawnParams
	);
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

const FSpeechDetails UDialogueEvent::GetCurrentSpeechDetails() const
{
	if (UDialogueSpeechNode* Speech = 
		GetTypedOuter<UDialogueSpeechNode>())
	{
		return Speech->GetDetails();
	}

	return FSpeechDetails();
}
