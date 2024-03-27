// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "Graph/Nodes/GraphNodeDialogue.h"
//Generated
#include "GraphNodeDialogueBranch.generated.h"

class UDialogueGraphCondition;
class UDialogueCondition;

/**
 * A graph node representing an if/else style branch in dialogue. 
 */
UCLASS()
class DIALOGUETREEEDITOR_API UGraphNodeDialogueBranch : 
	public UGraphNodeDialogue
{
	GENERATED_BODY()

public:
	/** Pseudo constructor for creating a blank template branch. Static.
	*
	* @param Outer - UObject*, the owner for the new template.
	*/
	static UGraphNodeDialogueBranch* MakeTemplate(UObject* Outer);

public:
	/** UEdGraphNode Implementation */
	virtual FText GetTooltipText() const override;
	virtual FName GetBaseID() const override;
	virtual void AllocateDefaultPins() override;
	virtual void PostPasteNode() override;
	/** End UEdGraphNode */

	/** UGraphNodeDialogueBase Impl. */
	virtual FText GetPinMenuLabel(int32 PinIndex) const override;
	/** End UGraphNodeDialogueBase */

	/** UGraphNodeDialogue Implementation */
	virtual void CreateAssetNode(class UDialogue* InAsset) override;
	virtual void FinalizeAssetNode() override;
	virtual bool CanCompileNode() override;
	/** End UGraphNodeDialogue */

	/**
	* Checks if the branch should evaluate to true in cases where any single 
	* condition evaluates to true, or whether all must do so. 
	* 
	* @return bool - If the branch evaluates true when any single condition
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
	/**
	* Retrieves the dialogue node associated with the "true" branch of this
	* graph node. 
	* 
	* @param OutputPins - TArray<UEdGraphPin*>&, the array of this node's output
	* pins. 
	* @return UDialogueNode* - the asset node associated with the "true" branch.
	* nullptr if none. 
	*/
	UDialogueNode* GetTrueNode(TArray<UEdGraphPin*>& OutputPins) const;

	/**
	* Retrieves the dialogue node associated with the "false" branch of this
	* graph node.
	*
	* @param OutputPins - TArray<UEdGraphPin*>&, the array of this node's output
	* pins.
	* @return UDialogueNode* - the asset node associated with the "false" 
	* branch. Nullptr if none.
	*/
	UDialogueNode* GetFalseNode(TArray<UEdGraphPin*>& OutputPins) const;

	/**
	* Retrieves an array of Dialogue Conditions that are 
	* associated with this node's Graph Conditions. 
	* 
	* @return TArray<UObject*> - the condition objects. 
	*/
	TArray<UDialogueCondition*> GetConditions() const;

	/**
	* Retrieves the first valid child node linked to the given pin. 
	* 
	* @param InPin - UEdGraphPin*, the target pin. 
	* @return UGraphNodeDialogue*, the linked child. 
	*/
	UGraphNodeDialogue* GetFirstLinkedChild(UEdGraphPin* InPin) const;

private: 
	/** False if all conditions must be true. True otherwise */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	bool bIfAny = false;

	/** The list of conditions for the branch */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TArray<UDialogueGraphCondition*> Conditions; 

	/** Constants */
	const int32 INVALID_PIN_INDEX = 3;
};

