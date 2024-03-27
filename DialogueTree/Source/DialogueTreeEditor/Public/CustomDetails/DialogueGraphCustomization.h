// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once 

//UE
//#include "IPropertyTypeCustomization.h"
#include "IDetailCustomization.h"

class IDetailPropertyRow;
class IPropertyHandle;
class UDialogueEdGraph;

/**
* Customizes graph properties in details panel. In particular allows dialogue
* asset's properties to be displayed as well, where appropriate. 
*/
class FDialogueGraphCustomization : public IDetailCustomization //IPropertyTypeCustomization
{
public:
	/** IDetailCustomization Impl. */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	/** End IDetailCustomization Impl. */

	static TSharedRef<IDetailCustomization> MakeInstance();
};