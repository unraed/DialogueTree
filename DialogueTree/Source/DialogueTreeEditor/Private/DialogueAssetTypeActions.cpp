// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "DialogueAssetTypeActions.h"
//Plugin
#include "Dialogue.h"
#include "DialogueEditor.h"

#define LOCTEXT_NAMESPACE "DialogueAssetTypeActions"

FDialogueAssetTypeActions::FDialogueAssetTypeActions(
	EAssetTypeCategories::Type InCategory)
	: AssetCategory(InCategory)
{
}

FText FDialogueAssetTypeActions::GetName() const
{
	return LOCTEXT("AssetTypeName", "Dialogue Tree");
}

UClass* FDialogueAssetTypeActions::GetSupportedClass() const
{
	return UDialogue::StaticClass();
}

FColor FDialogueAssetTypeActions::GetTypeColor() const
{
	return FColor::Green;
}

uint32 FDialogueAssetTypeActions::GetCategories()
{
	return AssetCategory;
}

void FDialogueAssetTypeActions::OpenAssetEditor(
	const TArray<UObject*>& InObjects, 
	TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	//Get the toolkit mode
	const EToolkitMode::Type Mode = 
		EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric
		: EToolkitMode::Standalone;

	//Attempt to open an asset editor for the object
	for (UObject* Object : InObjects)
	{
		UDialogue* TargetDialogue = Cast<UDialogue>(Object);
		if (TargetDialogue != nullptr)
		{
			TSharedRef<FDialogueEditor> TargetEditor(new FDialogueEditor());
			TargetEditor->InitEditor(
				Mode, 
				EditWithinLevelEditor, 
				TargetDialogue
			);
		}
	}
}

#undef LOCTEXT_NAMESPACE