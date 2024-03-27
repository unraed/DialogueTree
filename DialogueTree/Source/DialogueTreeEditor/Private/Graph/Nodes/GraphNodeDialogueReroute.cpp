// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Nodes/GraphNodeDialogueReroute.h"

bool UGraphNodeDialogueReroute::ShouldDrawNodeAsControlPointOnly(
	int32& OutInputPinIndex, int32& OutOutputPinIndex) const
{
	OutInputPinIndex = 0;
	OutOutputPinIndex = 1;
	return true;
}

void UGraphNodeDialogueReroute::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FName());
	CreatePin(EGPD_Output, "MultipleNodes", FName());
}


