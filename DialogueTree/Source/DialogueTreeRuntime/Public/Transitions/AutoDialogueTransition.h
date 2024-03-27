// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "DialogueTransition.h"
//Generated
#include "AutoDialogueTransition.generated.h"

/**
 * Speech transition that waits until the speech is done playing 
 * and then automatically progresses onward. 
 */
UCLASS()
class DIALOGUETREERUNTIME_API UAutoDialogueTransition :
	public UDialogueTransition
{
	GENERATED_BODY()

public: 
	/** DialogueTransition Implementation */
	virtual void TransitionOut() override;
	virtual FText GetDisplayName() const override;
	virtual FText GetNodeCreationTooltip() const override;
	/** End DialogueTranstion */
};
