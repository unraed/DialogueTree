// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SWidget.h"

class SComboButton;
class SScrollBox;

DECLARE_DELEGATE_RetVal_OneParam(FName, FGetObjectName, UObject*);
DECLARE_DELEGATE_OneParam(FOnPickerSelect, UObject*);

class SDialogueObjectPicker : public SCompoundWidget
{
public:
	/** Slate Arguments */
	SLATE_BEGIN_ARGS(SDialogueObjectPicker) {}
	SLATE_ARGUMENT(TSharedPtr<SComboButton>, ParentButton)
	SLATE_ARGUMENT(TArray<UObject*>, Collection)
	SLATE_ARGUMENT(FGetObjectName, NameGetter)
	SLATE_END_ARGS()

	/** Slate Constructor */
	void Construct( const FArguments& InArgs );

public:
	/**
	* Clears the search box text. 
	*/
	void ClearSearchText();

	/**
	* Refocuses the search box when reopening the picker. 
	*/
	void RefocusSearchBox();

private:
	/**
	* Sets up the collection in a useable form. 
	* 
	* @param InCollection - TArray<UObject*>&, the objects to 
	* pick from. 
	* @param NameGetter - FGetObjectName, a delegate for a function 
	* that takes a UObject* and returns an FName to associate that 
	* object with in the picker. 
	*/
	void InitCollection(const TArray<UObject*>& InCollection, 
		FGetObjectName NameGetter);

	/**
	* Sets up the array of filtered names. 
	*/
	void InitFilteredNames();

	/**
	* Builds the picker widget itself. 
	*/
	void BuildPicker();

	/**
	* Builds the search box. 
	* 
	* @param MainBox - TSharedRef<SVerticalBox>, the box to add the
	* search box widget to. 
	*/
	void AddSearchBox(TSharedRef<SVerticalBox> MainBox);

	/**
	* Builds the options box. 
	* 
	* @param MainBox - TSharedRef<SVerticalBox>, the box to add the
	* options widget to. 
	*/
	void AddOptionsWidget(TSharedRef<SVerticalBox> MainBox);

	/**
	* Refreshes the option buttons to display to the user. 
	*/
	void RefreshOptionsWidget();

	/**
	* Filters the collection based on the provided search text. 
	* 
	* @param SearchText - FText&, the filter text. 
	*/
	void FilterCollection(const FText& SearchText);

	/**
	* Sets the value of the target property to the object associated
	* with the selected name. 
	* 
	* @param SelectionName - FName, the name of the selected object.
	* @return FReply - Handled 
	*/
	FReply BroadcastSelectedOption(FName SelectionName);

	/**
	* Focuses the search box after constructing this object so that
	* the user can begin typing immediately. Borrowed almost 
	* entirely from SAssetPicker.cpp.
	* 
	* @param InCurrentTime - double, the current time.
	* @param InDeltaTime = float, the current delta time.
	* @return EActiveTimerReturnType - timer return. 
	*/
	EActiveTimerReturnType SetFocusPostConstruct(double InCurrentTime, 
		float InDeltaTime);

	/**
	* Gets the font style for the picker. 
	* 
	* @return FSlateFontInfo - the font style for the picker. 
	*/
	FSlateFontInfo GetFontStyle() const;

	/**
	* Creates an option button associated with a given pickable 
	* object. 
	* 
	* @parm InName - FName&, name of the object associated with the 
	* new button. 
	* @return TSharedRef<SButton> - the newly created button. 
	*/
	TSharedRef<SButton> CreateOptionButton(FName& InName);

public:
	/** Delegate to call on selecting an option */
	FOnPickerSelect OnSelectOption;

private: 
	/** The items being picked from */
	TMap<FName, UObject*> Collection;

	/** A list of item names, filtered */
	TArray<FName> FilteredNames;

	/** The combo button that opens this picker */
	TSharedPtr<SComboButton> ParentButton;

	/** Where the user enters search text */
	TSharedPtr<SSearchBox> SearchBox;

	/** The display for options to appear in */
	TSharedPtr<SScrollBox> OptionsBox;

	/** Constants */
	const float MAX_PICKER_HEIGHT = 150.f;
	const float OPTIONS_BOX_Y_PADDING = 20.f;
	const float OPTION_PADDING = 2.f;
	const ETextJustify::Type TEXT_JUSTIFY = ETextJustify::Center;
};