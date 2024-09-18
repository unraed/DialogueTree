// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Nodes/GraphNodeDialogueJump.h"
//Plugin
#include "Dialogue.h"
#include "DialogueNodeSocket.h"
#include "Graph/DialogueEdGraph.h"
#include "Nodes/DialogueJumpNode.h"

#define LOCTEXT_NAMESPACE "GraphNodeDialogueJump"

UGraphNodeDialogueJump* UGraphNodeDialogueJump::MakeTemplate(UObject* Outer)
{
    return NewObject<UGraphNodeDialogueJump>(Outer);
}

FText UGraphNodeDialogueJump::GetTooltipText() const
{
    return LOCTEXT(
        "TooltipText", 
        "Reverts the flow of dialogue to a specified node."
    );
}

void UGraphNodeDialogueJump::AllocateDefaultPins()
{
    CreatePin(EGPD_Input, "MultipleNodes", FName());
}

void UGraphNodeDialogueJump::PostPasteNode()
{
    Super::PostPasteNode();

    check(GetDialogueGraph());
    UDialogue* DialogueAsset = GetDialogueGraph()->GetDialogue();
    check(DialogueAsset);

    JumpTarget = DuplicateObject(JumpTarget, DialogueAsset);
}

void UGraphNodeDialogueJump::CreateAssetNode(UDialogue* InAsset)
{
    UDialogueJumpNode* NewNode = NewObject<UDialogueJumpNode>(InAsset);
    check(NewNode);

    SetAssetNode(NewNode);
}

void UGraphNodeDialogueJump::FinalizeAssetNode()
{
    UDialogueJumpNode* TargetAssetNode =
        CastChecked<UDialogueJumpNode>(GetAssetNode());
    UGraphNodeDialogue* TargetGraphNode = GetJumpTarget();

    check(TargetGraphNode);
    check(TargetGraphNode->GetAssetNode());
    TargetAssetNode->SetJumpTarget(TargetGraphNode->GetAssetNode());
}

bool UGraphNodeDialogueJump::CanCompileNode()
{
    UDialogueEdGraph* Graph = GetDialogueGraph();
    check(Graph);

    UGraphNodeDialogue* TargetNode = GetJumpTarget();

    if (TargetNode
        && Graph->ContainsNode(TargetNode->GetID())
        && TargetNode != this)
    {
        SetErrorFlag(false);
        return true;
    }

    SetErrorFlag(true);
    return false;
}

FName UGraphNodeDialogueJump::GetBaseID() const
{
    return "Jump";
}

UGraphNodeDialogue* UGraphNodeDialogueJump::GetJumpTarget()
{
    if (!JumpTarget || !JumpTarget->GetGraphNode())
    {
        return nullptr;
    }

    return Cast<UGraphNodeDialogue>(JumpTarget->GetGraphNode());
}

#undef LOCTEXT_NAMESPACE