// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Nodes/DialogueOptionLockNode.h"
//Plugin
#include "Conditionals/DialogueCondition.h"
#include "Dialogue.h"

FDialogueOption UDialogueOptionLockNode::GetAsOption()
{
	if (Children.Num() < 1 || Children[0] == nullptr)
	{
		return FDialogueOption();
	}

	FDialogueOption Option = Children[0]->GetAsOption();

	if (!PassesConditions())
	{
		Option.Details.bIsLocked = true;
		Option.Details.OptionMessage = LockedMessage;
	}
	else
	{
		Option.Details.bIsLocked = false;
		Option.Details.OptionMessage = UnlockedMessage;
	}

	return Option;
}

void UDialogueOptionLockNode::EnterNode()
{
	check(Dialogue);

	//Call super
	Super::EnterNode();

	//If no children, end dialogue 
	if (Children.Num() < 1 || Children[0] == nullptr)
	{
		Dialogue->EndDialogue();
	}

	//Otherwise, get first (only) child and enter that node 
	Dialogue->TraverseNode(Children[0]);
}

void UDialogueOptionLockNode::InitLockNodeData(bool InIfAny, 
	TArray<UDialogueCondition*>& InConditions, const FText& LockedText, 
	const FText& UnlockedText)
{
	bIfAny = InIfAny;
	LockedMessage = LockedText;
	UnlockedMessage = UnlockedText;

	Conditions.Empty();
	for (UDialogueCondition* Condition : InConditions)
	{
		Conditions.Add(Condition);
	}
}

bool UDialogueOptionLockNode::PassesConditions() const
{
	if (bIfAny)
	{
		return AnyConditionsTrue();
	}

	return AllConditionsTrue();
}

bool UDialogueOptionLockNode::AnyConditionsTrue() const
{
	for (UDialogueCondition* Condition : Conditions)
	{
		if (Condition->IsMet())
		{
			return true;
		}
	}

	return false;
}

bool UDialogueOptionLockNode::AllConditionsTrue() const
{
	for (UDialogueCondition* Condition : Conditions)
	{
		if (!Condition->IsMet())
		{
			return false;
		}
	}

	return true;
}
