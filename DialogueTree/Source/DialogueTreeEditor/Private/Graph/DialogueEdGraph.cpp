// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/DialogueEdGraph.h"
//UE
#include "GraphEditAction.h"
//Plugin
#include "Dialogue.h"
#include "DialogueSpeakerSocket.h"
#include "Graph/Nodes/GraphNodeDialogue.h"
#include "Nodes/DialogueNode.h"

UDialogueEdGraph::UDialogueEdGraph()
{
	//Setup handler for changing the graph
	AddOnGraphChangedHandler(
		FOnGraphChanged::FDelegate::CreateUObject(
			this,
			&UDialogueEdGraph::OnDialogueGraphChanged
		)
	);
}

bool UDialogueEdGraph::Modify(bool bAlwaysMarkDirty)
{
	bool ModifyReturnValue = Super::Modify(bAlwaysMarkDirty);
	GetDialogue()->Modify();

	for (UEdGraphNode* Node : Nodes)
	{
		Node->Modify();
	}

	return ModifyReturnValue;
}

void UDialogueEdGraph::PostEditUndo()
{
	Super::PostEditUndo();
	NotifyGraphChanged();
}

void UDialogueEdGraph::PostInitProperties()
{
	Super::PostInitProperties();

	//Set up speaker roles changed event
	UDialogue* OuterDialogue = Cast<UDialogue>(GetOuter());
	if (OuterDialogue)
	{
		OuterDialogue->OnSpeakerRolesChanged.BindUFunction(
			this,
			"OnSpeakerRolesChanged"
		);
	}
}

UDialogue* UDialogueEdGraph::GetDialogue() const
{
	return CastChecked<UDialogue>(GetOuter());
}

void UDialogueEdGraph::AddToNodeMap(UGraphNodeDialogue* InNode)
{
	check(InNode);
	NodeMap.Add(InNode->GetID(), InNode);
}

void UDialogueEdGraph::RemoveFromNodeMap(FName RemoveID)
{
	NodeMap.Remove(RemoveID);
}

bool UDialogueEdGraph::ContainsNode(FName InID) const
{
	return NodeMap.Contains(InID);
}

void UDialogueEdGraph::SetGraphRoot(UGraphNodeDialogue* InRoot)
{
	Root = InRoot;
}

UGraphNodeDialogue* UDialogueEdGraph::GetNode(FName InID) const
{
	if (NodeMap.Contains(InID))
	{
		return NodeMap[InID];
	}

	return nullptr;
}

TArray<UGraphNodeDialogue*> UDialogueEdGraph::GetAllNodes() const
{
	TArray<UGraphNodeDialogue*> DialogueNodes;
	for (auto& Entry : NodeMap)
	{
		if (Entry.Value)
		{
			DialogueNodes.Add(Entry.Value);
		}
	}

	return DialogueNodes;
}

bool UDialogueEdGraph::HasSpeaker(FName InName) const
{
	return GetDialogue()->GetSpeakerRoles().Contains(InName);
}

FColor UDialogueEdGraph::GetSpeakerColor(FName InName) const
{
	if (GetDialogue()->GetSpeakerRoles().Contains(InName))
	{
		return GetDialogue()->GetSpeakerRoles()[InName].GraphColor;
	}

	return FColor::White;
}

TArray<UDialogueSpeakerSocket*> UDialogueEdGraph::GetAllSpeakers() const
{
	TArray<UDialogueSpeakerSocket*> AllSpeakers;
	for (auto& Entry : GetDialogue()->GetSpeakerRoles())
	{
		AllSpeakers.Add(Entry.Value.SpeakerSocket);
	}
	return AllSpeakers;
}

void UDialogueEdGraph::CompileAsset()
{
	//Verify asset and root exist 
	UDialogue* Asset = GetDialogue();
	check(Asset && Root);

	//Prepare the dialogue to be compiled
	Asset->PreCompileDialogue();

	//Clear asset nodes
	ClearAssetNodes();

	//Ensure we can compile the asset 
	if (!CanCompileAsset())
	{
		Asset->SetCompileStatus(EDialogueCompileStatus::Failed);
		return;
	}

	//Set up speakers and root
	Root->CreateAssetNode(Asset);
	Asset->SetRootNode(Root->GetAssetNode());

	//Compile asset tree
	CreateAssetNodes(Asset);
	TSet<UGraphNodeDialogue*> VisitedNodes;
	UpdateAssetTreeRecursive(Root, VisitedNodes);
	FinalizeAssetNodes();

	//Mark compilation as successful 
	Asset->SetCompileStatus(EDialogueCompileStatus::Compiled);
}

bool UDialogueEdGraph::CanCompileAsset() const
{
	//Get all nodes
	TArray<UGraphNodeDialogue*> DialogueNodes;
	GetNodesOfClass<UGraphNodeDialogue>(DialogueNodes);

	//Verify all nodes can compile
	bool bCanCompile = true;
	for (UGraphNodeDialogue* Node : DialogueNodes)
	{
		if (!Node->CanCompileNode())
		{
			bCanCompile = false;
		}
	}

	return bCanCompile;
}

void UDialogueEdGraph::UpdateAllNodeVisuals()
{
	for (auto& Entry : NodeMap)
	{
		Entry.Value->UpdateDialogueNode();
	}
}

void UDialogueEdGraph::ClearAssetNodes()
{
	for (UEdGraphNode* Current : Nodes)
	{
		if (UGraphNodeDialogue* DialogueNode =
			Cast<UGraphNodeDialogue>(Current))
		{
			DialogueNode->ClearAssetNode();
		}
	}
}

void UDialogueEdGraph::CreateAssetNodes(UDialogue* InAsset)
{
	for (auto& Entry : NodeMap)
	{
		check(Entry.Value);
		Entry.Value->CreateAssetNode(InAsset);
		Entry.Value->AssignAssetNodeID();
		InAsset->AddNode(Entry.Value->GetAssetNode());
	}
}

void UDialogueEdGraph::FinalizeAssetNodes()
{
	for (auto& Entry : NodeMap)
	{
		check(Entry.Value);
		Entry.Value->FinalizeAssetNode();
	}
}

void UDialogueEdGraph::UpdateAssetTreeRecursive(UGraphNodeDialogue* InRoot,
	TSet<UGraphNodeDialogue*> VisitedNodes)
{
	check(InRoot);

	if (VisitedNodes.Contains(InRoot))
	{
		return;
	}
	else
	{
		VisitedNodes.Add(InRoot);
	}

	//Link the asset root to its parents
	InRoot->LinkAssetNode();

	//Retrieve children and order left to right
	TArray<UGraphNodeDialogue*> OutChildren;
	InRoot->GetChildren(OutChildren);
	UGraphNodeDialogue::SortNodesLeftToRight(OutChildren);

	//Recur over children
	for (UGraphNodeDialogue* Child : OutChildren)
	{
		UpdateAssetTreeRecursive(Child, VisitedNodes);
	}
}

void UDialogueEdGraph::OnDialogueGraphChanged(
	const FEdGraphEditAction& EditAction)
{
	//If removing a node, pull that node from the node map
	if (EditAction.Action == GRAPHACTION_RemoveNode)
	{ 
		for (const UEdGraphNode* Removed : EditAction.Nodes)
		{
			const UGraphNodeDialogue* RemovedNode =
				Cast<UGraphNodeDialogue>(Removed);

			if (RemovedNode)
			{
				NodeMap.Remove(RemovedNode->GetID());
			}
		}
	}
}

void UDialogueEdGraph::OnSpeakerRolesChanged()
{
	CanCompileAsset(); //Check for error banners
	UpdateAllNodeVisuals();
}
