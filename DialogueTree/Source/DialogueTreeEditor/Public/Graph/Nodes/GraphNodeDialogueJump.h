// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "Graph/Nodes/GraphNodeDialogue.h"
#include "Graph/PickableDialogueNode.h"
#include "DialogueNodeSocket.h"
//Generated
#include "GraphNodeDialogueJump.generated.h"

/**
 * Graph node representing a jump in control to another node. 
 */
UCLASS()
class DIALOGUETREEEDITOR_API UGraphNodeDialogueJump : public UGraphNodeDialogue
{
	GENERATED_BODY()

public:
	/** Pseudo constructor for creating a blank template jump node. Static.
	*
	* @param Outer - UObject*, the owner for the new template.
	*/
	static UGraphNodeDialogueJump* MakeTemplate(UObject* Outer);

public:
	/** UEdGraphNode Implementation */
	virtual FText GetTooltipText() const override;
	virtual void AllocateDefaultPins() override;
	virtual void PostPasteNode() override;
	/** End UEdGraphNode */

	/** UGraphNodeDialogue Impl. */
	virtual void CreateAssetNode(class UDialogue* InAsset) override;
	virtual void FinalizeAssetNode() override;
	virtual bool CanCompileNode() override;
	virtual FName GetBaseID() const override;
	/** End UGraphNodeDialogue */
	
	/**
	* Retrieves the jump node's target Dialogue Node.  
	* 
	* @return UGraphNodeDialogue*, the jump target node. Nullptr if none set.
	*/
	UGraphNodeDialogue* GetJumpTarget();

private:
	/** The node to jump to */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	UDialogueNodeSocket* JumpTarget;
};
