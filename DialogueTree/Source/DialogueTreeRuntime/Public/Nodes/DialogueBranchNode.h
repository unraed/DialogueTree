// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "DialogueNode.h"
//Generated
#include "DialogueBranchNode.generated.h"

class UDialogueCondition;

/**
 * Dialogue node that branches based on a set of user-specified
 * conditions. 
 */
UCLASS()
class DIALOGUETREERUNTIME_API UDialogueBranchNode : public UDialogueNode
{
	GENERATED_BODY()

public:
	/** UDialogueNode Implementation */
	virtual FDialogueOption GetAsOption() override;
	virtual void EnterNode() override;
	/** End UDialogueNode */

	/**
	* Populates the basic data for the branch node. 
	* 
	* @param InIfAny - bool, whether the conditions are linked together via an 
	* "or" relationship versus an "and" relationship.
	* @param InTrueNode - UDialogueNode*, node to transition to if conditions 
	* evaluate true.
	* @param InFalseNode - UDialogueNode*, node to transition to if conditions 
	* evaluate false. 
	* @param InConditions - TArray<UDialogueCondition*>&, array of conditions.
	*/
	void InitBranchData(bool InIfAny, UDialogueNode* InTrueNode, 
		UDialogueNode* InFalseNode, TArray<UDialogueCondition*>& InConditions);

	/**
	* Clears all conditions from the object
	*/
	void ClearConditions();

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

	/** The node to transition to if branch evaluates as true */
	UPROPERTY()
	TObjectPtr<UDialogueNode> TrueNode;

	/** The node to transition to if branch evaluates false */
	UPROPERTY()
	TObjectPtr<UDialogueNode> FalseNode;
};
