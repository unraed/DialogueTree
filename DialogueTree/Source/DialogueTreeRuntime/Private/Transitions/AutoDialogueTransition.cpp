// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Transitions/AutoDialogueTransition.h"
//Plugin
#include "Dialogue.h"
#include "DialogueSpeakerComponent.h"
#include "Nodes/DialogueSpeechNode.h"

FText UAutoDialogueTransition::GetDisplayName() const
{
	return FText::FromString("AutoTransition");
}

FText UAutoDialogueTransition::GetNodeCreationTooltip() const
{
	return FText::FromString("Create speech node that automatically transitions"
		" to the first viable option");
}

void UAutoDialogueTransition::TransitionOut()
{
	//Transition to the first linked node 
	TArray<UDialogueNode*> Children = OwningNode->GetChildren();
	if (!Children.IsEmpty() && Children[0])
	{
		OwningNode->GetDialogue()->TraverseNode(Children[0]);
	}
	else
	{
		//No child to transition to, end dialogue
		OwningNode->GetDialogue()->EndDialogue();
	}
}
