// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "GraphNodeDialogueBase.h"
//Generated
#include "GraphNodeDialogueReroute.generated.h"

/**
 * Node that redirects dialogue flow between nodes with actual content. 
 */
UCLASS()
class DIALOGUETREEEDITOR_API UGraphNodeDialogueReroute :
	public UGraphNodeDialogueBase
{
	GENERATED_BODY()
	
public:
	/** UEdGraphNode Impl. */
	virtual bool ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex,
		int32& OutOutputPinIndex) const override;
	virtual void AllocateDefaultPins() override;
	/** End UEdGraphNode */
};
