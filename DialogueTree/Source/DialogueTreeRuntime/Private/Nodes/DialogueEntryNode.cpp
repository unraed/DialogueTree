// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Nodes/DialogueEntryNode.h"
//Plugin
#include "Dialogue.h"
#include "LogDialogueTree.h"

void UDialogueEntryNode::EnterNode()
{
	check(Dialogue);

	//Call super
	Super::EnterNode();

	//If no children, end dialogue and throw error
	if (Children.Num() < 1 || Children[0] == nullptr)
	{
		UE_LOG(
			LogDialogueTree, 
			Warning, 
			TEXT("Exiting dialogue: Entry node has no children...")
		);
		Dialogue->EndDialogue();
	}

	//Otherwise, get first (only) child and enter that node 
	Dialogue->TraverseNode(Children[0]);
}
