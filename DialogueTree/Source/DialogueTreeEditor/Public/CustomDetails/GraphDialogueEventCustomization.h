// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//Plugin
#include "IPropertyTypeCustomization.h"

class UDialogueEventBase;

/**
* Customizes graph conditions to facilitate query based condition creation.
*/
class FGraphDialogueEventCustomization : public IPropertyTypeCustomization
{
public:
	/** IPropertyTypeCustomization Impl. */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
		FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void CustomizeChildren(
		TSharedRef<IPropertyHandle> PropertyHandle,
		IDetailChildrenBuilder& ChildBuilder,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	/** End IPropertyTypeCustomization */

	/**
	* Static method used to instantiate the customization.
	*
	* @return TSharedRef<IPropertyTypeCustomization> - the instance.
	*/
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

private:
	UDialogueEventBase* GetTargetEvent(TSharedRef<IPropertyHandle> PropertyHandle);
};