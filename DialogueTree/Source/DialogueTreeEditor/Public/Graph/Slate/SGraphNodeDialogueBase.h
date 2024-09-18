// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once 

//UE
#include "CoreMinimal.h"
#include "SGraphNode.h"

class UEdGraphNode;
class UGraphNodeDialogue;

/**
* Base behaviors to govern how a node should display on the dialogue graph. 
*/
class DIALOGUETREEEDITOR_API SGraphNodeDialogueBase : public SGraphNode
{
public:
	// Slate Arguments 
	SLATE_BEGIN_ARGS(SGraphNodeDialogueBase) {}
	SLATE_END_ARGS()

	/** Slate Constructor*/
	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);

public:
	/** SGraphNode Public Implementation */
	virtual void UpdateGraphNode() override;
	virtual void SetOwner(const TSharedRef<SGraphPanel>& OwnerPanel) override;
	/** End SGraphNode */

protected:
	/** SGraphNode Protected Implementation */
	virtual TSharedRef<SWidget> CreateNodeContentArea() override;
	virtual TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* Pin) 
		const override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	/** End SGraphNode */
	
	/**
	* Creates the title widget for the node. Virtual. 
	* 
	* @return TSharedRef<SWidget> - The created title widget. 
	*/
	virtual TSharedRef<SWidget> CreateNodeTitleWidget();

	/**
	* Creates the header widget for the node. Virtual. By default
	* this includes the node title widget. 
	*
	* @return TSharedRef<SWidget> - The created header widget.
	*/
	virtual TSharedRef<SWidget> CreateHeaderWidget();

	/**
	* Creates the error banner widget that displays if the node has a 
	* problem compiling. Virtual. 
	* 
	* @return TSharedRef<Swidget> - The created error widget. 
	*/
	virtual TSharedRef<SWidget> CreateErrorWidget();

	/**
	* Creates the input pin area widget for the node. Virtual.
	*
	* @return TSharedRef<SWidget> - The created input pin area widget.
	*/
	virtual TSharedRef<SWidget> CreateInputPinArea();

	/**
	* Creates the output pin area widget for the node. Virtual.
	*
	* @return TSharedRef<SWidget> - The created output pin area widget.
	*/
	virtual TSharedRef<SWidget> CreateOutputPinArea();

	/**
	* Gets the input pin's default vertical distance outside of the node. 
	* Virtual. 
	* 
	* @return float - the desired distance. 
	*/
	virtual float GetInputPinYPadding() const;

	/**
	* Gets the output pin's default vertical distance outside of the node.
	* Virtual.
	*
	* @return float - the desired distance.
	*/
	virtual float GetOutputPinYPadding() const;

private:
	/**
	* Assembles the header, content area and error widget together. 
	* 
	* @return TSharedRef<SWidget> - the assembled widget. 
	*/
	TSharedRef<SWidget> AssembleNodeContent();

protected: 
	/** Cached dialogue node this is representing */
	TObjectPtr<UGraphNodeDialogue> DialogueNode;

	/** Hold Input Pins. Equivalent of SGraphNode's LeftNodeBox. */
	TSharedPtr<SHorizontalBox> InputPinBox;

	/** Holds Output Pins. Equivalent of SGraphNode's RightNodeBox. */
	TSharedPtr<SHorizontalBox> OutputPinBox;

private:
	/** Constants */
	const FVector2D DEFAULT_NODE_SIZE = FVector2D(75.f, 35.f);
	const FVector2D PIN_BOX_PADDING = FVector2D(25.f, 0.f);
	const FMargin TITLE_PADDING = FMargin(5.f, 5.f, 5.f, 2.5f);
	const float BASE_PIN_PUSH_AMOUNT = -40.f;

protected:
	/** Constants */
	const ETextJustify::Type TEXT_JUSTIFY = ETextJustify::Left;
	const int32 BASE_FONT_SIZE = 10;
	const float WRAP_TEXT_AT = 250.f;
};