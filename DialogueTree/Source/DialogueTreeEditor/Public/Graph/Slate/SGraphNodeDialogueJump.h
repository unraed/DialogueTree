// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once 

//Plugin
#include "SGraphNodeDialogueBase.h"

class UGraphNodeDialogueJump;

/**
* Governs the visual representation of a jump node in the dialogue graph. 
*/
class DIALOGUETREEEDITOR_API SGraphNodeDialogueJump : 
	public SGraphNodeDialogueBase
{
public:
	//Slate Arguments 
	SLATE_BEGIN_ARGS(SGraphNodeDialogueJump) {}
	SLATE_END_ARGS()

	/** Slate Constructor */
	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);

protected:
	/** SGraphNode Implementation */
	virtual TSharedRef<SWidget> CreateNodeContentArea() override;
	/** End SGraphNode */

private:
	/**
	* Retrieves the display text for indicating the node's jump target. 
	* 
	* @return FText - the display text. 
	*/
	FText GetDisplayText() const; 

private:
	/** Cached Jump Node */
	TObjectPtr<UGraphNodeDialogueJump> JumpNode;

	/** Constants */
	float CONTENT_PADDING = 10.f;
};