// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "Misc/NotifyHook.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "UObject/GCObject.h"

class IDetailsView;
class UDialogue;

/**
* Manages core editor features and tabs for the dialogue graph. 
*/
class DIALOGUETREEEDITOR_API FDialogueEditor : public FAssetEditorToolkit, 
	public FNotifyHook, public FGCObject
{
public:
	/**
	* Initializes the editor. 
	* 
	* @param Mode - EToolkitMode::Type.
	* @param InitToolkitHost - TSharedPtr<IToolkitHost>&.
	* @param InDialogue - UDialogue*, the dialogue to edit.
	*/
	void InitEditor(
		const EToolkitMode::Type Mode,
		const TSharedPtr<IToolkitHost>& InitToolkitHost, 
		UDialogue* InDialogue
	);

	/**
	* Retrieve the dialogue being edited.
	* 
	* @return UDialogue*, the dialogue being edited. 
	*/
	UDialogue* GetDialogue() const;

	/** IToolkitInterface Impl. */
	virtual void RegisterTabSpawners(
		const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(
		const TSharedRef<FTabManager>& InTabManager) override;
	/** End IToolkitInterface */

	/** FAssetEditorToolkit Impl. */
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual bool OnRequestClose() override;
	/** End FAssetEditorToolkit */

	/** FGCObject Impl. */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	/** End FGCObject */

	/**
	* Handles pasting of copied nodes into the graph. 
	* CanPasteNodes() should always be checked first. 
	*/
	void OnPasteNodes();

	/**
	* Pastes copied nodes into the graph at the given location. 
	* CanPasteNodes() should always be checked first.
	* 
	* @param PasteLocation - const FVector2D&, the location to paste
	* the nodes. 
	*/
	void PasteNodesAtLocation(const FVector2D& PasteLocation); 

	/**
	* Checks if we can paste nodes into the graph. 
	* 
	* @return bool - true if we can paste nodes, false otherwise. 
	*/
	bool CanPasteNodes() const;

private: 
	/**
	* Creates the various widgets that make up the editor. 
	*/
	void CreateInternalWidgets();

	/**
	* Creates the graph/viewport widget. 
	* 
	* @return TSharedRef<SGraphEditor> - the viewport widget. 
	*/
	TSharedRef<SGraphEditor> CreateGraphViewportWidget();

	/**
	* Creates the dialogue's editor graph. 
	*/
	void CreateEdGraph();

	/**
	* Defines the layout for the dialogue editor. 
	* 
	* @return TSharedRef<FTabManager::FLayout> - the created layout.
	*/
	TSharedRef<FTabManager::FLayout> CreateLayout() const;

	/**
	* Registers the tab spawner for the viewport graph. 
	* 
	* @param InTabManager - const TSharedRef<FTabManager>&
	*/
	void RegisterViewportTabSpawner(
		const TSharedRef<FTabManager>& InTabManager);

	/**
	* Registers the tab spawner for the graph properties panel.
	*
	* @param InTabManager - const TSharedRef<FTabManager>&
	*/
	void RegisterGraphPropertiesTabSpawner(
		const TSharedRef<FTabManager>& InTabManager);

	/**
	* Registers the tab spawner for the selected node details
	* panel.
	*
	* @param InTabManager - const TSharedRef<FTabManager>&
	*/
	void RegisterNodeDetailsTabSpawner(
		const TSharedRef<FTabManager>& InTabManager);

	/**
	* Creates the widget for the selected node details panel.
	* 
	* @param PropertyModule - FPropertyEditorModule&
	*/
	void CreateNodeDetailsWidget(FPropertyEditorModule& PropertyModule);

	/**
	* Creates the widget for the graph properties panel.
	*
	* @param PropertyModule - FPropertyEditorModule&
	*/
	void CreateGraphPropertiesWidget(FPropertyEditorModule& PropertyModule);

	/**
	* Behaviors to perform whenever properties have changed. Clears
	* the visualization cache and marks the dialogue as needing to 
	* be compiled. 
	*/
	void OnFinishedChangingProperties(
		const FPropertyChangedEvent& PropertyChangedEvent);

	/**
	* Creates the tab for the graph viewport. 
	* 
	* @param Args - FSpawnTabArgs&. 
	* @return TSharedRef<SDockTab>, the created tab. 
	*/
	TSharedRef<SDockTab> CreateGraphViewportTab(
		const FSpawnTabArgs& Args) const;

	/**
	* Creates the tab for the selected node details panel. 
	* 
	* @param Args - FSPawnTabArgs&.
	* @return TSharedRef<SDockTab>, the created tab. 
	*/
	TSharedRef<SDockTab> CreateGraphDetailsTab(const FSpawnTabArgs& Args) const;

	/**
	* Creates the tab for the graph's properties panel.
	*
	* @param Args - FSPawnTabArgs&.
	* @return TSharedRef<SDockTab>, the created tab.
	*/
	TSharedRef<SDockTab> CreatePropertiesTab(const FSpawnTabArgs& Args) const;

	/**
	* Adds compile button and other options to the editor's toolbar.
	*/
	void ExtendToolbar();

	/**
	* Event called to fill the toolbar menu with new buttons. 
	* 
	* @param ToolbarBuilder - FToolBarBuilder&. 
	*/
	void FillToolbarMenu(FToolBarBuilder& ToolbarBuilder);

	/**
	* Gets the icon associated with the dialogue's current compile 
	* state. 
	* 
	* @return FSlateIcon - the retrieved icon. 
	*/
	FSlateIcon GetStatusImage() const;

	/**
	* Attempts to compile the dialogue. 
	*/
	void OnCompile();

	// Commands
	/**
	* Registers the command list for the dialogue editor. 
	*/
	void RegisterCommands();

	/** 
	* Renames a given node.
	*/
	void OnRenameNode();

	/**
	* Whether we can rename a selected node. 
	*/
	bool CanRenameNode() const;

	/** 
	* Duplicates a set of nodes.
	*/
	void OnDuplicateNodes();
	
	/**
	* Checks if we can duplicate a set of nodes.
	*/
	bool CanDuplicateNodes() const;

	/** 
	* Selects all of a set of nodes. 
	*/
	void OnSelectAll();

	/**
	* Checks if we can select all of a set of nodes.
	*/
	bool CanSelectAll() const;

	/** 
	* Deletes a set of nodes. 
	*/
	void OnDeleteNodes(); 

	/**
	* Deletes a set of duplicatable nodes. 
	*/
	void OnDeleteDuplicatableNodes();

	/**
	* Checks if we can delete a set of nodes. 
	*/
	bool CanDeleteNodes() const;

	/** 
	* Copies a set of nodes.
	*/
	void OnCopyNodes();

	/**
	* Checks if we can copy a set of nodes. 
	*/
	bool CanCopyNodes() const;

	/** 
	* Cuts a set of nodes
	*/
	void OnCutNodes();

	/**
	* Checks if we can cut a set of nodes. 
	*/
	bool CanCutNodes() const;
	
	/**
	* Handle changing the set of selected nodes.
	* 
	* @param SelectedObjects - const TSet<UObject*>&, the new
	* selection set. 
	*/
	void OnChangeSelection(const TSet<UObject*>& SelectedObjects);

private:
	/** The dialogue being edited */
	TObjectPtr<UDialogue> TargetDialogue;

	/** The main graph viewport */
	TSharedPtr<SGraphEditor> ViewportWidget;

	/** Details panel for the currently selected node */
	TSharedPtr<IDetailsView> NodeDetailsWidget;

	/** Details panel for the graph's properties */
	TSharedPtr<IDetailsView> GraphPropertiesWidget;

	/** The list of UI commands for the editor */
	TSharedPtr<FUICommandList> EditorCommands;
};