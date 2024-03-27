// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Nodes/DialogueNode.h"
//Plugin
#include "Dialogue.h"

UDialogue* UDialogueNode::GetDialogue() const
{
    return Dialogue;
}

void UDialogueNode::SetDialogue(UDialogue* InDialogue)
{
    Dialogue = InDialogue;
}

void UDialogueNode::AddParent(UDialogueNode* InParent)
{
    if (!Parents.Contains(InParent))
    {
        Parents.Add(InParent);
    }
}

void UDialogueNode::AddChild(UDialogueNode* InChild)
{
    if (!Children.Contains(InChild))
    {
        Children.Add(InChild);
    }
}

TArray<UDialogueNode*> UDialogueNode::GetParents() const
{
    return Parents;
}

TArray<UDialogueNode*> UDialogueNode::GetChildren() const
{
    return Children;
}

FDialogueOption UDialogueNode::GetAsOption()
{
    return FDialogueOption();
}

int32 UDialogueNode::GetNodeIndex() const
{
    return NodeIndex;
}

void UDialogueNode::SetNodeIndex(int32 InIndex)
{
    NodeIndex = InIndex;
}
