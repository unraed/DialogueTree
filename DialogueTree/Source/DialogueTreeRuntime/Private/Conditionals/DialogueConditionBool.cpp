// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Conditionals/DialogueConditionBool.h"
//Plugin
#include "Conditionals/Queries/Base/DialogueQueryBool.h"

#define LOCTEXT_NAMESPACE "DialogueQueryBool"

bool UDialogueConditionBool::IsMet() const
{
	check(Query);
	bool bQueryValue = Query->ExecuteQuery();
	
	if (QueryTrue)
	{
		return bQueryValue;
	}
	else
	{
		return bQueryValue == false;
	}
}

void UDialogueConditionBool::SetQuery(UDialogueQuery* InQuery)
{
	check(InQuery);
	Query = Cast<UDialogueQueryBool>(InQuery);
	check(Query)
}

void UDialogueConditionBool::SetDialogue(UDialogue* InDialogue)
{
	check(InDialogue);
	Query->SetDialogue(InDialogue);
}

FText UDialogueConditionBool::GetDisplayText(const TMap<FName, FText>& ArgTexts,
	const FText QueryText) const
{
	FText BaseText = LOCTEXT("BaseText", "{0} is {1}");
	FText NegateText = ArgTexts.FindChecked("QueryTrue");

	return FText::Format(
		BaseText,
		QueryText,
		NegateText);
}

FText UDialogueConditionBool::GetGraphDescription(FText QueryText)
{
	FText BaseText = LOCTEXT("BaseText", "{0}{1}");
	FText NegateText = QueryTrue ? FText() : LOCTEXT("FalseText", "NOT ");

	return FText::Format(
		BaseText,
		NegateText,
		QueryText);
}

bool UDialogueConditionBool::IsValidCondition()
{
	if (Query && Query->IsValidQuery())
	{
		return true;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE