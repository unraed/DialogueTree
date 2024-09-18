// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Events/DialogueEventBase.h"
#include "SetResumeNode.generated.h"

class UDialogueNodeSocket;

/**
 * Dialogue event that sets the node to use when resuming the current dialogue.
 */
UCLASS()
class DIALOGUETREERUNTIME_API USetResumeNode : public UDialogueEventBase
{
	GENERATED_BODY()
	
public:
	/** UDialogueEvent Impl. */
	virtual void PlayEvent() override;
	virtual bool HasAllRequirements() const override;
	virtual FText GetGraphDescription_Implementation() const override;
	/** End UDialogueEvent */

private:
	/** Node to reset */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TObjectPtr<UDialogueNodeSocket> TargetNode;
};
