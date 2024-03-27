// Copyright Zachary Brett, 2024. All rights reserved.
//Header
#include "SDialogueObjectPicker.h"
//UE
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "SDialogueObjectPicker"

void SDialogueObjectPicker::Construct(const FArguments& InArgs)
{
	//Validate args
	check(InArgs._Collection.IsEmpty() == false
		&& InArgs._ParentButton.IsValid()
		&& InArgs._NameGetter.IsBound()
	);

	//Set up the collection 
	InitCollection(InArgs._Collection, InArgs._NameGetter);

	//Set up the parent button
	ParentButton = InArgs._ParentButton;
	check(ParentButton.IsValid());

	//Set up filtered names array 
	InitFilteredNames();

	//Build up the picker widget 
	BuildPicker();

	//Set focus for search bar 
	RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(
		this, 
		&SDialogueObjectPicker::SetFocusPostConstruct
	));
}

void SDialogueObjectPicker::InitCollection(const TArray<UObject*>& InCollection, FGetObjectName NameGetter)
{
	Collection.Empty();
	bool bNameGetterBound = NameGetter.IsBound();

	for (UObject* Obj : InCollection)
	{
		//If no valid getter provided, use standard GetFName()
		FName ObjName = bNameGetterBound ?
			NameGetter.Execute(Obj) : Obj->GetFName();

		Collection.Add(ObjName, Obj);
	}
}

void SDialogueObjectPicker::InitFilteredNames()
{
	//Place all object names in collection into filter list
	TSet<FName> NamesSet;
	Collection.GetKeys(NamesSet);
	FilteredNames = NamesSet.Array();

	//Sort names alphabetically
	FilteredNames.Sort(
		[](FName Name1, FName Name2)
		{
			return Name1.LexicalLess(Name2);
		}
	);
}

void SDialogueObjectPicker::BuildPicker()
{
	//Set up main box to build off
	TSharedRef<SVerticalBox> MainBox = SNew(SVerticalBox);
	ChildSlot
	[
		MainBox
	];

	//Add search box 
	AddSearchBox(MainBox);

	//Add options box
	AddOptionsWidget(MainBox);
}

void SDialogueObjectPicker::AddSearchBox(TSharedRef<SVerticalBox> MainBox)
{
	//Delegate to call for filtering when text changes
	FOnTextChanged OnTextChanged;
	OnTextChanged.BindSP(this, &SDialogueObjectPicker::FilterCollection);

	//Add the search box to the main widget
	MainBox->AddSlot()
	.AutoHeight()
	[
		SAssignNew(SearchBox, SSearchBox)
		.OnTextChanged(OnTextChanged)
	];
}

void SDialogueObjectPicker::AddOptionsWidget(TSharedRef<SVerticalBox> MainBox)
{
	//Add the widget 
	MainBox->AddSlot()
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
		[
			SNew(SBox)
			.MaxDesiredHeight(MAX_PICKER_HEIGHT)
			[
				SAssignNew(OptionsBox, SScrollBox)
			]
		]
	];

	//Fill options 
	RefreshOptionsWidget();
}

void SDialogueObjectPicker::RefreshOptionsWidget()
{
	check(OptionsBox.IsValid());

	//Empty the box
	OptionsBox->ClearChildren();

	//If no filtered names, present a default entry 
	if (FilteredNames.IsEmpty())
	{
		OptionsBox->AddSlot()
		.Padding(0.f, OPTIONS_BOX_Y_PADDING)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("EmptyOptionsText", "No valid items"))
			.Font(GetFontStyle())
			.Justification(TEXT_JUSTIFY)
		];

		return;
	}

	OptionsBox->AddSlot()
	[
		SNew(SSpacer)
		.Size(FVector2D(OPTION_PADDING))
	];

	//Fill the box with option buttons 
	for (FName OptionName : FilteredNames)
	{
		OptionsBox->AddSlot()
		.Padding(OPTION_PADDING, OPTION_PADDING / 2.f)
		[
			CreateOptionButton(OptionName)
		];
	}
}

void SDialogueObjectPicker::FilterCollection(const FText& SearchText)
{
	FilteredNames.Empty();

	if (SearchText.IsEmptyOrWhitespace())
	{
		Collection.GetKeys(FilteredNames);
		RefreshOptionsWidget();
		return;
	}

	FString SearchString = SearchText.ToString();
	SearchString.RemoveSpacesInline();

	TArray<FName> Options;
	Collection.GetKeys(Options);

	for (FName Option : Options)
	{
		FString OptionString = Option.ToString();
		OptionString.RemoveSpacesInline();

		if (OptionString.Contains(SearchString))
		{
			FilteredNames.Add(Option);
		}
	}

	RefreshOptionsWidget();
}

FReply SDialogueObjectPicker::BroadcastSelectedOption(FName SelectionName)
{
	//Broadcast the selected option
	check(Collection.Contains(SelectionName));
	UObject* NewValue = Collection.FindChecked(SelectionName);

	OnSelectOption.ExecuteIfBound(NewValue);

	//Close the picker
	if (ParentButton.IsValid())
	{
		ParentButton->SetIsOpen(false);
	}

	return FReply::Handled();
}

EActiveTimerReturnType SDialogueObjectPicker::SetFocusPostConstruct(
	double InCurrentTime, float InDeltaTime)
{
	if (SearchBox.IsValid())
	{
		FWidgetPath WidgetToFocusPath;
		FSlateApplication::Get().GeneratePathToWidgetUnchecked(
			SearchBox.ToSharedRef(), WidgetToFocusPath
		);
		FSlateApplication::Get().SetKeyboardFocus(
			WidgetToFocusPath, 
			EFocusCause::SetDirectly
		);
		WidgetToFocusPath.GetWindow()->SetWidgetToFocusOnActivate(SearchBox);

		return EActiveTimerReturnType::Stop;
	}

	return EActiveTimerReturnType::Continue;
}

FSlateFontInfo SDialogueObjectPicker::GetFontStyle() const
{
	return FAppStyle::GetFontStyle(
		"PropertyWindow.NormalFont"
	);
}

TSharedRef<SButton> SDialogueObjectPicker::CreateOptionButton(
	FName& InName) 
{
	//Behavior for clicking button 
	FOnClicked OnButtonClick;
	OnButtonClick.BindSP(
		this,
		&SDialogueObjectPicker::BroadcastSelectedOption,
		InName
	);

	//Create button for option 
	return SNew(SButton)
		.Text(FText::FromName(InName))
		.OnClicked(OnButtonClick);
}

void SDialogueObjectPicker::ClearSearchText()
{
	if (SearchBox.IsValid())
	{
		SearchBox->SetText(FText());
	}
}

void SDialogueObjectPicker::RefocusSearchBox()
{
	RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(
		this,
		&SDialogueObjectPicker::SetFocusPostConstruct
	));
}

#undef LOCTEXT_NAMESPACE