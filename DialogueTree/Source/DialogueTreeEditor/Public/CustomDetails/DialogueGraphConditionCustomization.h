// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "IPropertyTypeCustomization.h"

class UDialogueEdGraph;
class UDialogueGraphCondition;
class UGraphNodeDialogue;
class IDetailPropertyRow;
class IPropertyHandle;

/**
* Customizes graph conditions to facilitate query based condition creation. 
*/
class FDialogueGraphConditionCustomization : public IPropertyTypeCustomization
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
	/**
	* Stores prereqs for the customization. 
	* 
	* @param PropertyHandle - TSharedRef<IPropertyHandle>, the primary property
	* handle. 
	* @param CustomizationUtils - IPropertyTypeCustomizationUtils&, the 
	* customization utilities. 
	*/
	void CachePrerequisites(
		TSharedRef<IPropertyHandle> PropertyHandle, 
		IPropertyTypeCustomizationUtils& CustomizationUtils
	);

	/**
	* Whether the necessary prereqs for customization have been cached. 
	* 
	* @return bool - True if the prereqs are present; False otherwise. 
	*/
	bool HasPrerequisites();

	/**
	* Creates the default header row. For use when the prerequisites are not 
	* found. 
	* 
	* @param PropertyHandle - TSharedRef<IPropertyHandle>, the primary property
	* handle. 
	* @param HeaderRow - FDetailWidgetRow&, the header row to build. 
	*/
	void CreateDefaultHeader(TSharedRef<IPropertyHandle> PropertyHandle,
		FDetailWidgetRow& HeaderRow);

	/**
	* Creates the default child property rows. For use when the prereqs are not
	* found. 
	* 
	* @param PropertyHandle - TSharedRef<IPropertyHandle>, the primary property
	* being customized. 
	* @param ChildBuilder - IDetailChildrenBuilder&, the builder for the child
	* properties. 
	*/
	void CreateDefaultChildren(TSharedRef<IPropertyHandle> PropertyHandle,
		IDetailChildrenBuilder& ChildBuilder);

	/**
	* Spawns a condition into the property. 
	* 
	* @param PropertyHandle - TSharedRef<IPropertyHandle>, the property being
	* customized. 
	*/
	void SpawnCondition(TSharedRef<IPropertyHandle> PropertyHandle);

	/**
	* Creates the desired header display text for the condition. 
	* 
	* @param PropertyHandle - TSharedRef<IPropertyHandle>, the property being 
	* customized. 
	* @param HedaderRow - IDetailWidgetRow&, the header row to customize. 
	*/
	void CreateHeaderDisplayText(TSharedRef<IPropertyHandle> PropertyHandle,
		FDetailWidgetRow& HeaderRow);

	/**
	* Creates the desired child property rows for the condition.
	*
	* @param PropertyHandle - TSharedRef<IPropertyHandle>, the property being
	* customized.
	* @param ChildBuilder - IDetailChildrenBuilder&, the builder for the child
	* properties. 
	*/
	void CreateChildPropertyRows(TSharedRef<IPropertyHandle> PropertyHandle,
		IDetailChildrenBuilder& ChildBuilder);

	/**
	* Refreshes the details view/panel. 
	*/
	void RefreshEditor();

	/**
	* Sets up a new property row. Does nothing if the NewRow pointer is null. 
	* 
	* @param NewRow - IDetailPropertyRow*, the new row that needs to be set up. 
	*/
	void InitNewPropertyRow(IDetailPropertyRow* NewRow);

private:
	/** The condition we are customizing */
	TWeakObjectPtr<UDialogueGraphCondition> TargetGraphCondition;

	/** The graph the selected node is a part of */
	TWeakObjectPtr<UDialogueEdGraph> TargetGraph;

	/** The node that owns the condition */
	TWeakObjectPtr<UGraphNodeDialogue> TargetGraphNode;

	/** Utils */
	TSharedPtr<IPropertyUtilities> Utils;
};