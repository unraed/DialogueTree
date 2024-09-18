// Copyright Zachary Brett, 2024. All rights reserved.

#include "CustomDetails/DialogueSocketCustomizationBase.h"
#include "DetailWidgetRow.h"
#include "Graph/DialogueEdGraph.h"
#include "Graph/Nodes/GraphNodeDialogue.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"
#include "SDialogueObjectPicker.h"

void FDialogueSocketCustomizationBase::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	//Only deal with one obj at a time
	if (CustomizationUtils.GetPropertyUtilities()
		->GetSelectedObjects().Num() > 1)
	{
		return;
	}

	//Retrieve target objects
	GetTargetObjects(
		PropertyHandle, 
		CustomizationUtils.GetPropertyUtilities()
	);
	SetUpPropertyChangeEvent(PropertyHandle);

	//If not part of the dialogue graph, make the default widget
	if (!HasPrerequisites())
	{
		CreateDefaultHeader(PropertyHandle, HeaderRow);
		return;
	}
}

void FDialogueSocketCustomizationBase::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	//Only deal with one obj at a time
	if (CustomizationUtils.GetPropertyUtilities()
		->GetSelectedObjects().Num() > 1)
	{
		return;
	}

	//If not part of the dialogue graph, make the default widget
	if (!HasPrerequisites())
	{
		CreateDefaultChildren(PropertyHandle, ChildBuilder);
		return;
	}

	CreatePropertyRow(PropertyHandle, ChildBuilder);
	CreatePicker();
}

void FDialogueSocketCustomizationBase::GetTargetObjects(TSharedRef<IPropertyHandle> PropertyHandle, TSharedPtr<IPropertyUtilities> Utilities)
{
	if (!Utilities.IsValid() || !PropertyHandle->IsValidHandle())
	{
		return;
	}

	//Cache utils
	Utils = Utilities;

	//Get array of selected objects
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	SelectedObjects = Utils->GetSelectedObjects();

	//Convert selected objects to dialogue nodes
	if (SelectedObjects.IsEmpty() || !SelectedObjects[0].IsValid())
	{
		return;
	}
	TargetGraphNode = Cast<UGraphNodeDialogue>(
		SelectedObjects[0].Get()
	);

	//Cache the graph from the node 
	if (TargetGraphNode.IsValid())
	{
		TargetGraph = TargetGraphNode->GetDialogueGraph();
	}

	//Get the target property 
	CachePrerequisites(PropertyHandle);
}

void FDialogueSocketCustomizationBase::CreateDefaultHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	];
	HeaderRow.ValueContent()
	[
		PropertyHandle->CreatePropertyValueWidget()
	];
}

void FDialogueSocketCustomizationBase::CreateDefaultChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	uint32 NumChildren;
	if (PropertyHandle->GetNumChildren(NumChildren) == FPropertyAccess::Fail)
	{
		return;
	}

	for (uint32 i = 0; i < NumChildren; ++i)
	{
		TSharedPtr<IPropertyHandle> ChildHandle =
			PropertyHandle->GetChildHandle(i);

		if (ChildHandle.IsValid() && ChildHandle->IsValidHandle())
		{
			FDetailWidgetRow& NewRow = ChildBuilder.AddCustomRow(
				ChildHandle->GetPropertyDisplayName()
			);
			NewRow.NameContent()
			[
				ChildHandle->CreatePropertyNameWidget()
			];
			NewRow.ValueContent()
			[
				ChildHandle->CreatePropertyValueWidget()
			];
		}
	}
}

void FDialogueSocketCustomizationBase::CreatePropertyRow(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	FDetailWidgetRow& NewRow = ChildBuilder.AddCustomRow(
		PropertyHandle->GetPropertyDisplayName()
	);

	//Create combo button to open picker 
	CreateComboButton();

	//Create name widget
	NewRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	];

	//Create value widget 
	NewRow.ValueContent()
	[
		ComboButton.ToSharedRef()
	];
}

void FDialogueSocketCustomizationBase::CreateComboButton()
{
	SAssignNew(ComboButton, SComboButton)
		.ToolTipText(GetButtonTooltip())
		.OnGetMenuContent(this, &FDialogueSocketCustomizationBase::OpenPicker)
		.OnMenuOpenChanged(
			this,
			&FDialogueSocketCustomizationBase::OnMenuOpenChanged
		)
		.ButtonContent()
		[
			SAssignNew(ComboButtonText, STextBlock)
			.Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
			.Text(GetSelectionText())
		];
}

void FDialogueSocketCustomizationBase::CreatePicker()
{
	TArray<UObject*> Options = GetOptions();

	FGetObjectName NameGetter;
	NameGetter.BindSP(
		this,
		&FDialogueSocketCustomizationBase::GetOptionName
	);

	SAssignNew(Picker, SDialogueObjectPicker)
		.ParentButton(ComboButton)
		.Collection(Options)
		.NameGetter(NameGetter);

	if (Picker.IsValid())
	{
		Picker->OnSelectOption.BindSP(
			this, 
			&FDialogueSocketCustomizationBase::SetSelection
		);
	}
}

TSharedRef<SWidget> FDialogueSocketCustomizationBase::OpenPicker() const
{
	check(Picker.IsValid());
	return Picker.ToSharedRef();
}

void FDialogueSocketCustomizationBase::OnMenuOpenChanged(bool bOpen)
{
	//On close menu, update the text of the combo button
	if (!ComboButton.IsValid() || !Picker.IsValid())
	{
		return;
	}

	if (bOpen == false) //Closing
	{
		Picker->ClearSearchText();
		ComboButtonText->SetText(GetSelectionText());
	}
	else //Opening
	{
		Picker->RefocusSearchBox();
	}
}

void FDialogueSocketCustomizationBase::RefreshEditor()
{
	//Refresh the details view
	if (Utils.IsValid())
	{
		Utils->RequestRefresh();
	}

	//Refresh the graph node
	if (TargetGraphNode.IsValid())
	{
		TargetGraphNode->UpdateDialogueNode();
	}
}
