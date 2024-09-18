// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Transitions/InputDialogueTransition.h"
//Plugin
#include "Dialogue.h"
#include "DialogueSpeakerComponent.h"
#include "Nodes/DialogueNode.h"
#include "Nodes/DialogueSpeechNode.h"
#include "LogDialogueTree.h"

#define LOCTEXT_NAMESPACE "InputDialogueTransition"

void UInputDialogueTransition::StartTransition()
{
	//Get any options
	GetOptions();

	//If the node is skippable, show options now
	if (OwningNode->GetCanSkip())
	{
		ShowOptions();
	}

	Super::StartTransition();
}

void UInputDialogueTransition::TransitionOut()
{
	Super::TransitionOut();

	//If there are no options to transition to, end dialogue
	if (Options.IsEmpty())
	{
		//If there is a child to transition to, pick it
		if (!OwningNode->GetChildren().IsEmpty())
		{
			UE_LOG(
				LogDialogueTree,
				Warning,
				TEXT("Terminating dialogue: input transition node has no valid options to select from. Valid options include speech nodes or nodes that eventually arrive at speech nodes.")
			);
		}

		OwningNode->GetDialogue()->EndDialogue();
		return;
	}
	//If the node is not skippable, display options now 
	else if (!OwningNode->GetCanSkip())
	{
		ShowOptions();
	}
}

void UInputDialogueTransition::SelectOption(int32 InOptionIndex)
{
	//End the dialogue if fed a bad index
	if (!Options.IsValidIndex(InOptionIndex))
	{
		UE_LOG(
			LogDialogueTree,
			Warning,
			TEXT("Attempted to transition to invalid option index.")
		);
		return;
	}

	//If option locked, do nothing more
	if (Options[InOptionIndex].Details.bIsLocked)
	{
		return;
	}

	//Stop playing audio 
	UDialogueSpeakerComponent* Speaker = OwningNode->GetSpeaker();
	if (Speaker)
	{
		Speaker->Stop();
	}

	//Transition to the selected node 
	UDialogueNode* Selected = Options[InOptionIndex].TargetNode;
	OwningNode->GetDialogue()->TraverseNode(Selected);
}

FText UInputDialogueTransition::GetDisplayName() const
{
	return LOCTEXT("DisplayName", "InputTransition");
}

FText UInputDialogueTransition::GetNodeCreationTooltip() const
{
	return LOCTEXT(
		"Tooltip",
		"Speech node that waits for the user to select an option"
		"before transitioning."
	);
}

EDialogueConnectionLimit UInputDialogueTransition::GetConnectionLimit() const
{
	return EDialogueConnectionLimit::Unlimited;
}

void UInputDialogueTransition::ShowOptions()
{
	//If valid options, display them 
	if (!Options.IsEmpty())
	{
		OwningNode->GetDialogue()->DisplayOptions(Options);
	}
}

void UInputDialogueTransition::GetOptions()
{
	//Retrieve all valid options 
	Options.Empty();
	TArray<UDialogueNode*> NodeChildren = OwningNode->GetChildren();

	for (UDialogueNode* Node : NodeChildren)
	{
		FDialogueOption NodeOption = Node->GetAsOption();

		//If a valid option
		if (!NodeOption.Details.SpeechText.IsEmpty() && NodeOption.TargetNode)
		{
			Options.Add(NodeOption);
		}
	}
}

#undef LOCTEXT_NAMESPACE