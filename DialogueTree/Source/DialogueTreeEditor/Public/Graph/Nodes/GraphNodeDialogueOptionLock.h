// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Graph/Nodes/GraphNodeDialogue.h"
#include "GraphNodeDialogueOptionLock.generated.h"

class UDialogueGraphCondition;

/**
 * A graph node representing an option lock in dialogue.
 */
UCLASS()
class DIALOGUETREEEDITOR_API UGraphNodeDialogueOptionLock : public UGraphNodeDialogue
{
	GENERATED_BODY()

public:
	/** Pseudo constructor for creating a blank template option lock. Static.
	*
	* @param Outer - UObject*, the owner for the new template.
	*/
	static UGraphNodeDialogueOptionLock* MakeTemplate(UObject* Outer);

public:
	/** UEdGraphNode Implementation */
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual FName GetBaseID() const override;
	virtual void PostPasteNode() override;
	/** End UEdGraphNode */
	
	/** UGraphNodeDialogue Implementation */
	virtual void CreateAssetNode(class UDialogue* InAsset) override;
	virtual void FinalizeAssetNode() override;
	virtual bool CanCompileNode() override;
	/** End UGraphNodeDialogue */

	/**
	* Checks if the option lock should evaluate to true in cases where any 
	* single condition evaluates to true, or whether all must do so.
	*
	* @return bool - If the lock evaluates true when any single condition
	* does.
	*/
	bool GetIfAny() const;

	/**
	* Retrieves the display texts for all conditions attached to the node as
	* an array.
	*
	* @return TArray<FText> - the condition texts.
	*/
	TArray<FText> GetConditionDisplayTexts() const;

private:
	/** False if all conditions must be true. True otherwise */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	bool bIfAny = false;

	/** The list of conditions for the option lock */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TArray<TObjectPtr<UDialogueGraphCondition>> Conditions;

	/** Optional message to include when locked */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	FText LockedMessage = FText();

	/** Optional message to include when unlocked */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	FText UnlockedMessage = FText();
};
