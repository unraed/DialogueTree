// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/DialogueTreeNodeFactory.h"
//UE
#include "SGraphNodeKnot.h"
//Plugin
#include "Graph/Nodes/GraphNodeDialogueBranch.h"
#include "Graph/Nodes/GraphNodeDialogueEntry.h"
#include "Graph/Nodes/GraphNodeDialogueEvent.h"
#include "Graph/Nodes/GraphNodeDialogueJump.h"
#include "Graph/Nodes/GraphNodeDialogueReroute.h"
#include "Graph/Nodes/GraphNodeDialogueOptionLock.h"
#include "Graph/Nodes/GraphNodeDialogueSpeech.h"
#include "Graph/Slate/SGraphNodeDialogueBase.h"
#include "Graph/Slate/SGraphNodeDialogueBranch.h"
#include "Graph/Slate/SGraphNodeDialogueEvent.h"
#include "Graph/Slate/SGraphNodeDialogueJump.h"
#include "Graph/Slate/SGraphNodeDialogueOptionLock.h"
#include "Graph/Slate/SGraphNodeDialogueSpeech.h"

TSharedPtr<class SGraphNode> FDialogueTreeNodeFactory::CreateNode(
	UEdGraphNode* Node) const
{
	//If we are dealing with an Entry Node 
	if (UGraphNodeDialogueEntry* GraphEntryNode = 
		Cast<UGraphNodeDialogueEntry> (Node))
	{
		return SNew(SGraphNodeDialogueBase, GraphEntryNode);
	}
	//If we are dealing with a Speech Node 
	else if (UGraphNodeDialogueSpeech* GraphSpeechNode =
		Cast<UGraphNodeDialogueSpeech>(Node))
	{
		return SNew(SGraphNodeDialogueSpeech, GraphSpeechNode);
	}
	//If we are dealing with a Branch Node 
	else if (UGraphNodeDialogueBranch* BranchNode =
		Cast<UGraphNodeDialogueBranch>(Node))
	{
		return SNew(SGraphNodeDialogueBranch, BranchNode);
	}
	//If we are dealing with an Event Node
	else if (UGraphNodeDialogueEvent* EventNode =
		Cast<UGraphNodeDialogueEvent>(Node))
	{
		return SNew(SGraphNodeDialogueEvent, EventNode);
	}
	//If we are dealing with a jump node
	else if (UGraphNodeDialogueJump* JumpNode =
		Cast<UGraphNodeDialogueJump>(Node))
	{
		return SNew(SGraphNodeDialogueJump, JumpNode);
	}
	//If we are dealing with an option lock node 
	else if (UGraphNodeDialogueOptionLock* LockNode =
		Cast<UGraphNodeDialogueOptionLock>(Node))
	{
		return SNew(SGraphNodeDialogueOptionLock, LockNode);
	}
	//If we are dealing with a knot type proxy node 
	else if (UGraphNodeDialogueReroute* KnotNode = 
		Cast<UGraphNodeDialogueReroute>(Node))
	{
		return SNew(SGraphNodeKnot, KnotNode);
	}

	return nullptr;
}
