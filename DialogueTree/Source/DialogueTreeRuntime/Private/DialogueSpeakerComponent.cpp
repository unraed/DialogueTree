// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "DialogueSpeakerComponent.h"
//UE
#include "Engine/World.h"
//Plugin
#include "Dialogue.h"
#include "DialogueController.h"
#include "DialogueManagerSubsystem.h"
#include "LogDialogueTree.h"

UDialogueSpeakerComponent::UDialogueSpeakerComponent()
{
	//Prevent the speaker from ticking
	PrimaryComponentTick.bCanEverTick = false;
}

void UDialogueSpeakerComponent::BeginPlay()
{
	Super::BeginPlay();

	UDialogueManagerSubsystem* DialogueSubsystem = 
		GetWorld()->GetSubsystem<UDialogueManagerSubsystem>();
	check(DialogueSubsystem);

	DialogueController = DialogueSubsystem->GetCurrentController();
	if (!DialogueController)
	{
		UE_LOG(
			LogDialogueTree, 
			Error, 
			TEXT("Speaker component failed to find an active dialogue controller")
		);
		return;
	}
}

void UDialogueSpeakerComponent::SetDisplayName(FText InDisplayName)
{
	if (InDisplayName.IsEmpty())
	{
		return;
	}

	DisplayName = InDisplayName;
}

void UDialogueSpeakerComponent::SetDialogueName(FName InDialogueName)
{
	if (InDialogueName.IsNone())
	{
		return;
	}

	DialogueName = InDialogueName;
}

FText UDialogueSpeakerComponent::GetDisplayName() const
{
	return DisplayName;
}

FName UDialogueSpeakerComponent::GetDialogueName() const
{
	return DialogueName;
}

void UDialogueSpeakerComponent::SetOwnedDialogue(UDialogue* InDialogue)
{
	OwnedDialogue = InDialogue;
}

UDialogue* UDialogueSpeakerComponent::GetOwnedDialogue()
{
	return OwnedDialogue;
}

FGameplayTagContainer UDialogueSpeakerComponent::GetCurrentGameplayTags()
{
	return GameplayTags;
}

void UDialogueSpeakerComponent::EndCurrentDialogue()
{
	if (!DialogueController 
		|| !DialogueController->SpeakerInCurrentDialogue(this))
	{
		return;
	}

	DialogueController->EndDialogue();
}

void UDialogueSpeakerComponent::TrySkipSpeech()
{
	if (!DialogueController || 
		!DialogueController->SpeakerInCurrentDialogue(this))
	{
		return;
	}

	DialogueController->Skip();
}

void UDialogueSpeakerComponent::PlaySpeechAudioClip_Implementation(
	USoundBase* InAudio)
{
	SetSound(InAudio);
	Play();
}

void UDialogueSpeakerComponent::SetCurrentGameplayTags(FGameplayTagContainer InTags)
{
	GameplayTags.Reset();
	GameplayTags.AppendTags(InTags);
	BroadcastCurrentGameplayTags();
}

void UDialogueSpeakerComponent::ClearGameplayTags()
{
	GameplayTags.Reset();
	BroadcastCurrentGameplayTags();
}

void UDialogueSpeakerComponent::StartOwnedDialogueWithNames(
	TMap<FName, UDialogueSpeakerComponent*> InSpeakers, bool bResume)
{
	if (OwnedDialogue)
	{
		StartDialogueWithNames(OwnedDialogue, InSpeakers, bResume);
	}
}

void UDialogueSpeakerComponent::StartOwnedDialogue(
	TArray<UDialogueSpeakerComponent*> InSpeakers, bool bResume)
{
	if (OwnedDialogue)
	{
		StartDialogue(OwnedDialogue, InSpeakers, bResume);
	}
}

