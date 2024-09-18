// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Events/SetResumeNode.h"
//Plugin
#include "Dialogue.h"
#include "DialogueNodeSocket.h"

#define LOCTEXT_NAMESPACE "SetResumeNode"

void USetResumeNode::PlayEvent()
{
	if (!Dialogue || !TargetNode)
	{
		return;
	}

	UDialogueNode* ResumeNode = TargetNode->GetDialogueNode();
	if (!ResumeNode)
	{
		return;
	}

	Dialogue->SetResumeNode(ResumeNode);
}

bool USetResumeNode::HasAllRequirements() const
{
#if WITH_EDITOR
	return TargetNode && TargetNode->GetGraphNode();
#else
	return TargetNode != nullptr;
#endif 
}

FText USetResumeNode::GetGraphDescription_Implementation() const
{
#if WITH_EDITOR
	if (!TargetNode || !TargetNode->GetGraphNode())
	{
		return LOCTEXT("DefaultText", "Invalid Event");
	}

	FText BaseText = LOCTEXT("DescriptionText", "Set resume node: {0}");
	FText NodeText = TargetNode->GetDisplayID();
	return FText::Format(BaseText, NodeText);
#else
	return FText();
#endif 
}

#undef LOCTEXT_NAMESPACE