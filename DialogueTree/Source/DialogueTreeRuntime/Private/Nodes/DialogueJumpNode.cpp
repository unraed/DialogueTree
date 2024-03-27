// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Nodes/DialogueJumpNode.h"
//Plugin
#include "Dialogue.h"

void UDialogueJumpNode::EnterNode()
{
	Super::EnterNode();
	Dialogue->TraverseNode(JumpTarget);
}

FDialogueOption UDialogueJumpNode::GetAsOption()
{
	if (JumpTarget)
	{
		FSpeechDetails OptionDetails = 
			JumpTarget->GetAsOption().Details;
		return FDialogueOption{ OptionDetails, this };
	}

	return FDialogueOption();
}

void UDialogueJumpNode::SetJumpTarget(UDialogueNode* InTarget)
{
	check(InTarget);
	JumpTarget = InTarget;
}
