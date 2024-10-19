// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Dialogue.h"
//UE
#include "EdGraph/EdGraph.h"
#include "Kismet/GameplayStatics.h"
//Plugin
#include "DialogueController.h"
#include "DialogueSpeakerComponent.h"
#include "DialogueSpeakerSocket.h"
#include "LogDialogueTree.h"
#include "Nodes/DialogueEntryNode.h"

FColor FDefaultDialogueColors::PopColor()
{
	FColor TargetColor = Colors[ColorIndex];
	++ColorIndex;
	ColorIndex = ColorIndex % Colors.Num();

	return TargetColor;
}

UDialogue::UDialogue()
{
	//Add default speakers
	AddDefaultSpeakers();
}

#if WITH_EDITOR

void UDialogue::PostEditChangeProperty(
	FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.GetMemberPropertyName();

	//If editing the speaker roles
	if (PropertyName.IsEqual("SpeakerRoles"))
	{
		OnChangeSpeakers(PropertyChangedEvent.ChangeType);
	}
}

#endif

void UDialogue::SetSpeaker(FName InName, UDialogueSpeakerComponent* InSpeaker)
{
	if (Speakers.Contains(InName))
	{
		Speakers[InName] = InSpeaker;
	}
}

UDialogueSpeakerComponent* UDialogue::GetSpeaker(FName InName) const
{
	if (Speakers.Contains(InName))
	{
		return Speakers[InName];
	}

	return nullptr;
}

void UDialogue::AddSpeakerEntry(FName InName)
{
	if (!Speakers.Contains(InName))
	{
		Speakers.Add(InName, nullptr);
	}
}

void UDialogue::OpenDialogueAt(FName InNodeID, ADialogueController* InController, TMap<FName, UDialogueSpeakerComponent*> InSpeakers)
{
	//Make sure we can start the dialogue 
	FString ErrorMessage;
	if (!CanPlay(InController, ErrorMessage))
	{
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Cannot play dialogue. %s"),
			*ErrorMessage
		);
		return;
	}

	if (!HasNode(InNodeID))
	{
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Cannot play dialogue starting from Node %s. No such node exists."),
			*InNodeID.ToString()
		);
		return;
	}

	//Store the controller 
	DialogueController = InController;

	//Fill the speakers with the provided values 
	FillSpeakers(InSpeakers);

	//Traverse the first node 
	UDialogueNode* StartNode = DialogueNodes[InNodeID];
	check(StartNode);
	TraverseNode(StartNode);
}

void UDialogue::ClearController()
{
	DialogueController = nullptr;
}

void UDialogue::EndDialogue() const
{
	//End the dialogue
	if (DialogueController)
	{
		DialogueController->EndDialogue();
	}
}

void UDialogue::DisplaySpeech(const FSpeechDetails& InDetails) const
{
	if (!Speakers[InDetails.SpeakerName] || !DialogueController)
	{
		EndDialogue();
		return;
	}

	DialogueController->DisplaySpeech(
		InDetails,
		Speakers[InDetails.SpeakerName]
	);
	DialogueController->OnDialogueSpeechDisplayed.Broadcast(InDetails);
}

void UDialogue::DisplayOptions(TArray<FDialogueOption> InOptions) const
{
	if (!DialogueController)
	{
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Attempting to display options via missing dialogue controller. Aborting dialogue.")
		);
		return;
	}

	TArray<FSpeechDetails> AllDetails;
	for (FDialogueOption Option : InOptions)
	{
		AllDetails.Add(Option.Details);
	}

	DialogueController->DisplayOptions(AllDetails);
}

void UDialogue::SelectOption(int32 InOptionIndex) const
{
	if (ActiveNode)
	{
		ActiveNode->SelectOption(InOptionIndex);
	}
}

void UDialogue::Skip() const
{
	if (ActiveNode)
	{
		ActiveNode->Skip();
	}
}

