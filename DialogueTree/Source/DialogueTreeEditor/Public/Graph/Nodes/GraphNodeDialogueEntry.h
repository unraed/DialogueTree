// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "GraphNodeDialogue.h"
//Generated
#include "GraphNodeDialogueEntry.generated.h"

/**
 * Graph node that represents the root or entry point into the dialogue graph.
 */
UCLASS()
class DIALOGUETREEEDITOR_API UGraphNodeDialogueEntry : public UGraphNodeDialogue
{
	GENERATED_BODY()

public: 
	/** UEdGraphNode Impl. */
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool CanUserDeleteNode() const;
	virtual bool CanDuplicateNode() const override;
	/** End UEdGraphNode */

	/** UGraphNodeDialogueBase Impl. */
	virtual FText GetContextMenuName() const override;
	/** End UGraphNodeDialogueBase */

	/** UGraphNodeDialogue Impl. */
	virtual void CreateAssetNode(class UDialogue* InAsset) override;
	virtual FName GetBaseID() const override;
	/** End UGraphNodeDialogue */
};
