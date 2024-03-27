// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "SGraphPin.h"

/**
* Base functionality for defining pin visuals in the dialogue graph. 
*/
class SDialoguePinBase : public SGraphPin
{
public:
	//Slate Arguments 
	SLATE_BEGIN_ARGS(SDialoguePinBase) {}
	SLATE_ARGUMENT(FSlateColor, PinColor)
	SLATE_END_ARGS()

	/** Slate Constructor */
	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:
	/** SGraphPin Implementation */
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	virtual const FSlateBrush* GetPinIcon() const override;
	/** End SGraphPin */

	/**
	* Sets up the icon representing the pin. 
	*/
	virtual void CachePinIcons() {};

protected:
	/** Icon representing the pin appearance */
	const FSlateBrush* DialoguePinImage;
};

/**
* Governs visual representation of a standard pin in the dialogue graph. 
*/
class SDialogueStandardPin : public SDialoguePinBase
{
public:
	//Slate Arguments 
	SLATE_BEGIN_ARGS(SDialogueStandardPin) {}
	SLATE_END_ARGS()

	/** Slate Constructor */
	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected: 
	/** SDialoguePinBase Implementation */
	virtual void CachePinIcons() override;
	/** End SDialoguePinBase */ 
};

/**
* Governs the visual representation of an exit pin in the dialogue graph. 
*/
class SDialogueExitPin : public SDialoguePinBase
{
public:
	// Slate Arguments 
	SLATE_BEGIN_ARGS(SDialogueStandardPin) {}
	SLATE_END_ARGS()

	/** Slate Constructor */
	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:
	/** SDialoguePinBase Implementation */
	virtual void CachePinIcons() override;
	/** End SDialoguePinBase */
};