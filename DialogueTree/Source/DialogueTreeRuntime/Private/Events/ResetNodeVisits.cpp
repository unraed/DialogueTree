// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Events/ResetNodeVisits.h"
//Plugin
#include "Dialogue.h"
#include "DialogueNodeSocket.h"
#include "Nodes/DialogueNode.h"

#define LOCTEXT_NAMESPACE "UResetNodeVisits"

void UResetNodeVisits::PlayEvent()
{
	if (Dialogue && TargetNode && TargetNode->GetDialogueNode())
	{
		Dialogue->MarkNodeVisited(TargetNode->GetDialogueNode(), false);
	}
}

bool UResetNodeVisits::HasAllRequirements() const
{
#if WITH_EDITOR
	return TargetNode && TargetNode->GetGraphNode();
#else
	return TargetNode != nullptr;
#endif 
}

FText UResetNodeVisits::GetGraphDescription_Implementation() const
{
#if WITH_EDITOR
	if (!TargetNode || !TargetNode->GetGraphNode())
	{
		return LOCTEXT("DefaultText", "Invalid Event");
	}

	FText BaseText = LOCTEXT("DescriptionText", "Mark {0} unvisited");
	FText NodeText = TargetNode->GetDisplayID();
	return FText::Format(BaseText, NodeText);
#else
	return FText();
#endif 
}

UDialogueNodeSocket* UResetNodeVisits::GetTargetSocket() const
{
	return TargetNode;
}

#undef LOCTEXT_NAMESPACE