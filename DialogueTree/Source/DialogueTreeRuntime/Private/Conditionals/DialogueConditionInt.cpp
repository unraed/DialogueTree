// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Conditionals/DialogueConditionInt.h"
//Plugin
#include "Conditionals/Queries/Base/DialogueQueryInt.h"

#define LOCTEXT_NAMESPACE "DialogueQueryInt"

bool UDialogueConditionInt::IsMet() const
{
    check(Query);
    int32 QueryValue = Query->ExecuteQuery();

    if (Comparison == EIntComparison::GreaterThan)
    {
        return QueryValue > CompareValue;
    }
    else if (Comparison == EIntComparison::LessThan)
    {
        return QueryValue < CompareValue;
    }
    else //Equal to
    {
        return QueryValue == CompareValue;
    }
}

void UDialogueConditionInt::SetQuery(UDialogueQuery* InQuery)
{
    check(InQuery);
    Query = Cast<UDialogueQueryInt>(InQuery);
    check(Query);
}

void UDialogueConditionInt::SetDialogue(UDialogue* InDialogue)
{
    check(InDialogue);
    Query->SetDialogue(InDialogue);
}

FText UDialogueConditionInt::GetDisplayText(const TMap<FName, FText>& ArgTexts, 
    const FText QueryText) const
{
    FText BaseText = LOCTEXT("BaseText", "{0} {1} {2}");
    FText CompareText = ArgTexts.FindChecked("Comparison");
    FText CompareValueText = ArgTexts.FindChecked("CompareValue");

    return FText::Format(
        BaseText, 
        QueryText,
        CompareText,
        CompareValueText);
}

FText UDialogueConditionInt::GetGraphDescription(FText QueryText)
{
    FText BaseText = LOCTEXT("BaseText", "{0} {1} {2}");
    FText CompareText;
    switch (Comparison)
    {
    case EIntComparison::GreaterThan:
        CompareText = LOCTEXT("GreaterThan", ">");
        break;
    case EIntComparison::LessThan:
        CompareText = LOCTEXT("LessThan", "<");
        break;
    default:
        CompareText = LOCTEXT("EqualTo", "=");
        break;
    }
    FText CompareValueText = FText::AsNumber(CompareValue);

    return FText::Format(
        BaseText,
        QueryText,
        CompareText,
        CompareValueText);
}

bool UDialogueConditionInt::IsValidCondition()
{
    if (Query && Query->IsValidQuery())
    {
        return true;
    }

    return false;
}

#undef LOCTEXT_NAMESPACE