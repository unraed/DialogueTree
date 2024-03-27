// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Events/ResetAllNodeVisits.h"
//Plugin
#include "Dialogue.h"

#define LOCTEXT_NAMESPACE "UResetAllNodeVisits"

void UResetAllNodeVisits::PlayEvent()
{
	if (Dialogue)
	{
		Dialogue->ClearAllNodeVisits();
	}
}

bool UResetAllNodeVisits::HasAllRequirements() const
{
	return true;
}

FText UResetAllNodeVisits::GetGraphDescription_Implementation() const
{
	return LOCTEXT("Description", "Mark all nodes unvisited");
}

#undef LOCTEXT_NAMESPACE