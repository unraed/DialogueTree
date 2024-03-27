// Copyright Zachary Brett, 2024. All rights reserved.

#include "DialogueNodeSocket.h"

#if WITH_EDITOR
void UDialogueNodeSocket::SetGraphNode(UEdGraphNode* InGraphNode)
{
    GraphNode = InGraphNode;
}

UEdGraphNode* UDialogueNodeSocket::GetGraphNode()
{
    return GraphNode;
}
#endif 

void UDialogueNodeSocket::SetDisplayID(FText InText)
{
    DisplayID = InText;
}

FText UDialogueNodeSocket::GetDisplayID()
{
    return DisplayID;
}

void UDialogueNodeSocket::SetDialogueNode(UDialogueNode* InNode)
{
    DialogueNode = InNode;
}

UDialogueNode* UDialogueNodeSocket::GetDialogueNode()
{
    return DialogueNode;
}
