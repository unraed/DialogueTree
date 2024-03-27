// Copyright Zachary Brett, 2024. All rights reserved.

#include "CustomDetails/DialogueGraphCustomization.h"
#include "Dialogue.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "Graph/DialogueEdGraph.h"

void FDialogueGraphCustomization::CustomizeDetails(
	IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> TargetObjects;
	DetailBuilder.GetObjectsBeingCustomized(TargetObjects);
	if (TargetObjects.IsEmpty() || !TargetObjects[0].IsValid())
	{
		return;
	}

	UDialogueEdGraph* TargetGraph =
		Cast<UDialogueEdGraph>(TargetObjects[0].Get());
	if (!TargetGraph)
	{
		return;
	}

	UDialogue* TargetDialogue = TargetGraph->GetDialogue();
	if (!TargetDialogue)
	{
		return;
	}

	IDetailCategoryBuilder& CategoryBuilder = 
		DetailBuilder.EditCategory("Dialogue");

	FAddPropertyParams AddParams;
	AddParams.HideRootObjectNode(true);

	CategoryBuilder.AddExternalObjects(
		TArray<UObject*>{TargetDialogue},
		EPropertyLocation::Default,
		AddParams
	);
}

TSharedRef<IDetailCustomization> FDialogueGraphCustomization::MakeInstance()
{
	return MakeShareable(new FDialogueGraphCustomization);
}
