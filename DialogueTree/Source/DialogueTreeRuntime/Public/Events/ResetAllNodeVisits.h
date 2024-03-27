// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Events/DialogueEventBase.h"
#include "ResetAllNodeVisits.generated.h"

/**
 * Dialogue event that resets/clears the visited status of all nodes in the 
 * dialogue. Effectively returns the dialogue to an unexplored state. 
 */
UCLASS()
class DIALOGUETREERUNTIME_API UResetAllNodeVisits : public UDialogueEventBase
{
	GENERATED_BODY()
	
protected:
	/** UDialogueEvent Impl. */
	virtual void PlayEvent() override;
	virtual bool HasAllRequirements() const override;
	virtual FText GetGraphDescription_Implementation() const override;
	/** End UDialogueEvent */
};
