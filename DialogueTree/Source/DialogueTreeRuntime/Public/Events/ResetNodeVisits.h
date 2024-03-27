// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Events/DialogueEventBase.h"
#include "ResetNodeVisits.generated.h"

class UDialogueNodeSocket;

/**
 * Dialogue event that clears/resets the visited status of a single node 
 * that the user can select from the dialogue editor.
 */
UCLASS()
class DIALOGUETREERUNTIME_API UResetNodeVisits : public UDialogueEventBase
{
	GENERATED_BODY()
	
protected:
	/** UDialogueEvent Impl. */
	virtual void PlayEvent() override;
	virtual bool HasAllRequirements() const override;
	virtual FText GetGraphDescription_Implementation() const override;
	/** End UDialogueEvent */

public:
	/**
	* Gets the target node socket. 
	* 
	* @return UDialogueNodeSocket*, the target socket.
	*/
	UDialogueNodeSocket* GetTargetSocket() const;

private:
	/** Node to reset */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	UDialogueNodeSocket* TargetNode;
};
