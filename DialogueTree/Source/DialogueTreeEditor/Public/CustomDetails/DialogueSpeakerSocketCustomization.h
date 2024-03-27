// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once
#include "CustomDetails/DialogueSocketCustomizationBase.h"

/**
* Property customization for the Dialogue Speaker Socket which allows for
* a speaker to be selected from the graph.
*/
class FDialogueSpeakerSocketCustomization : 
	public FDialogueSocketCustomizationBase
{
public:
	/**
	* Static method used to instantiate the customization.
	*
	* @return TSharedRef<IPropertyTypeCustomization> - the instance.
	*/
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

protected:
	/** FDialogueSocketCustomizationBase Impl. */
	virtual void CachePrerequisites(
		TSharedRef<IPropertyHandle> PropertyHandle) override;
	virtual FText GetSelectionText() const override;
	virtual TArray<UObject*> GetOptions() const override;
	virtual FName GetOptionName(UObject* InObj) const override;
	virtual FText GetButtonTooltip() const override;
	virtual void SetUpPropertyChangeEvent(
		TSharedRef<IPropertyHandle> PropertyHandle) override;
	virtual void SetSelection(UObject* InSelection) override;
	virtual bool HasPrerequisites() const override;
	/** End FDialogueSocketCustomizationBase */

private:
	/** Handle to the property being picked */
	TSharedPtr<IPropertyHandle> TargetProperty;
};