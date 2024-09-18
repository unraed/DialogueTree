// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once 

//UE
#include "CoreMinimal.h"
//Plugin
#include "SGraphNodeDialogueBase.h"

class UGraphNodeDialogueBranch;

/**
* Governs the visual representation of a branch node in the dialogue graph. 
*/
class DIALOGUETREEEDITOR_API SGraphNodeDialogueBranch : 
	public SGraphNodeDialogueBase
{
public:
	//Slate Arguments 
	SLATE_BEGIN_ARGS(SGraphNodeDialogueBranch) {}
	SLATE_END_ARGS()

	/** Slate Constructor */
	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);

protected:
	/** SGraphNode Implementation */
	virtual TSharedRef<SWidget> CreateNodeContentArea() override;
	/** End SGraphNode */

	/** SGraphNodeDialogueBase Implementation */
	virtual TSharedRef<SWidget> CreateOutputPinArea() override;
	virtual float GetOutputPinYPadding() const override;
	/** End SGraphNodeDialogueBase */

private:
	/**
	* Retrieves the any/all text which displays whether all or any of the 
	* conditions must be met for the branch to register as "true".
	* 
	* @param BranchNode - UGraphNodeDialogueBranch*, the owning branch node.
	* @return FText - the any/all text. 
	*/
	FText GetAnyAllText() const;

	/**
	* Assembles the any all text and the condition texts into a widget for 
	* display in the content area. 
	* 
	* @param ConditionTexts - TArray<FText>&, the list of condition texts.
	* @param AnyAllText - FText that displays whether any or all of the 
	* conditions must be true. 
	* @return TSharedRef<SWidget> - The completed widget. 
	*/
	TSharedRef<SWidget> GetConditionBox(TArray<FText>& ConditionTexts, 
		FText& AnyAllText) const;

private:
	/** Cached branch node this represents */
	TObjectPtr<UGraphNodeDialogueBranch> BranchNode;

	/** Constants */
	const FMargin CONDITION_BOX_PADDING = FMargin(10.f, 7.f, 10.f, 7.f);
	const ETextJustify::Type PIN_LABEL_JUSTIFY = ETextJustify::Center;
	const float OUTPUT_PIN_PUSH_AMOUNT = -55.f;
};

