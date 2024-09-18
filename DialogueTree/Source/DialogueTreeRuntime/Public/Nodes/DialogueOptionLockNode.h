// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/DialogueNode.h"
#include "DialogueOptionLockNode.generated.h"

class UDialogueCondition;

/**
 * Dialogue node that serves as a conditional filter for a single option. 
 * If the condition fails to pass, the option is marked as "locked."
 */
UCLASS()
class DIALOGUETREERUNTIME_API UDialogueOptionLockNode : public UDialogueNode
{
	GENERATED_BODY()
	
public:
	/** UDialogueNode Implementation */
	virtual FDialogueOption GetAsOption() override;
	virtual void EnterNode() override;
	/** End UDialogueNode */

public:
	/**
	* Populates the basic data for the lock node.
	*
	* @param InIfAny - bool, whether the conditions are linked together via an
	* "or" relationship versus an "and" relationship.
	* @param InConditions - TArray<UDialogueCondition*>&, array of conditions.
	* @param LockedText - FText - optional message when locked
	* @param UnlockedText - FText - optional message when unlocked
	*/
	void InitLockNodeData(bool InIfAny, 
		TArray<UDialogueCondition*>& InConditions, 
		const FText& LockedText, const FText& UnlockedText);

private:
	/**
	* Determines if the branch node passes its conditions to
	* transition to the "true" node.
	*
	* @return bool - true if conditions are passed, false otherwise.
	*/
	bool PassesConditions() const;

	/**
	* Determines if any condition in the conditions list is true.
	*
	* @return bool - true if any condition is true, else false.
	*/
	bool AnyConditionsTrue() const;

	/**
	* Determines if all conditions in the conditions list are true.
	*
	* @return bool - true if all conditions are true, else false.
	*/
	bool AllConditionsTrue() const;

private:
	/** Conditions which govern branching */
	UPROPERTY()
	TArray<TObjectPtr<UDialogueCondition>> Conditions;

	/** If the branch should evaluate true if any condition does */
	UPROPERTY()
	bool bIfAny = false;

	/** Optional message to include when locked */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	FText LockedMessage = FText();

	/** Optional message to include when unlocked */
	FText UnlockedMessage = FText();
};
