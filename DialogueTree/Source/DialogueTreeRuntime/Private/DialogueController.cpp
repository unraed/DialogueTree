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
	if (CurrentDialogue)
	{
		CurrentDialogue->SelectOption(InOptionIndex);
	}
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
	TMap<FName, UDialogueSpeakerComponent*> InSpeakers, bool bResume)
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
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Could not start dialogue. CanOpenDisplay() was false.")
		);
		return;
	}

	//Set the target dialogue 
	CurrentDialogue = InDialogue;

	//Get start node 
	FName StartNodeID = CurrentDialogue->GetRootNode()->GetNodeID();
	FName RecordName = CurrentDialogue->GetFName();
	if (bResume
		&& DialogueRecords.Records.Contains(RecordName))
	{
		const FDialogueNodeVisits& Record =
			DialogueRecords.Records[RecordName];
		StartNodeID = CurrentDialogue->HasNode(Record.ResumeNodeID) ?
			Record.ResumeNodeID : StartNodeID;
	}

	//Start the dialogue 
	OpenDisplay();
	CurrentDialogue->OpenDialogueAt(StartNodeID, this, InSpeakers);
	OnDialogueStarted.Broadcast();
}

void ADialogueController::StartDialogue(UDialogue* InDialogue,
	TArray<UDialogueSpeakerComponent*> InSpeakers, bool bResume)
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

	StartDialogueWithNames(InDialogue, TargetSpeakers, bResume);
}

void ADialogueController::StartDialogueWithNamesAt(UDialogue* InDialogue, FName NodeID, TMap<FName, UDialogueSpeakerComponent*> InSpeakers)
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
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Could not start dialogue. Display could not be opened.")
		);
		return;
	}

	if (!InDialogue->HasNode(NodeID))
	{
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Could not start dialogue from Node: %s. No such node exists."),
			*NodeID.ToString()
		);
		return;
	}

	CurrentDialogue = InDialogue;

	OpenDisplay();
	CurrentDialogue->OpenDialogueAt(NodeID, this, InSpeakers);
	OnDialogueStarted.Broadcast();
}

void ADialogueController::StartDialogueAt(UDialogue* InDialogue, FName NodeID, TArray<UDialogueSpeakerComponent*> InSpeakers)
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

		if (!InDialogue->HasNode(NodeID))
		{
			UE_LOG(
				LogDialogueTree,
				Error,
				TEXT("Could not start dialogue from Node: %s. No such node exists."),
				*NodeID.ToString()
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

	StartDialogueWithNamesAt(InDialogue, NodeID, TargetSpeakers);
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
				Entry.Value->ClearGameplayTags();
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

void ADialogueController::MarkNodeVisited(UDialogue* TargetDialogue, FName TargetNodeID)
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
	DialogueRecords.Records[TargetDialogueName].VisitedNodeIDs.Add(
		TargetNodeID
	);
}

void ADialogueController::MarkNodeUnvisited(UDialogue* TargetDialogue, FName TargetNodeID)
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
	DialogueRecords.Records[TargetDialogueName].VisitedNodeIDs.Remove(
		TargetNodeID
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

	DialogueRecords.Records[TargetDialogueName].VisitedNodeIDs.Empty();
}

bool ADialogueController::WasNodeVisited(const UDialogue* TargetDialogue,
	FName TargetNodeID) const
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

	return TargetRecord.VisitedNodeIDs.Contains(TargetNodeID);
}

void ADialogueController::SetResumeNode(UDialogue* InDialogue, FName InNodeID)
{
	if (!InDialogue || InNodeID.IsNone())
	{
		return;
	}

	//Ensure there is a record
	FName RecordName = InDialogue->GetFName();
	if (!DialogueRecords.Records.Contains(RecordName))
	{
		DialogueRecords.Records.Add(RecordName);
	}

	//Set the record's resume node 
	DialogueRecords.Records[RecordName].ResumeNodeID = InNodeID;
}
