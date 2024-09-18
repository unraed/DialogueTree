// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Nodes/GraphNodeDialogue.h"
//Plugin
#include "Dialogue.h"
#include "Graph/DialogueEdGraph.h"
#include "Nodes/DialogueNode.h"

#define LOCTEXT_NAMESPACE "DialogueEdGraph"

UGraphNodeDialogue::UGraphNodeDialogue()
	: AssetNode(nullptr)
{
}

void UGraphNodeDialogue::PostEditUndo()
{
	Super::PostEditUndo();
	UpdateDialogueNode();
	MarkDialogueDirty();
}

void UGraphNodeDialogue::PostEditChangeProperty(
	FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateDialogueNode();
	MarkDialogueDirty();
}

FLinearColor UGraphNodeDialogue::GetNodeTitleColor() const
{
	//Defaults to a gray color 
	return DEFAULT_GRAY;
}

FText UGraphNodeDialogue::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (!ID.IsNone())
	{
		return FText::FromName(ID);
	}

	return FText::FromName(GetFName());
}

void UGraphNodeDialogue::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);
	UpdateDialogueNode();
	MarkDialogueDirty();
}

void UGraphNodeDialogue::ResizeNode(const FVector2D& NewSize)
{
	//If we are not setting size for the first time, adjust position 
	if (NodeWidth > 1.f)
	{
		float WidthChange = NewSize.X - NodeWidth;
		NodePosX -= WidthChange / 2.f;
	}

	//Cache the new size
	NodeWidth = NewSize.X;
	NodeHeight = NewSize.Y;
}

FName UGraphNodeDialogue::GetID() const
{
	return ID;
}

UDialogueNode* UGraphNodeDialogue::GetAssetNode() const
{
	return AssetNode;
}

void UGraphNodeDialogue::SetAssetNode(UDialogueNode* InDialogueNode)
{
	AssetNode = InDialogueNode;
}

bool UGraphNodeDialogue::CanCompileNode()
{
	return true;
}

UDialogueEdGraph* UGraphNodeDialogue::GetDialogueGraph() const
{
	return DialogueGraph;
}

void UGraphNodeDialogue::BindOnUpdateVisuals(FOnUpdateNode& UpdateDelegate)
{
	OnUpdateVisuals = UpdateDelegate;
}

void UGraphNodeDialogue::SortNodesLeftToRight(
	TArray<UGraphNodeDialogue*>& Nodes)
{
	Nodes.Sort(
		[](const UGraphNodeDialogue& Node1,
			const UGraphNodeDialogue& Node2)
		{
			return Node1.NodePosX < Node2.NodePosX;
		}
	);
}

void UGraphNodeDialogue::SetErrorFlag(bool InFlag)
{
	if (InFlag != bDialogueError)
	{
		bDialogueError = InFlag;
		UpdateDialogueNode();
	}
}

bool UGraphNodeDialogue::HasError() const
{
	return bDialogueError;
}

void UGraphNodeDialogue::LinkAssetNode()
{
	check(AssetNode);
	
	//Retrieve parent nodes
	TArray<UGraphNodeDialogue*> Parents; 
	GetParents(Parents);

	//Link the asset node to its parents
	for (UGraphNodeDialogue* Parent : Parents)
	{
		//Verify that the parent's asset node has been spawned
		if (Parent->GetAssetNode())
		{
			LinkToParent(Parent);
			Parent->LinkToChild(this);
		}
	}
}

void UGraphNodeDialogue::LinkToParent(UGraphNodeDialogue* InParent)
{
	UDialogueNode* ParentAssetNode = InParent->GetAssetNode();
	if (AssetNode && ParentAssetNode)
	{
		AssetNode->AddParent(ParentAssetNode);
	}
}

void UGraphNodeDialogue::LinkToChild(UGraphNodeDialogue* InChild)
{
	UDialogueNode* ChildAssetNode = InChild->GetAssetNode();
	if (AssetNode && InChild)
	{
		AssetNode->AddChild(ChildAssetNode);
	}
}

void UGraphNodeDialogue::ClearAssetNode()
{
	AssetNode = nullptr;
}

void UGraphNodeDialogue::GetParents(TArray<UGraphNodeDialogue*>& OutNodes) const
{
	OutNodes.Empty();
	GetParents(GetDirectParents(), OutNodes);
}

void UGraphNodeDialogue::GetChildren(
	TArray<UGraphNodeDialogue*>& OutNodes) const
{
	OutNodes.Empty();
	GetChildren(GetDirectChildren(), OutNodes);
}