void UDialogueSpeakerComponent::StartDialogueWithNames(UDialogue* InDialogue, 
	TMap<FName, UDialogueSpeakerComponent*> InSpeakers, bool bResume)
{	 
	if (!InDialogue)
	{
		UE_LOG(
			LogDialogueTree, 
			Warning, 
			TEXT("Speaker: No valid dialogue found to start")
		);
		return;
	}

	//Validate the controller
	if (!DialogueController)
	{
		UE_LOG(
			LogDialogueTree, 
			Error, 
			TEXT("Speaker could not start dialogue because dialogue controller was invalid")
		);
		return;
	}

	//Start the dialogue 
	DialogueController->StartDialogueWithNames(InDialogue, InSpeakers);
}

void UDialogueSpeakerComponent::StartDialogue(UDialogue* InDialogue,
	TArray<UDialogueSpeakerComponent*> InSpeakers, bool bResume)
{
	//Validate that a dialogue was provided 
	if (!InDialogue)
	{
		UE_LOG(
			LogDialogueTree,
			Warning,
			TEXT("Speaker: No valid dialogue found to start")
		);
		return;
	}

	//Validate the controller
	if (!DialogueController)
	{
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Speaker could not start dialogue because dialogue controller was invalid")
		);
		return;
	}

	//Start the dialogue 
	if (InSpeakers.Contains(this) == false)
	{
		InSpeakers.Add(this);
	}

	DialogueController->StartDialogue(InDialogue, InSpeakers, bResume);
}

void UDialogueSpeakerComponent::StartOwnedDialogueWithNamesAt(FName InNodeID, TMap<FName, UDialogueSpeakerComponent*> InSpeakers)
{
	if (OwnedDialogue)
	{
		StartDialogueWithNamesAt(OwnedDialogue, InNodeID, InSpeakers);
	}
}

void UDialogueSpeakerComponent::StartOwnedDialogueAt(FName InNodeID, TArray<UDialogueSpeakerComponent*> InSpeakers)
{
	if (OwnedDialogue)
	{
		StartDialogueAt(OwnedDialogue, InNodeID, InSpeakers);
	}
}

void UDialogueSpeakerComponent::StartDialogueWithNamesAt(UDialogue* InDialogue, FName InNodeID, TMap<FName, UDialogueSpeakerComponent*> InSpeakers)
{
	if (!InDialogue)
	{
		UE_LOG(
			LogDialogueTree,
			Warning,
			TEXT("Speaker: No valid dialogue found to start")
		);
		return;
	}

	//Validate the controller
	if (!DialogueController)
	{
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Speaker could not start dialogue because dialogue controller was invalid")
		);
		return;
	}

	//Start the dialogue 
	DialogueController->StartDialogueWithNamesAt(
		InDialogue, 
		InNodeID, 
		InSpeakers
	);
}

void UDialogueSpeakerComponent::StartDialogueAt(UDialogue* InDialogue, FName InNodeID, TArray<UDialogueSpeakerComponent*> InSpeakers)
{
	//Validate that a dialogue was provided 
	if (!InDialogue)
	{
		UE_LOG(
			LogDialogueTree,
			Warning,
			TEXT("Speaker: No valid dialogue found to start")
		);
		return;
	}

	//Validate the controller
	if (!DialogueController)
	{
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Speaker could not start dialogue because dialogue controller was invalid")
		);
		return;
	}

	//Start the dialogue 
	if (InSpeakers.Contains(this) == false)
	{
		InSpeakers.Add(this);
	}

	DialogueController->StartDialogueAt(InDialogue, InNodeID, InSpeakers);
}

FSpeakerActorEntry UDialogueSpeakerComponent::ToSpeakerActorEntry()
{
	FSpeakerActorEntry Entry;
	Entry.SpeakerComponent = this; 
	Entry.Actor = GetOwner();

	return Entry;
}

void UDialogueSpeakerComponent::BroadcastSpeechSkipped(
	FSpeechDetails SkippedSpeech)
{
	OnSpeechSkipped.Broadcast(SkippedSpeech);
}

void UDialogueSpeakerComponent::BroadcastCurrentGameplayTags()
{
	OnGameplayTagsChanged.Broadcast(GameplayTags);
}
