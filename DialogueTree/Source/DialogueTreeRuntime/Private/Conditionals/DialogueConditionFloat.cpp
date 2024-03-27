// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Conditionals/DialogueConditionFloat.h"
//Plugin
#include "Conditionals/Queries/Base/DialogueQueryFloat.h"

#define LOCTEXT_NAMESPACE "DialogueQueryFloat"

bool UDialogueConditionFloat::IsMet() const
{
	check(Query);
	double QueryValue = Query->ExecuteQuery();

	if (Comparison == EFloatComparison::GreaterThan)
	{
		return QueryValue > CompareValue;
	}
	else
	{
		return QueryValue < CompareValue;
	}
}

void UDialogueConditionFloat::SetQuery(UDialogueQuery* InQuery)
{
	check(InQuery);
	Query = Cast<UDialogueQueryFloat>(InQuery);
	check(Query);
}

void UDialogueConditionFloat::SetDialogue(UDialogue* InDialogue)
{
	check(InDialogue);
	Query->SetDialogue(InDialogue);
}

FText UDialogueConditionFloat::GetDisplayText(const TMap<FName,
	FText>& ArgTexts, const FText QueryText) const
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

FText UDialogueConditionFloat::GetGraphDescription(FText QueryText)
{
	FText BaseText = LOCTEXT("BaseText", "{0} {1} {2}");
	FText CompareText;
	switch (Comparison)
	{
	case EFloatComparison::GreaterThan:
		CompareText = LOCTEXT("GreaterThan", ">");
		break;
	default:
		CompareText = LOCTEXT("LessThan", "<");
		break;
	}
	FText CompareValueText = FText::AsNumber(CompareValue);

	return FText::Format(
		BaseText,
		QueryText,
		CompareText,
		CompareValueText);
}

bool UDialogueConditionFloat::IsValidCondition()
{
	if (Query && Query->IsValidQuery())
	{
		return true;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE