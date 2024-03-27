// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Slate/SDialogueGraphEditor.h"
//UE
#include "SGraphPin.h"
#include "SNodePanel.h"

void SDialogueGraphEditor::Construct(const FArguments& InArgs)
{
	SGraphEditor::FArguments Args;
	Args._AdditionalCommands = InArgs._AdditionalCommands;
	Args._IsEditable = InArgs._IsEditable;
	Args._DisplayAsReadOnly = InArgs._DisplayAsReadOnly;
	Args._IsEmpty = InArgs._IsEmpty;
	Args._GraphToEdit = InArgs._GraphToEdit;
	Args._DiffResults = InArgs._DiffResults;
	Args._AutoExpandActionMenu = InArgs._AutoExpandActionMenu;
	Args._ShowGraphStateOverlay = InArgs._ShowGraphStateOverlay;

	SGraphEditor::Construct(Args);
}


