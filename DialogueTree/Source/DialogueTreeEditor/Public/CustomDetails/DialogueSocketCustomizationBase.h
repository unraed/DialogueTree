// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once
#include "IPropertyTypeCustomization.h"

class UDialogueEdGraph;
class UGraphNodeDialogue;
class SDialogueObjectPicker;

/**
* Property customization for dialogue objects that we want to be able to 
* select from the graph via a custom picker.
*/
class FDialogueSocketCustomizationBase : public IPropertyTypeCustomization
{
public:
	/** IPropertyTypeCustomization Impl. */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
		FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void CustomizeChildren(
		TSharedRef<IPropertyHandle> Property,
		IDetailChildrenBuilder& ChildBuilder,
		IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	/** End IPropertyTypeCustomization */

protected:
	/**
	* Caches prerequisites for customization. 
	* 
	* @param PropertyHandle - TSharedRef<IPropertyHandle>, the property handle
	* being customized.
	*/
	virtual void CachePrerequisites(
		TSharedRef<IPropertyHandle> PropertyHandle) = 0;

	/**
	* Gets the display text for the currently selected object.
	*
	* @return FText - the display text. 
	*/
	virtual FText GetSelectionText() const = 0;

	/**
	* Gets the available options for the user to pick between. 
	*
	* @return TArray<UObject*> - the objects the user can pick from. 
	*/
	virtual TArray<UObject*> GetOptions() const = 0;

	/**
	* Gets the FName ID of the specified option object. 
	* 
	* @param InObj - UObject*, the option object. 
	* @return FName - ID of the object. 
	*/
	virtual FName GetOptionName(UObject* InObj) const = 0;

	/**
	* Retrieves the tooltip for the picker's combo button. 
	* 
	* @return FText - the tooltip. 
	*/
	virtual FText GetButtonTooltip() const = 0;

	/**
	* Sets up the event to refresh the graph on changing a node property.
	*
	* @param PropertyHandle - TSharedRef<IPropertyHandle>, the base property
	* being customized.
	*/
	virtual void SetUpPropertyChangeEvent(
		TSharedRef<IPropertyHandle> PropertyHandle) = 0;

	/**
	* Function that sets the selection field. 
	* 
	* @param InSelection - UObject*, the newly selected object. 
	*/
	virtual void SetSelection(UObject* InSelection) = 0;

	/**
	* Checks if the necessary objects for customization have been cached.
	*
	* @return bool - True if the required objects have been found; False
	* otherwise.
	*/
	virtual bool HasPrerequisites() const = 0;

protected:
	/**
	* Caches the objects necessary to customize the property as desired. 
	* 
	* @param PropertyHandle - TSharedRef<IPropertyHandle>, the base property 
	* being customized.
	* @param Utils - TSharedPtr<IPropertyUtilities>, the utilities object. 
	*/
	void GetTargetObjects(TSharedRef<IPropertyHandle> PropertyHandle,
		TSharedPtr<IPropertyUtilities> Utilities);

	/**
	* Creates the default header for the property as if it had never been 
	* customized. 
	*
	* @param PropertyHandle - TSharedRef<IPropertyHandle>, the base property
	* being customized.
	* @param HeaderRow - FDetailWidgetRow&, the header row. 
	*/
	void CreateDefaultHeader(TSharedRef<IPropertyHandle> PropertyHandle,
		FDetailWidgetRow& HeaderRow);

	/**
	* Creates the default rows for any property children as if the property 
	* had never been customized. 
	*
	* @param PropertyHandle - TSharedRef<IPropertyHandle>, the base property
	* being customized.
	* @param ChildBuilder - IDetailChildrenBuilder&, the builder for the child
	* property widgets. 
	*/
	void CreateDefaultChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder);

	/**
	* Creates the custom property row. 
	*
	* @param PropertyHandle - TSharedRef<IPropertyHandle>, the base property
	* being customized.
	* @param ChildBuilder - IDetailChildrenBuilder&, the builder for the child
	* property widgets. 
	*/
	void CreatePropertyRow(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder);

	/**
	* Creates the combo button used to open the picker.
	*/
	void CreateComboButton();

	/**
	* Creates the picker widget.
	*/
	void CreatePicker();

	/**
	* Opens/retrieves the picker widget.
	*/
	TSharedRef<SWidget> OpenPicker() const;

	/**
	* Handles opening and closing of the picker widget.
	* 
	* @param bOpen - bool, whether the widget is now open. 
	*/
	void OnMenuOpenChanged(bool bOpen);

	/**
	* Refreshes the graph view on changing values. 
	*/
	void RefreshEditor();

protected:
	/** The graph under consideration*/
	TWeakObjectPtr<UDialogueEdGraph> TargetGraph;

	/** The graph node under consideration */
	TWeakObjectPtr<UGraphNodeDialogue> TargetGraphNode;

	/** Utils */
	TSharedPtr<IPropertyUtilities> Utils;

	/** The slate widget used by the user to pick the target object */
	TSharedPtr<SDialogueObjectPicker> Picker;

	/** The text asssigned to the button which opens the picker */
	TSharedPtr<STextBlock> ComboButtonText;

	/** The button which opens the picker */
	TSharedPtr<SComboButton> ComboButton;
};