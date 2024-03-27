// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Conditionals/Queries/NodeVisitedQuery.h"
//Plugin
#include "Dialogue.h"
#include "DialogueNodeSocket.h"
#include "LogDialogueTree.h"

#define LOCTEXT_NAMESPACE "NodeVisitedQuery"

bool UNodeVisitedQuery::ExecuteQuery()
{
	if (!TargetNode->GetDialogueNode()) //Should not be possible, close dialogue
	{
		UE_LOG(
			LogDialogueTree, 
			Error, 
			TEXT("Closing dialogue: Attempted to execute a Node Visited Query "
				"on a nullptr")
		);
		
		GetDialogue()->EndDialogue();
		return false;
	}

	return GetDialogue()->WasNodeVisited(TargetNode->GetDialogueNode());
}

FText UNodeVisitedQuery::GetGraphDescription_Implementation() const
{
	//Get the node's ID 
	if (!TargetNode)
	{
		return LOCTEXT("Invalid node", "Invalid Node");
	}
	FText NodeID = TargetNode->GetDisplayID();

	//Return the display text
	FText BaseText =
		LOCTEXT("BaseDisplayText", "{0} was visited");
	return FText::Format(
		BaseText,
		NodeID
	);
}

bool UNodeVisitedQuery::IsValidQuery() const
{
	bool bGraphNode = true;

#if WITH_EDITOR
	if (TargetNode)
	{
		bGraphNode = TargetNode->GetGraphNode() != nullptr;
	}
#endif 

	return TargetNode && bGraphNode;
}

void UNodeVisitedQuery::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);

	check(GetDialogue());

	UDialogueNodeSocket* CopiedSocket = 
		DuplicateObject(TargetNode, GetDialogue());
	TargetNode = CopiedSocket;
}

UDialogueNodeSocket* UNodeVisitedQuery::GetSocket()
{
	return TargetNode;
}

#undef LOCTEXT_NAMESPACE