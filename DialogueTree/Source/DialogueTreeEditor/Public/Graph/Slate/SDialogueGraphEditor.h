// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once 

//UE
#include "GraphEditor.h"

/**
* Governs the visual representation of the dialogue graph editor. 
*/
class SDialogueGraphEditor : public SGraphEditor
{
public:
	SLATE_BEGIN_ARGS(SDialogueGraphEditor)
		: _AdditionalCommands(static_cast<FUICommandList*>(NULL))
		, _IsEditable(true)
		, _DisplayAsReadOnly(false)
		, _IsEmpty(false)
		, _GraphToEdit(NULL)
		, _DiffResults()
		, _AutoExpandActionMenu(false)
		, _ShowGraphStateOverlay(true)
	{}

	SLATE_ARGUMENT(TSharedPtr<FUICommandList>, AdditionalCommands)
		SLATE_ATTRIBUTE(bool, IsEditable)
		SLATE_ATTRIBUTE(bool, DisplayAsReadOnly)
		SLATE_ATTRIBUTE(bool, IsEmpty)
		SLATE_ARGUMENT(TSharedPtr<SWidget>, TitleBar)
		SLATE_ATTRIBUTE(FGraphAppearanceInfo, Appearance)
		SLATE_EVENT(FEdGraphEvent, OnGraphModuleReloaded)
		SLATE_ARGUMENT(UEdGraph*, GraphToEdit)
		SLATE_ARGUMENT(TSharedPtr<TArray<FDiffSingleResult>>, DiffResults)
		SLATE_ARGUMENT(FGraphEditorEvents, GraphEvents)
		SLATE_ARGUMENT(bool, AutoExpandActionMenu)
		SLATE_ARGUMENT(TWeakPtr<FAssetEditorToolkit>, AssetEditorToolkit)
		SLATE_EVENT(FSimpleDelegate, OnNavigateHistoryBack)
		SLATE_EVENT(FSimpleDelegate, OnNavigateHistoryForward)
		SLATE_ATTRIBUTE(bool, ShowGraphStateOverlay)
		SLATE_END_ARGS()

	/** Slate Constructor */
	void Construct(const FArguments& InArgs);
};