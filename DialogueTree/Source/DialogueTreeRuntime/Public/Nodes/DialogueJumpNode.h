// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "Nodes/DialogueNode.h"
//Generated
#include "DialogueJumpNode.generated.h"

/**
 * Dialogue node that reverts control/flow to another node in the
 * dialogue, as specified by the user. 
 */
UCLASS()
class DIALOGUETREERUNTIME_API UDialogueJumpNode : public UDialogueNode
{
	GENERATED_BODY()
	
public:
	/** UDialogueNode Implementation */
	virtual void EnterNode() override;
	virtual FDialogueOption GetAsOption() override;
	/** End UDialogueNode */

	/**
	* Sets the target of the node's jump. 
	* 
	* @param InTarget - UDialogueNode*, the node to jump to. 
	*/
	void SetJumpTarget(UDialogueNode* InTarget);

private:
	/** The target node to "jump" to */
	UPROPERTY()
	TObjectPtr<UDialogueNode> JumpTarget;
};
