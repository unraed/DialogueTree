// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "SGraphNodeDialogueBase.h"

class UGraphNodeDialogueEvent;

/**
* Governs the visual representation of an event node in the dialogue graph. 
*/
class DIALOGUETREEEDITOR_API SGraphNodeDialogueEvent : 
	public SGraphNodeDialogueBase
{
public:
	//Slate Arguments 
	SLATE_BEGIN_ARGS(SGraphNodeDialogueEvent) {}
	SLATE_END_ARGS()

	/** Slate Constructor */
	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);

protected:
	/** SGraphNode Implementation */
	virtual TSharedRef<SWidget> CreateNodeContentArea() override;
	/** End SGraphNode */

private:
	/**
	* Assembles the full display text widget for the node's events. 
	* 
	* @param InTexts - TArray<FText>&, the text associated with each event. 
	* @return TSharedRef<SWidget> - the built widget. 
	*/
	TSharedRef<SWidget> GetDisplayTextBox(TArray<FText>& InTexts) const;

private:
	/** Cached event node this represents */
	TObjectPtr<UGraphNodeDialogueEvent> EventNode;

	/** Constants */
	const FMargin EVENT_TEXT_PADDING = FMargin(10.f, 7.f, 10.f, 7.f);
};