void UGraphNodeDialogue::GetPinChildren(UEdGraphPin* InPin, 
	TArray<UGraphNodeDialogue*>& OutNodes) const
{
	OutNodes.Empty();

	TArray<UGraphNodeDialogueBase*> LinkedNodes;
	for (UEdGraphPin* LinkedPin : InPin->LinkedTo)
	{
		UGraphNodeDialogueBase* BaseNode = 
			Cast<UGraphNodeDialogueBase>(
				LinkedPin->GetOwningNode()
			);

		if (BaseNode)
		{
			LinkedNodes.Add(BaseNode);
		}
	}

	GetChildren(LinkedNodes, OutNodes);
}

void UGraphNodeDialogue::MarkDialogueDirty()
{
	if (!DialogueGraph)
	{
		return;
	}

	DialogueGraph->GetDialogue()->SetCompileStatus(
		EDialogueCompileStatus::Uncompiled
	);
}

void UGraphNodeDialogue::UpdateDialogueNode()
{
	OnUpdateVisuals.ExecuteIfBound();
}

FName UGraphNodeDialogue::GetBaseID() const
{
	return FName("DialogueNode");
}

void UGraphNodeDialogue::AssignAssetNodeID() const
{
	check(AssetNode && !ID.IsNone());
	AssetNode->SetNodeID(ID);
}

void UGraphNodeDialogue::InitNodeInDialogueGraph(UEdGraph* OwningGraph)
{
	check(OwningGraph);
	DialogueGraph = CastChecked<UDialogueEdGraph>(OwningGraph);
	FText BaseIDText = LOCTEXT("BaseIDText", "{0} {1}");

	//Set initial ID
	int32 Counter = 1;
	FName BaseID = GetBaseID();
	FText IDText = FText::FromName(BaseID);

	//Increment number until ID is unique 
	while (DialogueGraph->ContainsNode(FName(IDText.ToString())))
	{
		Counter++;
		FText CounterText = FText::AsNumber(Counter);

		IDText = FText::Format(
			BaseIDText,
			FText::FromName(BaseID),
			CounterText
		);
	}
	
	//Set the ID
	ID = FName(IDText.ToString());
	DialogueGraph->AddToNodeMap(this);
}

void UGraphNodeDialogue::ResetID()
{
	check(DialogueGraph);

	DialogueGraph->RemoveFromNodeMap(ID);
	InitNodeInDialogueGraph(DialogueGraph);
}

void UGraphNodeDialogue::GetParents(TArray<UGraphNodeDialogueBase*> InParents,
	TArray<UGraphNodeDialogue*>& OutNodes) const
{
	//Parents of to recur over (nodes that don't descend from this)
	TArray<UGraphNodeDialogueBase*> RecurParents;

	//Go through all input parent nodes
	for (UGraphNodeDialogueBase* Node : InParents)
	{
		//If the parent is a GraphNodeDialogue
		if (UGraphNodeDialogue* DialogueNode =
			Cast<UGraphNodeDialogue>(Node))
		{
			OutNodes.Add(DialogueNode);
		}
		//If not a GraphNodeDialogue, recur over the parent's parents
		else
		{
			for (UGraphNodeDialogueBase* Parent : Node->GetDirectParents())
			{
				if (RecurParents.Contains(Parent) == false)
				{
					RecurParents.Add(Parent);
				}
			}
		}
	}

	//If we found any non-GraphNodeDialogue nodes
	if (RecurParents.Num() > 0)
	{
		GetParents(RecurParents, OutNodes);
	}
}

void UGraphNodeDialogue::GetChildren(TArray<UGraphNodeDialogueBase*> InChildren,
	TArray<UGraphNodeDialogue*>& OutNodes) const
{
	//Children of non-GraphNodeDialogue child nodes
	TArray<UGraphNodeDialogueBase*> RecurChildren;

	//Go through all input child nodes
	for (UGraphNodeDialogueBase* Node : InChildren)
	{
		//If the child is a GraphNodeDialogue
		if (UGraphNodeDialogue* DialogueNode =
			Cast<UGraphNodeDialogue>(Node))
		{
			OutNodes.Add(DialogueNode);
		}
		//If not an element, recur over the child's children
		else
		{
			for (UGraphNodeDialogueBase* Child : Node->GetDirectChildren())
			{
				if (RecurChildren.Contains(Child) == false)
				{
					RecurChildren.Add(Child);
				}
			}
		}
	}

	//If any non-GraphNodeDialogue children found, recur
	if (RecurChildren.Num() > 0)
	{
		GetChildren(RecurChildren, OutNodes);
	}
}

#undef LOCTEXT_NAMESPACE