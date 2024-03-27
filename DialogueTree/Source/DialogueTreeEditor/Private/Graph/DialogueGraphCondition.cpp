// Copyright Zachary Brett, 2024. All rights reserved.


#include "Graph/DialogueGraphCondition.h"
#include "Conditionals/DialogueConditionBool.h"
#include "Conditionals/DialogueConditionFloat.h"
#include "Conditionals/DialogueConditionInt.h"
#include "Conditionals/Queries/Base/DialogueQueryBool.h"
#include "Conditionals/Queries/Base/DialogueQueryFloat.h"
#include "Conditionals/Queries/Base/DialogueQueryInt.h"
#include "Conditionals/Queries/NodeVisitedQuery.h"
#include "Dialogue.h"
#include "DialogueNodeSocket.h"
#include "Graph/Nodes/GraphNodeDialogue.h"

void UDialogueGraphCondition::PostDuplicate(bool bDuplicateForPIE)
{
    Super::PostDuplicate(bDuplicateForPIE);

    if (!Query)
    {
        return;
    }

    Query = DuplicateObject(Query, this);
    RefreshCondition();
}

void UDialogueGraphCondition::RefreshCondition()
{
    if (!Query)
    {
        Condition = nullptr;
        return;
    }

    if (Query->IsA(UDialogueQueryBool::StaticClass()))
    {
        Condition = NewObject<UDialogueConditionBool>(this);
        Condition->SetQuery(Query);
    }
    else if (Query->IsA(UDialogueQueryFloat::StaticClass()))
    {
        Condition = NewObject<UDialogueConditionFloat>(this);
        Condition->SetQuery(Query);
    }
    else if (Query->IsA(UDialogueQueryInt::StaticClass()))
    {
        Condition = NewObject<UDialogueConditionInt>(this);
        Condition->SetQuery(Query);
    }
    else
    {
        Condition = nullptr;
    }
}

void UDialogueGraphCondition::FinalizeCondition(UDialogue* InOuter)
{
    check(Query);
    check(Condition);
    check(InOuter);

    //Change outer and set up
    Query->Rename(nullptr, InOuter);
    Condition->Rename(nullptr, InOuter);
    Condition->SetDialogue(InOuter);

    //If a node visited query, update the node socket
    UNodeVisitedQuery* NodeQuery = Cast<UNodeVisitedQuery>(Query);
    if (NodeQuery)
    {
        HandleNodeVisitedQuery(NodeQuery);
    }
}

UDialogueQuery* UDialogueGraphCondition::GetQuery()
{
    return Query;
}

UDialogueCondition* UDialogueGraphCondition::GetCondition()
{
    return Condition;
}

bool UDialogueGraphCondition::ShouldRefreshCondition()
{
    if (!Condition)
    {
        if (Query)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    if (Query)
    {
        if (Query->IsA<UDialogueQueryBool>() 
            && !Condition->IsA<UDialogueConditionBool>())
        {
            return true;
        }
        
        if (Query->IsA<UDialogueQueryFloat>()
            && !Condition->IsA<UDialogueConditionFloat>())
        {
            return true;
        }
        
        if (Query->IsA<UDialogueQueryInt>()
            && !Condition->IsA<UDialogueConditionInt>())
        {
            return true;
        }
    }

    return false;
}

void UDialogueGraphCondition::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    if (ShouldRefreshCondition())
    {
        RefreshCondition();
    }
}

void UDialogueGraphCondition::HandleNodeVisitedQuery(UNodeVisitedQuery* InQuery)
{
    check(InQuery);
    UDialogueNodeSocket* TargetSocket = InQuery->GetSocket();

    check(TargetSocket);
    check(TargetSocket->GetGraphNode());

    UGraphNodeDialogue* TargetGraphNode =
        Cast<UGraphNodeDialogue>(TargetSocket->GetGraphNode());

    check(TargetGraphNode);

    UDialogueNode* TargetDialogueNode = TargetGraphNode->GetAssetNode();

    check(TargetDialogueNode);

    TargetSocket->SetDialogueNode(TargetDialogueNode);
}
