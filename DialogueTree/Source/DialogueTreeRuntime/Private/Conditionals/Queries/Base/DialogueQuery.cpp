// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Conditionals/Queries/Base/DialogueQuery.h"
//Plugin
#include "Dialogue.h"
#include "LogDialogueTree.h"

void UDialogueQuery::SetDialogue(UDialogue* InDialogue)
{
    check(InDialogue);
    Dialogue = InDialogue;
}

UDialogue* UDialogueQuery::GetDialogue() const
{
    check(Dialogue)
    return Dialogue;
}

FText UDialogueQuery::GetGraphDescription_Implementation() const
{
    return FText::FromString(GetClass()->GetName());
}

bool UDialogueQuery::IsValidQuery() const
{
    return true;
}
