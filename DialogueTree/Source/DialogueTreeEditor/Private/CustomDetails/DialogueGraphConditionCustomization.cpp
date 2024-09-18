// Copyright Zachary Brett, 2024. All rights reserved.

#include "CustomDetails/DialogueGraphConditionCustomization.h"
#include "Conditionals/DialogueCondition.h"
#include "Conditionals/Queries/Base/DialogueQuery.h"
#include "DetailWidgetRow.h"
#include "Dialogue.h"
#include "Graph/DialogueEdGraph.h"
#include "Graph/DialogueGraphCondition.h"
#include "Graph/Nodes/GraphNodeDialogue.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"

#define LOCTEXT_NAMESPACE "DialogueGraphConditionCustomization"

void FDialogueGraphConditionCustomization::CustomizeHeader(
	TSharedRef<class IPropertyHandle> PropertyHandle, 
	FDetailWidgetRow& HeaderRow, 
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	//Only deal with one obj at a time
	if (StructCustomizationUtils.GetPropertyUtilities()
		->GetSelectedObjects().Num() > 1)
	{
		return;
	}

	CachePrerequisites(PropertyHandle, StructCustomizationUtils);

	//If missing customization prereqs, create default
	if (!HasPrerequisites())
	{
		CreateDefaultHeader(PropertyHandle, HeaderRow);
		return;
	}

	CreateHeaderDisplayText(PropertyHandle, HeaderRow);
}

void FDialogueGraphConditionCustomization::CustomizeChildren(
	TSharedRef<class IPropertyHandle> PropertyHandle, 
	IDetailChildrenBuilder& ChildBuilder, 
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	//Only deal with one obj at a time
	if (StructCustomizationUtils.GetPropertyUtilities()
		->GetSelectedObjects().Num() > 1)
	{
		return;
	}

	//If missing customization prereqs, create default
	if (!HasPrerequisites())
	{
		CreateDefaultChildren(PropertyHandle, ChildBuilder);
	}

	CreateChildPropertyRows(PropertyHandle, ChildBuilder);

	
}

TSharedRef<IPropertyTypeCustomization> 
FDialogueGraphConditionCustomization::MakeInstance()
{
	return MakeShareable(new FDialogueGraphConditionCustomization);
}

void FDialogueGraphConditionCustomization::CachePrerequisites(
	TSharedRef<class IPropertyHandle> PropertyHandle, 
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	//Get condition
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	//Retrieve graph condition
	UObject* PropertyValue;
	if (PropertyHandle->GetValue(PropertyValue) == FPropertyAccess::Fail 
		|| !PropertyValue)
	{
		//No valid condition, spawn one
		SpawnCondition(PropertyHandle);
		return;
	}

	UDialogueGraphCondition* ValueAsCondition =
		Cast<UDialogueGraphCondition>(PropertyValue);

	if (!ValueAsCondition)
	{
		return;
	}

	TargetGraphCondition = ValueAsCondition;

	//Get graph and node
	Utils = CustomizationUtils.GetPropertyUtilities();
	if (!Utils.IsValid())
	{
		return;
	}

	TArray<TWeakObjectPtr<UObject>> SelectedObjects = 
		Utils->GetSelectedObjects();

	if (SelectedObjects.IsEmpty())
	{
		return;
	}

	UObject* FirstSelected = SelectedObjects[0].Get();
	TargetGraphNode = Cast<UGraphNodeDialogue>(FirstSelected);
	
	if (!TargetGraphNode.IsValid())
	{
		return;
	}

	TargetGraph = TargetGraphNode->GetDialogueGraph();
}

bool FDialogueGraphConditionCustomization::HasPrerequisites()
{
	return TargetGraphCondition.IsValid();
}

void FDialogueGraphConditionCustomization::CreateDefaultHeader(
	TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow)
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

void FDialogueGraphConditionCustomization::CreateDefaultChildren(
	TSharedRef<IPropertyHandle> PropertyHandle, 
	IDetailChildrenBuilder& ChildBuilder)
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

void FDialogueGraphConditionCustomization::SpawnCondition(TSharedRef<class IPropertyHandle> PropertyHandle)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	//Get outer
	TArray<UObject*> OuterObjects;
	PropertyHandle->GetOuterObjects(OuterObjects);

	if (OuterObjects.IsEmpty())
	{
		return;
	}

	UDialogue* TargetDialogue = OuterObjects[0]->GetTypedOuter<UDialogue>();

	if (!TargetDialogue)
	{
		return;
	}

	//Make condition
	UDialogueGraphCondition* NewCondition = 
		NewObject<UDialogueGraphCondition>(TargetDialogue);

	//Cache condition
	FPropertyAccess::Result SetResult = PropertyHandle->SetValue(NewCondition);
	check(SetResult == FPropertyAccess::Success);
	TargetGraphCondition = NewCondition;
	check(TargetGraphCondition.IsValid());
}

