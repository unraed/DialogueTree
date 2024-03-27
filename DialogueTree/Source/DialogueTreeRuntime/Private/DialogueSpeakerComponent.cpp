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

FGameplayTagContainer UDialogueSpeakerComponent::GetBehaviorFlags()
{
	return BehaviorFlags;
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

void UDialogueSpeakerComponent::SetBehaviorFlags(FGameplayTagContainer InFlags)
{
	BehaviorFlags.Reset();
	BehaviorFlags.AppendTags(InFlags);
	BroadcastBehaviorFlags();
}

void UDialogueSpeakerComponent::ClearBehaviorFlags()
{
	BehaviorFlags.Reset();
	BroadcastBehaviorFlags();
}

void UDialogueSpeakerComponent::StartOwnedDialogueWithNames(
	TMap<FName, UDialogueSpeakerComponent*> InSpeakers)
{
	if (OwnedDialogue)
	{
		StartDialogueWithNames(OwnedDialogue, InSpeakers);
	}
}

void UDialogueSpeakerComponent::StartOwnedDialogue(
	TArray<UDialogueSpeakerComponent*> InSpeakers)
{
	if (OwnedDialogue)
	{
		StartDialogue(OwnedDialogue, InSpeakers);
	}
}

void UDialogueSpeakerComponent::StartDialogueWithNames(UDialogue* InDialogue, 
	TMap<FName, UDialogueSpeakerComponent*> InSpeakers)
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
	TArray<UDialogueSpeakerComponent*> InSpeakers)
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

	DialogueController->StartDialogue(InDialogue, InSpeakers);
}

FSpeakerActorEntry UDialogueSpeakerComponent::ToSpeakerActorEntry() 
{
	FSpeakerActorEntry Entry;
	Entry.SpeakerComponent = this; 
	Entry.Actor = GetOwner();

	return Entry;
}

void UDialogueSpeakerComponent::BroadcastBehaviorFlags()
{
	OnBehaviorFlagsChanged.Broadcast(BehaviorFlags);
}
