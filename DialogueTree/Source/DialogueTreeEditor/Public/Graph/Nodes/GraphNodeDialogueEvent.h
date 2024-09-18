// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "Graph/Nodes/GraphNodeDialogue.h"
//Generated
#include "GraphNodeDialogueEvent.generated.h"

class UDialogueEventBase;

USTRUCT()
struct FGraphDialogueEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, Category = "Dialogue")
	UDialogueEventBase* Event;
};

/**
 * Graph node representing a series of events triggered from dialogue. 
 */
UCLASS()
class DIALOGUETREEEDITOR_API UGraphNodeDialogueEvent : public UGraphNodeDialogue
{
	GENERATED_BODY()

public:
	/** Pseudo constructor for creating a blank template event node. Static.
	*
	* @param Outer - UObject*, the owner for the new template.
	*/
	static UGraphNodeDialogueEvent* MakeTemplate(UObject* Outer);

public:
	/** UObject Implementation */
	virtual void PostEditChangeProperty(
		struct FPropertyChangedEvent& PropertyChangedEvent) override;
	/** End UObject*/

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
	* Retrieves the display texts for the assigned events as an array. 
	* 
	* @return TArray<FText&> - the display texts for all assigned events.
	*/
	TArray<FText> GetGraphDescriptions() const;

	/**
	* Returns the number of events associated with the node.
	* 
	* @return int - number of events.
	*/
	int GetNumEvents() const;

private:
	void FinalizeNodeSocket(UDialogueEventBase* InEvent);

private:
	/** The list of user-defined events to play */
	UPROPERTY(EditAnywhere, Category = "DialogueEvents")
	TArray<FGraphDialogueEvent> Events;
};
