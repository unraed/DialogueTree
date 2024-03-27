// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Conditionals/DialogueCondition.h"
//Plugin
#include "LogDialogueTree.h"

void UDialogueCondition::SetQuery(UDialogueQuery* InQuery)
{
    UE_LOG(
        LogDialogueTree,
        Error,
        TEXT("Should not use abstract condition directly")
    );
}

void UDialogueCondition::SetDialogue(UDialogue* InDialogue)
{
    UE_LOG(
        LogDialogueTree,
        Error,
        TEXT("Should not use abstract condition directly")
    );
}

bool UDialogueCondition::IsMet() const
{
    UE_LOG(
        LogDialogueTree,
        Error,
        TEXT("Should not use abstract condition directly")
    );
    return false;
}

FText UDialogueCondition::GetDisplayText(const TMap<FName,
    FText>& ArgTexts, const FText QueryText) const
{
    UE_LOG(
        LogDialogueTree,
        Error,
        TEXT("Should not use abstract condition directly")
    );
    return FText();
}

FText UDialogueCondition::GetGraphDescription(FText QueryText)
{
    return FText();
}

bool UDialogueCondition::IsValidCondition()
{
    return false;
}