void UDialogue::TraverseNode(UDialogueNode* InNode)
{
	//return if the dialogue is already closed
	if (!DialogueController)
	{
		return;
	}

	//If no node provided, end the dialogue
	if (!InNode)
	{
		EndDialogue();
		return;
	}

	//Mark the node visited 
	DialogueController->MarkNodeVisited(this, InNode->GetNodeID());

	//Traverse the target node 
	ActiveNode = InNode;
	ActiveNode->EnterNode();
}

EDialogueCompileStatus UDialogue::GetCompileStatus() const
{
	return CompileStatus;
}

TMap<FName, UDialogueSpeakerComponent*> UDialogue::GetAllSpeakers() const
{
	//Have to manually copy to handle conversion from TObjectPtr
	TMap<FName, UDialogueSpeakerComponent*> AllSpeakers;
	for (auto& Speaker : Speakers)
	{
		AllSpeakers.Add(Speaker.Key, Speaker.Value);
	}
	return AllSpeakers;
}

bool UDialogue::SpeakerIsPresent(const FName SpeakerName) const
{
	return GetSpeaker(SpeakerName) != nullptr;
}

bool UDialogue::WasNodeVisited(UDialogueNode* TargetNode) const
{
	if (!DialogueController || !TargetNode
		|| !DialogueNodes.Contains(TargetNode->GetNodeID()))
	{
		return false;
	}

	return DialogueController->WasNodeVisited(
		this,
		TargetNode->GetNodeID()
	);
}

void UDialogue::MarkNodeVisited(UDialogueNode* TargetNode, bool bVisited)
{
	if (!DialogueController || !TargetNode)
	{
		return;
	}

	if (bVisited)
	{
		DialogueController->MarkNodeVisited(
			this,
			TargetNode->GetNodeID()
		);
	}
	else
	{
		DialogueController->MarkNodeUnvisited(
			this,
			TargetNode->GetNodeID()
		);
	}
}

void UDialogue::ClearAllNodeVisits()
{
	if (!DialogueController)
	{
		return;
	}

	DialogueController->ClearAllNodeVisitsForDialogue(this);
}

bool UDialogue::HasNode(FName NodeID) const
{
	return DialogueNodes.Contains(NodeID);
}

void UDialogue::SetResumeNode(UDialogueNode* InNode)
{
	if (InNode && DialogueNodes.Contains(InNode->GetNodeID())
		&& DialogueController)
	{
		DialogueController->SetResumeNode(this, InNode->GetNodeID());
	}
}

UDialogueNode* UDialogue::GetRootNode() const
{
	return RootNode;
}

#if WITH_EDITOR

UEdGraph* UDialogue::GetEdGraph() const
{
	return EdGraph;
}

void UDialogue::SetEdGraph(UEdGraph* InEdGraph)
{
	EdGraph = InEdGraph;
}

const TMap<FName, FSpeakerField>& UDialogue::GetSpeakerRoles() const
{
	return SpeakerRoles;
}

void UDialogue::AddNode(UDialogueNode* InNode)
{
	if (InNode)
	{
		DialogueNodes.Add(InNode->GetNodeID(), InNode);
		InNode->SetDialogue(this);
	}
}

void UDialogue::RemoveNode(UDialogueNode* InNode)
{
	if (InNode)
	{
		DialogueNodes.Remove(InNode->GetNodeID());
	}
}

void UDialogue::SetRootNode(UDialogueNode* InNode)
{
	if (UDialogueEntryNode* EntryNode
		= Cast<UDialogueEntryNode>(InNode))
	{
		RootNode = EntryNode;
	}
}

void UDialogue::ClearDialogue()
{
	RootNode = nullptr;
	DialogueNodes.Empty();
	Speakers.Empty();
	CompileStatus = EDialogueCompileStatus::Uncompiled;
}

void UDialogue::PreCompileDialogue()
{
	ClearDialogue();

	//Refresh the accessible speaker component entries from their roles
	for (auto& Entry : SpeakerRoles)
	{
		AddSpeakerEntry(Entry.Key);
	}
}

