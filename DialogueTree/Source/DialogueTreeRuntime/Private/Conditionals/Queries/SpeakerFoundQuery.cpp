// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Conditionals/Queries/SpeakerFoundQuery.h"
//Plugin
#include "Dialogue.h"
#include "DialogueSpeakerSocket.h"

#define LOCTEXT_NAMESPACE "SpeakerFoundQuery"

bool USpeakerFoundQuery::ExecuteQuery()
{
	check(Speaker);

	return GetDialogue()->SpeakerIsPresent(Speaker->GetSpeakerName());
}

FText USpeakerFoundQuery::GetGraphDescription_Implementation() const
{
	//Get the speaker name from the arg texts
	if (!Speaker)
	{
		return LOCTEXT("InvalidSpeaker", "Invalid Speaker for Query");
	}

	FText SpeakerNameText = FText::FromName(Speaker->GetSpeakerName());

	//Construct and return the display text
	FText BaseText = LOCTEXT("BaseText", "{0} found");
	return FText::Format(BaseText, SpeakerNameText);
}

bool USpeakerFoundQuery::IsValidQuery() const
{
	if (Speaker && Speaker->IsValidSocket())
	{
		return true;
	}

	return false;
}

void USpeakerFoundQuery::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);
}

#undef LOCTEXT_NAMESPACE