// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Nodes/DialogueBranchNode.h"
//Plugin
#include "Conditionals/DialogueCondition.h"
#include "Dialogue.h"

FDialogueOption UDialogueBranchNode::GetAsOption()
{
    if (PassesConditions() && TrueNode)
    {
        FSpeechDetails OptionDetails = TrueNode->GetAsOption().Details;
        return FDialogueOption{ OptionDetails, this };
    }
    else if (FalseNode)
    {
        FSpeechDetails OptionDetails = FalseNode->GetAsOption().Details;
        return FDialogueOption{ OptionDetails, this };
    }

    return FDialogueOption();
}

void UDialogueBranchNode::EnterNode()
{
    //Call super
    Super::EnterNode();

    //Determine the correct next node based on conditions
    UDialogueNode* NextNode;
    if (PassesConditions())
    {
        NextNode = TrueNode;
    }
    else
    {
        NextNode = FalseNode;
    }

    //If next node is nullptr, exit the dialogue
    if (!NextNode)
    {
        GetDialogue()->EndDialogue();
    }

    //Next node found, transition to it 
    GetDialogue()->TraverseNode(NextNode);
}

void UDialogueBranchNode::InitBranchData(bool InIfAny, 
    UDialogueNode* InTrueNode, UDialogueNode* InFalseNode, 
    TArray<UDialogueCondition*>& InConditions)
{
    bIfAny = InIfAny;
    TrueNode = InTrueNode;
    FalseNode = InFalseNode;

    Conditions.Empty();
    for (UDialogueCondition* Condition : InConditions)
    {
        Conditions.Add(Condition);
    }
}

void UDialogueBranchNode::ClearConditions()
{
    Conditions.Empty();
}

bool UDialogueBranchNode::PassesConditions() const
{
    if (bIfAny)
    {
        return AnyConditionsTrue();
    }

    return AllConditionsTrue();
}

bool UDialogueBranchNode::AnyConditionsTrue() const
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

bool UDialogueBranchNode::AllConditionsTrue() const
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