void UDialogue::SetCompileStatus(EDialogueCompileStatus InStatus)
{
	CompileStatus = InStatus;
	MarkPackageDirty(); //need to save
}
#endif

void UDialogue::AddDefaultSpeakers()
{
	//Default NPC
	UDialogueSpeakerSocket* NPCSpeaker =
		CreateDefaultSubobject<UDialogueSpeakerSocket>(
			"DefaultSpeakerNPC"
		);
	NPCSpeaker->SetSpeakerName("NPC");
	FSpeakerField NPCField;
	NPCField.GraphColor = DefaultSpeakerColors.PopColor();
	NPCField.SpeakerSocket = NPCSpeaker;

	SpeakerRoles.Add(NPCSpeaker->GetSpeakerName(), NPCField);

	//Default player
	UDialogueSpeakerSocket* PlayerSpeaker =
		CreateDefaultSubobject<UDialogueSpeakerSocket>(
			"DefaultSpeakerPlayer"
		);
	PlayerSpeaker->SetSpeakerName("Player");
	FSpeakerField PlayerField;
	PlayerField.GraphColor = DefaultSpeakerColors.PopColor();
	PlayerField.SpeakerSocket = PlayerSpeaker;

	SpeakerRoles.Add(PlayerSpeaker->GetSpeakerName(), PlayerField);
}

void UDialogue::OnChangeSpeakers(const EPropertyChangeType::Type& ChangeType)
{
	if (ChangeType == EPropertyChangeType::ArrayAdd)
	{
		OnAddSpeaker();
	}
	else if (ChangeType == EPropertyChangeType::ArrayRemove)
	{
		OnRemoveSpeaker();
	}
	else
	{
		OnChangeSingleSpeaker();
	}

	OnSpeakerRolesChanged.ExecuteIfBound();
}

void UDialogue::OnAddSpeaker()
{
	for (auto& Entry : SpeakerRoles)
	{
		FSpeakerField& Value = Entry.Value;

		//If entry has yet to be filled
		if (!Value.SpeakerSocket)
		{
			//Create a new socket for the entry
			Value.SpeakerSocket =
				NewObject<UDialogueSpeakerSocket>(this);

			//In case the name is not unfilled for whatever reason
			Value.SpeakerSocket->SetSpeakerName(Entry.Key);

			//Add the default color for the entry 
			Value.GraphColor = DefaultSpeakerColors.PopColor();
		}
	}
}

void UDialogue::OnRemoveSpeaker()
{
	//Stub used later if necessary
}

void UDialogue::OnChangeSingleSpeaker()
{
	//Check for any speakers where the socket doesn't match the name
	for (auto& Entry : SpeakerRoles)
	{
		FSpeakerField& Value = Entry.Value;

		if (Value.SpeakerSocket->GetSpeakerName() != Entry.Key)
		{
			Value.SpeakerSocket->SetSpeakerName(Entry.Key);
		}
	}
}

bool UDialogue::CanPlay(ADialogueController* InController,
	FString& OutErrorMessage) const
{
	if (CompileStatus != EDialogueCompileStatus::Compiled)
	{
		OutErrorMessage = "Dialogue is not compiled.";
		return false;
	}
	if (!InController)
	{
		OutErrorMessage = "No valid controller provided.";
		return false;
	}
	if (!RootNode)
	{
		OutErrorMessage = "Entry node does not exist.";
		return false;
	}

	return true;
}

void UDialogue::FillSpeakers(TMap<FName, UDialogueSpeakerComponent*> InSpeakers)
{
	//Clear speakers 
	for (auto& Entry : Speakers)
	{
		Entry.Value = nullptr;
	}

	//Set speakers
	for (auto& Entry : InSpeakers)
	{
		if (Speakers.Contains(Entry.Key) &&
			Entry.Value != nullptr)
		{
			Speakers[Entry.Key] = Entry.Value;
		}
	}

	//Verify that no speakers are missing 
	for (auto& Entry : Speakers)
	{
		if (!Entry.Value)
		{
			DialogueController->HandleMissingSpeaker(Entry.Key);
		}
	}
}
