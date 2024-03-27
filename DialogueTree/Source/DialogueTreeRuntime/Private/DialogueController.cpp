// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "DialogueController.h"
//Plugin
#include "Dialogue.h"
#include "DialogueSpeakerComponent.h"
#include "LogDialogueTree.h"
//Engine
#include "GameFramework/Actor.h"
#include "UObject/UObjectIterator.h"


// Sets default values
ADialogueController::ADialogueController()
{
	PrimaryActorTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA
	bIsSpatiallyLoaded = false;
#endif
}

void ADialogueController::SelectOption(int32 InOptionIndex) const
{
	CurrentDialogue->SelectOption(InOptionIndex);
}

TMap<FName, UDialogueSpeakerComponent*> ADialogueController::GetSpeakers() const
{
	if (CurrentDialogue)
	{
		return CurrentDialogue->GetAllSpeakers();
	}

	return TMap<FName, UDialogueSpeakerComponent*>();
}

void ADialogueController::StartDialogueWithNames(UDialogue* InDialogue, 
	TMap<FName, UDialogueSpeakerComponent*> InSpeakers)
{
	if (!InDialogue)
	{
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Could not start dialogue. Provided dialogue null.")
		);
		return;
	}

	if (!CanOpenDisplay())
	{
		return;
	}

	CurrentDialogue = InDialogue;

	OpenDisplay();
	CurrentDialogue->OpenDialogue(this, InSpeakers);
	OnDialogueStarted.Broadcast();
}

void ADialogueController::StartDialogue(UDialogue* InDialogue, 
	TArray<UDialogueSpeakerComponent*> InSpeakers)
{
	if (InSpeakers.IsEmpty())
	{
		UE_LOG(
			LogDialogueTree,
			Warning,
			TEXT("No speakers provided on dialogue start.")
		);
		return;
	}

	TMap<FName, UDialogueSpeakerComponent*> TargetSpeakers;
	for (UDialogueSpeakerComponent* Speaker : InSpeakers)
	{
		if (Speaker == nullptr)
		{
			UE_LOG(
				LogDialogueTree,
				Error,
				TEXT("Could not start dialogue. Invalid speaker provided.")
			);
			return;
		}

		if (Speaker->GetDialogueName().IsNone())
		{
			UE_LOG(
				LogDialogueTree,
				Error,
				TEXT("Could not start dialogue. A provided speaker has an" 
					" unfilled DialogueName.")
			);
			return;
		}

		if (TargetSpeakers.Contains(Speaker->GetDialogueName()))
		{
			UE_LOG(
				LogDialogueTree,
				Error,
				TEXT("Could not start dialogue. Multiple provided speakers "
					"share a DialogueName.")
			);
			return;
		}

		TargetSpeakers.Add(Speaker->GetDialogueName(), Speaker);
	}

	StartDialogueWithNames(InDialogue, TargetSpeakers);
}

void ADialogueController::EndDialogue()
{
	CloseDisplay();
	OnDialogueEnded.Broadcast();

	if (CurrentDialogue)
	{
		//Clear any behavior flags from the speakers and stop speaking
		for (auto& Entry : CurrentDialogue->GetAllSpeakers())
		{
			if (Entry.Value)
			{
				Entry.Value->Stop();
				Entry.Value->ClearBehaviorFlags();
			}
		}

		CurrentDialogue->ClearController();
		CurrentDialogue = nullptr;
	}
}

void ADialogueController::Skip() const
{
	if (CurrentDialogue)
	{
		CurrentDialogue->Skip();
	}
}

void ADialogueController::ClearNodeVisits()
{
	if (CurrentDialogue)
	{
		ClearAllNodeVisitsForDialogue(CurrentDialogue);
	}
}

void ADialogueController::SetSpeaker(FName InName, 
	UDialogueSpeakerComponent* InSpeaker)
{
	if (CurrentDialogue && InSpeaker)
	{
		CurrentDialogue->SetSpeaker(InName, InSpeaker);
	}
}

FDialogueRecords ADialogueController::GetDialogueRecords() const
{
	return DialogueRecords;
}

void ADialogueController::ClearDialogueRecords()
{
	DialogueRecords.Records.Empty();
}

void ADialogueController::ImportDialogueRecords(FDialogueRecords InRecords)
{
	DialogueRecords = InRecords;
}

bool ADialogueController::SpeakerInCurrentDialogue(UDialogueSpeakerComponent* TargetSpeaker) const
{
	//If no active dialogue, then automatically false
	if (!CurrentDialogue)
	{
		return false;
	}

	//Retrieve speakers
	TMap<FName, UDialogueSpeakerComponent*> Speakers =
		CurrentDialogue->GetAllSpeakers();
	TArray<FName> SpeakerRoles;
	Speakers.GetKeys(SpeakerRoles);

	//If any one speaker matches the target, true
	for (FName NextRole : SpeakerRoles)
	{
		if (Speakers[NextRole] == TargetSpeaker)
		{
			return true;
		}
	}

	//No speakers matched the target, false
	return false;
}

void ADialogueController::MarkNodeVisited(UDialogue* TargetDialogue, int32 TargetNodeIndex)
{
	if (!TargetDialogue)
	{
		return;
	}

	FName TargetDialogueName = TargetDialogue->GetFName();

	if (TargetDialogueName.IsEqual(NAME_None))
	{
		return;
	}

	//Create a new record if the target record does not exist
	if (!DialogueRecords.Records.Contains(TargetDialogueName))
	{
		FDialogueNodeVisits NewRecord;
		NewRecord.DialogueFName = TargetDialogueName;

		DialogueRecords.Records.Add(TargetDialogueName, NewRecord);
	}

	//Mark the node visited in the record 
	DialogueRecords.Records[TargetDialogueName].VisitedNodeIndices.Add(
		TargetNodeIndex
	);
}

void ADialogueController::MarkNodeUnvisited(UDialogue* TargetDialogue, int32 TargetNodeIndex)
{
	if (!TargetDialogue)
	{
		return;
	}

	FName TargetDialogueName = TargetDialogue->GetFName();

	//If there is no record of that dialogue, do nothing
	if (!DialogueRecords.Records.Contains(TargetDialogueName))
	{
		return;
	}

	//If there is a record, remove the target index from the visited nodes
	DialogueRecords.Records[TargetDialogueName].VisitedNodeIndices.Remove(
		TargetNodeIndex
	);
}

void ADialogueController::ClearAllNodeVisitsForDialogue(UDialogue* TargetDialogue)
{
	if (!TargetDialogue)
	{
		return;
	}

	FName TargetDialogueName = TargetDialogue->GetFName();

	//If there is no record of that dialogue, do nothing
	if (!DialogueRecords.Records.Contains(TargetDialogueName))
	{
		return;
	}

	DialogueRecords.Records[TargetDialogueName].VisitedNodeIndices.Empty();
}

bool ADialogueController::WasNodeVisited(const UDialogue* TargetDialogue, 
	int32 TargetNodeIndex) const
{
	if (!TargetDialogue)
	{
		return false;
	}

	FName TargetDialogueName = TargetDialogue->GetFName();

	if (!DialogueRecords.Records.Contains(TargetDialogueName))
	{
		return false;
	}

	FDialogueNodeVisits TargetRecord = 
		DialogueRecords.Records[TargetDialogueName];

	return TargetRecord.VisitedNodeIndices.Contains(TargetNodeIndex);
}