void FDialogueGraphConditionCustomization::CreateHeaderDisplayText(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow)
{
	if (!PropertyHandle->IsValidHandle() || !TargetGraphCondition.IsValid())
	{
		return;
	}

	//Get the display text
	FText DisplayText = LOCTEXT("DisplayText", "Invalid Condition");

	UDialogueQuery* OwnedQuery = TargetGraphCondition->GetQuery();
	UDialogueCondition* OwnedCondition = TargetGraphCondition->GetCondition();

	if (OwnedQuery && OwnedCondition && OwnedCondition->IsValidCondition())
	{
		FText QueryText = OwnedQuery->GetGraphDescription();
		FText ConditionText = OwnedCondition->GetGraphDescription(QueryText);

		if (!ConditionText.IsEmptyOrWhitespace())
		{
			DisplayText = ConditionText;
		}
	}

	//Create Name Section
	HeaderRow.NameContent()
	[
		SNew(SSpacer)
		.Size(FVector2D(5.f, 0.f))
	];

	//Create Value Section
	HeaderRow.ValueContent()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Fill)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(DisplayText)
			.Justification(ETextJustify::Left)
		]
	];
}

void FDialogueGraphConditionCustomization::CreateChildPropertyRows(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder)
{
	if (!PropertyHandle->IsValidHandle())
	{
		return;
	}

	if (!TargetGraphCondition.IsValid())
	{
		return;
	}

	TArray<UObject*> TargetGraphConditionArray;
	TargetGraphConditionArray.Add(TargetGraphCondition.Get());

	//Add query field 
	FAddPropertyParams QueryParams;
	QueryParams.AllowChildren(false);
	IDetailPropertyRow* QueryRow = ChildBuilder.AddExternalObjectProperty(
		TargetGraphConditionArray,
		GET_MEMBER_NAME_CHECKED(UDialogueGraphCondition, Query),
		QueryParams
	);

	InitNewPropertyRow(QueryRow);

	//Add any condition fields
	UDialogueCondition* OwnedCondition = TargetGraphCondition->GetCondition();

	if (!OwnedCondition)
	{
		return;
	}

	TArray<UObject*> OwnedConditionArray;
	OwnedConditionArray.Add(OwnedCondition);

	for (TFieldIterator<FProperty> ConditionPropertyIt(
		OwnedCondition->GetClass()); 
		ConditionPropertyIt; 
		++ConditionPropertyIt)
	{
		bool bEditable = ConditionPropertyIt->HasAnyPropertyFlags(
			EPropertyFlags::CPF_Edit
		);

		//Ignore properties that don't allow editing in editor
		if (bEditable)
		{
			//Get property name 
			FName PropertyName = ConditionPropertyIt->GetFName();
		
			//Add property
			FAddPropertyParams ConditionParams;
			ConditionParams.AllowChildren(false);
			IDetailPropertyRow* ConditionPropertyRow =
				ChildBuilder.AddExternalObjectProperty(
					OwnedConditionArray,
					PropertyName,
					ConditionParams
				);

			//If a property was added
			InitNewPropertyRow(ConditionPropertyRow);
		}
	}
}

void FDialogueGraphConditionCustomization::RefreshEditor()
{
	//Refresh the details view
	if (Utils.IsValid())
	{
		Utils->ForceRefresh();
	}

	//Refresh the owning condition 
	if (TargetGraphCondition.IsValid()
		&& TargetGraphCondition->ShouldRefreshCondition())
	{
		TargetGraphCondition->RefreshCondition();
	}

	//Refresh the graph node 
	if (TargetGraphNode.IsValid())
	{
		TargetGraphNode->UpdateDialogueNode();
	}
}

void FDialogueGraphConditionCustomization::InitNewPropertyRow(IDetailPropertyRow* NewRow)
{
	if (NewRow)
	{
		TSharedPtr<IPropertyHandle> NewHandle =
			NewRow->GetPropertyHandle();

		if (NewHandle.IsValid() && NewHandle->IsValidHandle())
		{
			NewHandle->SetOnPropertyValueChanged(
				FSimpleDelegate::CreateSP(
					this,
				&FDialogueGraphConditionCustomization::RefreshEditor
				)
			);
			NewHandle->SetOnChildPropertyValueChanged(
				FSimpleDelegate::CreateSP(
					this, &FDialogueGraphConditionCustomization::RefreshEditor
				)
			);
		}
	}
}

#undef LOCTEXT_NAMESPACE