// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "DialogueEditor.h"
//UE
#include "EdGraphUtilities.h"
#include "FileHelpers.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "HAL/PlatformApplicationMisc.h"
#include "IDetailsView.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UObject/SavePackage.h"
#include "PropertyEditorModule.h"
#include "SGraphPanel.h"
#include "ToolMenuEntry.h"
//Plugin
#include "Dialogue.h"
#include "DialogueEditorTabs.h"
#include "Graph/DialogueEdGraph.h"
#include "Graph/DialogueEdGraphSchema.h"
#include "Graph/Nodes/GraphNodeDialogue.h"
#include "Graph/Slate/SDialogueGraphEditor.h"
#include "LogDialogueTree.h"
#include "Nodes/DialogueNode.h"

#define LOCTEXT_NAMESPACE "DialogueEditor"

void FDialogueEditor::InitEditor(const EToolkitMode::Type Mode, 
    const TSharedPtr<IToolkitHost>& InitToolkitHost, UDialogue* InDialogue)
{
    TargetDialogue = InDialogue;
    CreateEdGraph();

    FGenericCommands::Register();
    FGraphEditorCommands::Register();

    CreateInternalWidgets();

    //Define Layout 
    const TSharedRef<FTabManager::FLayout> DefaultLayout = 
        CreateLayout();
       
    //Initialize toolkit
    const bool bCreateDefaultStandaloneMenu = true; 
    const bool bCreateDefaultToolbar = true; 

    FAssetEditorToolkit::InitAssetEditor
    (
        Mode,
        InitToolkitHost,
        AppName,
        DefaultLayout,
        bCreateDefaultStandaloneMenu,
        bCreateDefaultToolbar,
        TargetDialogue,
        false
    );

    RegenerateMenusAndToolbars();
}

UDialogue* FDialogueEditor::GetDialogue() const
{
    return TargetDialogue;
}

void FDialogueEditor::RegisterTabSpawners(
    const TSharedRef<FTabManager>& InTabManager)
{
    //Set up workspace category
    WorkspaceMenuCategory = 
        InTabManager->AddLocalWorkspaceMenuCategory(
            LOCTEXT(
                "DialogueEditorWorkspace", "Dialogue Tree Editor"
            )
        );
    FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

    //Register tab spawners
    RegisterViewportTabSpawner(InTabManager);
    RegisterGraphPropertiesTabSpawner(InTabManager); 
    RegisterNodeDetailsTabSpawner(InTabManager);
}

void FDialogueEditor::UnregisterTabSpawners(
    const TSharedRef<FTabManager>& InTabManager)
{
    FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

    InTabManager->UnregisterTabSpawner(
        FDialogueEditorTabs::ViewportTabID
    ); 
    InTabManager->UnregisterTabSpawner(
        FDialogueEditorTabs::NodeDetailsTabID
    );
    InTabManager->UnregisterTabSpawner(
        FDialogueEditorTabs::GraphPropertiesTabID
    );
}

FLinearColor FDialogueEditor::GetWorldCentricTabColorScale() const
{
    return FLinearColor::White;
}

FName FDialogueEditor::GetToolkitFName() const
{
    return FName(TEXT("FDialogueTreeEditor"));
}

FText FDialogueEditor::GetBaseToolkitName() const
{
    return LOCTEXT(
        "DialogueEditorBaseToolkitName", "Dialogue Tree Editor"
    );
}

FString FDialogueEditor::GetWorldCentricTabPrefix() const
{
    return TEXT("DialogueTreeEditor");
}

FText FDialogueEditor::GetToolkitName() const
{
    if (TargetDialogue)
    {
        return FText::FromString(TargetDialogue->GetName());
    }

    return LOCTEXT(
        "DialogueEditorToolkitName", "Dialogue Tree Editor"
    );
}

FText FDialogueEditor::GetToolkitToolTipText() const
{
    return FAssetEditorToolkit::GetToolTipTextForObject(
        TargetDialogue
    );
}

bool FDialogueEditor::OnRequestClose()
{
    return FAssetEditorToolkit::OnRequestClose();
}

void FDialogueEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
    check(TargetDialogue);

    Collector.AddReferencedObject(TargetDialogue);
}

FString FDialogueEditor::GetReferencerName() const
{
    return TEXT("FDialogueTreeEditor");
}

void FDialogueEditor::CreateInternalWidgets()
{
    //Create viewport widget
    ViewportWidget = CreateGraphViewportWidget();

    //Get property module
    FPropertyEditorModule& PropertyModule = 
        FModuleManager::LoadModuleChecked<FPropertyEditorModule>(
            "PropertyEditor"
        );

    //Create details/properties widgets
    CreateNodeDetailsWidget(PropertyModule);
    CreateGraphPropertiesWidget(PropertyModule);

    //Add compile button to the toolbar
    ExtendToolbar();
}

TSharedRef<SGraphEditor> FDialogueEditor::CreateGraphViewportWidget()
{
    check(TargetDialogue);
    UEdGraph* TargetGraph = TargetDialogue->GetEdGraph();
    check(TargetGraph);

    //Set up corner text
    FGraphAppearanceInfo AppearanceInfo;
    AppearanceInfo.CornerText = FText::FromString(TargetDialogue->GetName());

    //Register editor commands
    RegisterCommands();

    //Register editor selection events 
    SGraphEditor::FGraphEditorEvents EditorEvents;
    EditorEvents.OnSelectionChanged = 
        SGraphEditor::FOnSelectionChanged::CreateSP(
            this, 
            &FDialogueEditor::OnChangeSelection
        );

    //Return graph editor 
    return SNew(SGraphEditor)
        .AdditionalCommands(EditorCommands)
        .IsEditable(true)
        .Appearance(AppearanceInfo)
        .GraphToEdit(TargetGraph)
        .GraphEvents(EditorEvents);
}

void FDialogueEditor::CreateEdGraph()
{   
    if (!TargetDialogue->GetEdGraph())
    {
        UE_LOG(
            LogDialogueTree,
            Warning,
            TEXT("No dialogue graph found. Creating a new graph. If this is the first time you open the dialogue asset you can ignore this message.")
        );

        //Create the ed graph
        UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(
            TargetDialogue,
            NAME_None, 
            UDialogueEdGraph::StaticClass(),
            UDialogueEdGraphSchema::StaticClass()
        );
        check(NewGraph);

        //Attach new graph to the dialogue asset
        TargetDialogue->SetEdGraph(NewGraph);
        TargetDialogue->GetEdGraph()->bAllowDeletion = false;

        //Spawn initial nodes
        const UEdGraphSchema* GraphSchema = NewGraph->GetSchema();
        check(GraphSchema);
        GraphSchema->CreateDefaultNodesForGraph(
            *NewGraph
        );
    }
}

TSharedRef<FTabManager::FLayout> FDialogueEditor::CreateLayout() const
{
    FName LayoutName = "DialogueEditorLayout";
    FName ViewportTabID = 
        FDialogueEditorTabs::ViewportTabID;
    FName GraphPropertiesTabID = 
        FDialogueEditorTabs::GraphPropertiesTabID;
    FName NodeDetailsTabID = 
        FDialogueEditorTabs::NodeDetailsTabID;

    return FTabManager::NewLayout(LayoutName)
        ->AddArea
        (
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Vertical)
            ->Split
            (
                FTabManager::NewSplitter()
                ->SetOrientation(Orient_Horizontal)
                ->SetSizeCoefficient(0.9f)
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.8f)
                    ->AddTab(
                        ViewportTabID, 
                        ETabState::OpenedTab
                    )
                    ->SetHideTabWell(true)
                )
                ->Split
                (
                    FTabManager::NewSplitter()
                    ->SetOrientation(Orient_Vertical)
                    ->SetSizeCoefficient(0.2f)
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->SetSizeCoefficient(0.4f)
                        ->AddTab(
                            GraphPropertiesTabID, 
                            ETabState::OpenedTab
                        )
                        ->SetHideTabWell(true)
                    )
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->SetSizeCoefficient(0.6f)
                        ->AddTab(
                            NodeDetailsTabID, 
                            ETabState::OpenedTab
                        )
                        ->SetHideTabWell(true)
                    )
                )
            )
        );
}

void FDialogueEditor::RegisterViewportTabSpawner(
    const TSharedRef<FTabManager>& InTabManager)
{
    check(WorkspaceMenuCategory.IsValid());

    //Get args 
    FOnSpawnTab ViewportSpawner = FOnSpawnTab::CreateSP(
        this, &FDialogueEditor::CreateGraphViewportTab
    );
    FText ViewportDisplayText = LOCTEXT(
        "DialogueEditorViewportTab", "Viewport"
    );
    FSlateIcon ViewportIcon = FSlateIcon(
        FAppStyle::GetAppStyleSetName(),
        "GraphEditor.EventGraph_16x"
    );

    //Register spawner
    InTabManager->RegisterTabSpawner(
        FDialogueEditorTabs::ViewportTabID,
        ViewportSpawner)
        .SetDisplayName(ViewportDisplayText)
        .SetGroup(WorkspaceMenuCategory.ToSharedRef())
        .SetIcon(ViewportIcon);
}

void FDialogueEditor::RegisterGraphPropertiesTabSpawner(
    const TSharedRef<FTabManager>& InTabManager)
{
    check(WorkspaceMenuCategory.IsValid());

    //Get spawner args
    FOnSpawnTab GraphPropertiesSpawner = FOnSpawnTab::CreateSP(
        this, &FDialogueEditor::CreatePropertiesTab
    );
    FText GraphPropertiesDisplayText = LOCTEXT(
        "DialogueEditorViewportTab", "Viewport"
    );
    FSlateIcon GraphPropertiesIcon = FSlateIcon(
        FAppStyle::GetAppStyleSetName(),
        "LevelEditor.Tabs.Details"
    );

    //Register spawner
    InTabManager->RegisterTabSpawner(
        FDialogueEditorTabs::GraphPropertiesTabID,
        GraphPropertiesSpawner)
        .SetDisplayName(GraphPropertiesDisplayText)
        .SetGroup(WorkspaceMenuCategory.ToSharedRef())
        .SetIcon(GraphPropertiesIcon);
}

void FDialogueEditor::RegisterNodeDetailsTabSpawner(
    const TSharedRef<FTabManager>& InTabManager)
{
    check(WorkspaceMenuCategory.IsValid());

    //Get spawner args
    FOnSpawnTab NodeDetailsSpawner = FOnSpawnTab::CreateSP(
        this, 
        &FDialogueEditor::CreateGraphDetailsTab
    );
    FText NodeDetailsDisplayText = LOCTEXT(
        "DialogueEditorDetailsTab", "Selection Details");
    FSlateIcon NodeDetailsIcon = FSlateIcon(
        FAppStyle::GetAppStyleSetName(),
        "LevelEditor.Tabs.Details"
    );

    //Register spawner
    InTabManager->RegisterTabSpawner(
        FDialogueEditorTabs::NodeDetailsTabID,
        NodeDetailsSpawner)
        .SetDisplayName(NodeDetailsDisplayText)
        .SetGroup(WorkspaceMenuCategory.ToSharedRef())
        .SetIcon(NodeDetailsIcon);
}

void FDialogueEditor::CreateNodeDetailsWidget(
    FPropertyEditorModule& PropertyModule)
{
    //Args for new widget
    FDetailsViewArgs DetailsArgs;
    DetailsArgs.bHideSelectionTip = true;
    DetailsArgs.NotifyHook = this;

    //Create the details widget
    NodeDetailsWidget = 
        PropertyModule.CreateDetailView(DetailsArgs);

    //Setup the widget 
    NodeDetailsWidget->SetObject(nullptr);
    NodeDetailsWidget->OnFinishedChangingProperties().AddSP(
        this, 
        &FDialogueEditor::OnFinishedChangingProperties
    );
}

void FDialogueEditor::CreateGraphPropertiesWidget(
    FPropertyEditorModule& PropertyModule)
{
    //Get graph
    check(ViewportWidget.IsValid());
    UDialogueEdGraph* DialogueGraph =
        Cast<UDialogueEdGraph>(ViewportWidget->GetCurrentGraph());
    check(DialogueGraph);

    //Create graph properties widget
    FDetailsViewArgs PropertiesArgs;
    PropertiesArgs.bHideSelectionTip = true;
    PropertiesArgs.NotifyHook = this;
    GraphPropertiesWidget =
        PropertyModule.CreateDetailView(PropertiesArgs);

    //Setup new widget
    GraphPropertiesWidget->SetObject(DialogueGraph);
    GraphPropertiesWidget->OnFinishedChangingProperties().AddSP(
        this, 
        &FDialogueEditor::OnFinishedChangingProperties
    );
}

void FDialogueEditor::OnFinishedChangingProperties(
    const FPropertyChangedEvent& PropertyChangedEvent)
{
    //Get graph
    check(TargetDialogue);
    UEdGraph* TargetGraph = TargetDialogue->GetEdGraph();
    check(TargetGraph);

    //Set compile status to uncompiled and clear vis cache
    TargetDialogue->SetCompileStatus(EDialogueCompileStatus::Uncompiled);
    TargetGraph->GetSchema()->ForceVisualizationCacheClear();
}

TSharedRef<SDockTab> FDialogueEditor::CreateGraphViewportTab(
    const FSpawnTabArgs& Args) const
{
    //Verify working with the viewport tab
    check(Args.GetTabId() == FDialogueEditorTabs::ViewportTabID);
    check(TargetDialogue);

    //Create the new tab
    TSharedRef<SDockTab> NewTab = 
        SNew(SDockTab)
        .Label(LOCTEXT("ViewportTabLabel", "Dialogue Graph"))
        .OnCanCloseTab_Lambda([]() { return false; });

    if (ViewportWidget.IsValid())
    {
        NewTab->SetContent(ViewportWidget.ToSharedRef());
    }

    return NewTab;
}

TSharedRef<SDockTab> FDialogueEditor::CreateGraphDetailsTab(
    const FSpawnTabArgs& Args) const
{
    //Verify we are working with the details tab
    check(Args.GetTabId() == FDialogueEditorTabs::NodeDetailsTabID);

    //Create the tab
    TSharedRef<SDockTab> NewTab =
        SNew(SDockTab)
        .Label(LOCTEXT("GraphDetailsTabLabel", "Selection Details"))
        .OnCanCloseTab_Lambda([]() { return false; });

    //Attach the widget
    if (NodeDetailsWidget.IsValid())
    {
        NewTab->SetContent(NodeDetailsWidget.ToSharedRef());
    }

    return NewTab;
}

TSharedRef<SDockTab> FDialogueEditor::CreatePropertiesTab(
    const FSpawnTabArgs& Args) const
{
    //Verify we are working with the properties tab
    check(Args.GetTabId() == 
        FDialogueEditorTabs::GraphPropertiesTabID);

    //Create the tab
    TSharedRef<SDockTab> NewTab =
        SNew(SDockTab)
        .Label(LOCTEXT(
            "GraphPropertiesTabLabel", 
            "Graph Properties"
        ))
        .OnCanCloseTab_Lambda([]() { return false; });

    //Add the widget to the tab
    if (GraphPropertiesWidget.IsValid())
    {
        NewTab->SetContent(GraphPropertiesWidget.ToSharedRef());
    }

    return NewTab;
}

void FDialogueEditor::ExtendToolbar()
{
    //Set up the toolbar exteder
    TSharedPtr<FExtender> Extender = MakeShareable(new FExtender);

    //Create callback delegate and register with extender
    FToolBarExtensionDelegate ExtensionDelegate =
        FToolBarExtensionDelegate::CreateSP(
            this,
            &FDialogueEditor::FillToolbarMenu
        );

    Extender->AddToolBarExtension(
        FName("Asset"),
        EExtensionHook::Before,
        GetToolkitCommands(),
        ExtensionDelegate
    );

    //Add the extender to the toolbar
    AddToolbarExtender(Extender);
}

void FDialogueEditor::FillToolbarMenu(FToolBarBuilder& ToolbarBuilder)
{
    //Build up toolbar 
    ToolbarBuilder.BeginSection("DialogueTree");
    {
        FText CompileLabel =
            LOCTEXT("CompileLabel", "Compile");

        FUIAction CompileAction = 
            FUIAction(FExecuteAction::CreateSP(
                this, 
                &FDialogueEditor::OnCompile
            ));

        ToolbarBuilder.AddToolBarButton(
            CompileAction,
            NAME_None,
            CompileLabel,
            CompileLabel,
            TAttribute<FSlateIcon>(
                this,
                &FDialogueEditor::GetStatusImage
            )
        );
    }
    ToolbarBuilder.EndSection();
}

FSlateIcon FDialogueEditor::GetStatusImage() const
{
    check(TargetDialogue);

    //Set icon names
    static const FName CompileStatusBackground(
        "Blueprint.CompileStatus.Background"
    );
    static const FName CompileStatusCompiled(
        "Blueprint.CompileStatus.Overlay.Good"
    );
    static const FName CompileStatusUncompiled(
        "Blueprint.CompileStatus.Overlay.Unknown"
    );
    static const FName CompileStatusFailed(
        "Blueprint.CompileStatus.Overlay.Error"
    );

    //Combine icons and return as appropriate 
    switch (TargetDialogue->GetCompileStatus())
    {
    case EDialogueCompileStatus::Compiled:
        return FSlateIcon(
            FAppStyle::GetAppStyleSetName(), 
            CompileStatusBackground, 
            NAME_None, 
            CompileStatusCompiled
        );

    case EDialogueCompileStatus::Failed:
        return FSlateIcon(
            FAppStyle::GetAppStyleSetName(), 
            CompileStatusBackground, 
            NAME_None, 
            CompileStatusFailed
        );

    case EDialogueCompileStatus::Uncompiled:
        return FSlateIcon(
            FAppStyle::GetAppStyleSetName(), 
            CompileStatusBackground, 
            NAME_None, 
            CompileStatusUncompiled
        );

    default: //Default to failed: should never happen
        return FSlateIcon(
            FAppStyle::GetAppStyleSetName(), 
            CompileStatusBackground, 
            NAME_None, 
            CompileStatusFailed
        );
    }
}

void FDialogueEditor::OnCompile()
{
    check(TargetDialogue);
    UEdGraph* TargetGraph = TargetDialogue->GetEdGraph();
    check(TargetGraph);

    UDialogueEdGraph* TargetDialogueGraph = 
        CastChecked<UDialogueEdGraph>(TargetGraph);
    TargetDialogueGraph->CompileAsset();
}

void FDialogueEditor::RegisterCommands()
{
    //Don't double register commands
    if (EditorCommands.IsValid())
    {
        return;
    }

    EditorCommands = MakeShareable(new FUICommandList);

    //Rename action
    EditorCommands->MapAction(
        FGenericCommands::Get().Rename,
        FExecuteAction::CreateRaw(
            this,
            &FDialogueEditor::OnRenameNode
        ),
        FCanExecuteAction::CreateRaw(
            this,
            &FDialogueEditor::CanRenameNode
        )
    );

    //Duplicate action
    EditorCommands->MapAction(
        FGenericCommands::Get().Duplicate,
        FExecuteAction::CreateRaw(
            this, 
            &FDialogueEditor::OnDuplicateNodes
        ),
        FCanExecuteAction::CreateRaw(
            this, 
            &FDialogueEditor::CanDuplicateNodes
        )
    );

    //Select all action 
    EditorCommands->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(
            this,
            &FDialogueEditor::OnSelectAll
        ),
        FCanExecuteAction::CreateRaw(
            this, 
            &FDialogueEditor::CanSelectAll
        )
    );

    //Delete action 
    EditorCommands->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(
            this, 
            &FDialogueEditor::OnDeleteNodes
        ),
        FCanExecuteAction::CreateRaw(
            this, 
            &FDialogueEditor::CanDeleteNodes
        )
    );

    //Copy action 
    EditorCommands->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw(
            this, 
            &FDialogueEditor::OnCopyNodes
        ),
        FCanExecuteAction::CreateRaw(
            this, 
            &FDialogueEditor::CanCopyNodes
        )
    );

    //Cut action 
    EditorCommands->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw(
            this, 
            &FDialogueEditor::OnCutNodes
        ),
        FCanExecuteAction::CreateRaw(
            this, 
            &FDialogueEditor::CanCutNodes
        )
    );

    //Paste action
    EditorCommands->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw(
            this, 
            &FDialogueEditor::OnPasteNodes
        ),
        FCanExecuteAction::CreateRaw(
            this, 
            &FDialogueEditor::CanPasteNodes
        )
    );
}

void FDialogueEditor::OnRenameNode()
{
    if (ViewportWidget.IsValid() == false)
    {
        return;
    }

    FGraphPanelSelectionSet Selected = 
        ViewportWidget->GetSelectedNodes();
    for (UObject* Item : Selected)
    {
        UEdGraphNode* Node = Cast<UEdGraphNode>(Item);
        if (Node)
        {
            ViewportWidget->IsNodeTitleVisible(Node, true);
        }
    }
}

bool FDialogueEditor::CanRenameNode() const
{
    if (ViewportWidget.IsValid() == false)
    {
        return false;
    }

    FGraphPanelSelectionSet Selected = 
        ViewportWidget->GetSelectedNodes();

    //Allow only one node to be renamed at a time
    if (Selected.Num() != 1)
    {
        return false;
    }

    //Ensure the node can be renamed 
    for (UObject* Item : Selected)
    {
        UEdGraphNode* Node = Cast<UEdGraphNode>(Item);
        if (!Node || Node->bCanRenameNode == false)
        {
            return false;
        }
    }

    return true;
}

void FDialogueEditor::OnDuplicateNodes()
{
    OnCopyNodes();
    OnPasteNodes();
}

bool FDialogueEditor::CanDuplicateNodes() const
{
    return CanCopyNodes();
}

void FDialogueEditor::OnSelectAll()
{
    if (ViewportWidget.IsValid())
    {
        ViewportWidget->SelectAllNodes();
    }
}

bool FDialogueEditor::CanSelectAll() const
{
    return true;
}

void FDialogueEditor::OnDeleteNodes()
{
    if (ViewportWidget.IsValid() == false)
    {
        return;
    }

    //Set up transaction
    const FScopedTransaction Transaction(
        FGenericCommands::Get().Delete->GetDescription()
    );
    ViewportWidget->GetCurrentGraph()->Modify();

    FGraphPanelSelectionSet Selected = 
        ViewportWidget->GetSelectedNodes();
    ViewportWidget->ClearSelectionSet();

    //Delete any nodes that allow deletion 
    for (UObject* Item : Selected)
    {
        //Convert selected item to a node 
        UEdGraphNode* Node = Cast<UEdGraphNode>(Item);

        if (Node && Node->CanUserDeleteNode())
        {
            Node->Modify();
            Node->GetSchema()->BreakNodeLinks(*Node);
            Node->DestroyNode();
        }
    }
}

void FDialogueEditor::OnDeleteDuplicatableNodes()
{
    if (ViewportWidget.IsValid() == false)
    {
        return;
    }

    //Set up transaction
    const FScopedTransaction Transaction(
        FGenericCommands::Get().Delete->GetDescription()
    );
    ViewportWidget->GetCurrentGraph()->Modify();

    //Filter out any non-duplicatable items so they are not deleted
    FGraphPanelSelectionSet OldSelected = 
        ViewportWidget->GetSelectedNodes();
    ViewportWidget->ClearSelectionSet();

    for (UObject* Item : OldSelected)
    {
        UEdGraphNode* Node = Cast<UEdGraphNode>(Item);

        if (Node && Node->CanDuplicateNode())
        {
            ViewportWidget->SetNodeSelection(Node, true);
        }
    }

    //Delete filtered selection 
    OnDeleteNodes();
    ViewportWidget->ClearSelectionSet();

    //Add back any selection items that had been filtered out 
    for (UObject* Item : OldSelected)
    {
        if (UEdGraphNode* Node = Cast<UEdGraphNode>(Item))
        {
            ViewportWidget->SetNodeSelection(Node, true);
        }
    }
}

bool FDialogueEditor::CanDeleteNodes() const
{
    if (ViewportWidget.IsValid() == false)
    {
        return false;
    }

    FGraphPanelSelectionSet Selected = 
        ViewportWidget->GetSelectedNodes();

    //Can delete if any selected node can be deleted 
    for (UObject* Item : Selected)
    {
        //Convert selected item to a node 
        UEdGraphNode* Node = Cast<UEdGraphNode>(Item);

        if (Node && Node->CanUserDeleteNode())
        {
            return true;
        }
    }

    return false;
}

void FDialogueEditor::OnCopyNodes()
{
    if (ViewportWidget.IsValid() == false)
    {
        return;
    }

    FGraphPanelSelectionSet Selected = 
        ViewportWidget->GetSelectedNodes();

    //Copy any nodes that allow 
    for (FGraphPanelSelectionSet::TIterator SelectedIter(Selected);
        SelectedIter; ++SelectedIter)
    {
        //Convert selected item to a node 
        UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);

        if (Node && Node->CanDuplicateNode())
        {
            Node->PrepareForCopying();
        }
        else
        {
            SelectedIter.RemoveCurrent();
        }
    }

    FString Exported;
    FEdGraphUtilities::ExportNodesToText(Selected, Exported);
    FPlatformApplicationMisc::ClipboardCopy(*Exported);
}

bool FDialogueEditor::CanCopyNodes() const
{
    if (ViewportWidget.IsValid() == false)
    {
        return false;
    }

    FGraphPanelSelectionSet Selected = 
        ViewportWidget->GetSelectedNodes();

    //Can copy if any node can be duplicated
    for (UObject* Item : Selected)
    {
        //Convert selected item to a node 
        UEdGraphNode* Node = Cast<UEdGraphNode>(Item);

        if (Node && Node->CanDuplicateNode())
        {
            return true;
        }
    }

    return false;
}

void FDialogueEditor::OnCutNodes()
{
    OnCopyNodes();
    OnDeleteDuplicatableNodes();
}

bool FDialogueEditor::CanCutNodes() const
{
    return CanCopyNodes() && CanDeleteNodes();
}

void FDialogueEditor::OnChangeSelection(const TSet<UObject*>& SelectedObjects)
{
    //If none selected, clear details panel
    if (SelectedObjects.Num() < 1)
    {
        NodeDetailsWidget->SetObject(nullptr);
    }
    //Otherwise, view the selected nodes via the details panel 
    else
    {
        TArray<UObject*> SelectedNodes;

        for (UObject* CurrentObject : SelectedObjects)
        {
            UGraphNodeDialogue* GraphNode =
                Cast<UGraphNodeDialogue>(CurrentObject);

            if (GraphNode)
            {
                SelectedNodes.Add(
                    Cast<UObject>(GraphNode)
                );
            }
        }

        NodeDetailsWidget->SetObjects(SelectedNodes);
    }
}

void FDialogueEditor::OnPasteNodes()
{
    if (ViewportWidget.IsValid())
    {
        PasteNodesAtLocation(ViewportWidget->GetPasteLocation());
    }
}

void FDialogueEditor::PasteNodesAtLocation(const FVector2D& PasteLocation)
{
    UE_LOG(LogTemp, Error, TEXT("Pasting"));


    if (ViewportWidget.IsValid() == false)
    {
        return;
    }

    //Set up transaction
    const FScopedTransaction Transaction(
        FGenericCommands::Get().Paste->GetDescription()
    );
    UEdGraph* EdGraph = ViewportWidget->GetCurrentGraph();
    EdGraph->Modify();

    //Clear selection to make room for pasted items to be selected
    ViewportWidget->ClearSelectionSet();
    
    //Retrieve import text from clipboard 
    FString ImportText;
    FPlatformApplicationMisc::ClipboardPaste(ImportText);

    //Retrieve nodes from text 
    TSet<UEdGraphNode*> PastedNodes;
    FEdGraphUtilities::ImportNodesFromText(
        EdGraph, 
        ImportText,
        PastedNodes
    );

    //Average node position for group movement 
    FVector2D AverageNodePosition(0.f, 0.f);
    for (UEdGraphNode* Node : PastedNodes)
    {
        AverageNodePosition.X += Node->NodePosX;
        AverageNodePosition.Y += Node->NodePosY;
    }
    AverageNodePosition /= float(PastedNodes.Num());

    //Place each node 
    for (UEdGraphNode* Node : PastedNodes)
    {
        ViewportWidget->SetNodeSelection(Node, true);
        Node->NodePosX = 
            Node->NodePosX
            - AverageNodePosition.X 
            + PasteLocation.X;
        Node->NodePosY = 
            Node->NodePosY
            - AverageNodePosition.Y 
            + PasteLocation.Y;
        Node->SnapToGrid(16);
        Node->CreateNewGuid();
    }

    //Init. each node's ID as necessary & notify them they have been copied 
    for (UEdGraphNode* Node : PastedNodes)
    {
        if (UGraphNodeDialogue* DialogueNode =
            Cast<UGraphNodeDialogue>(Node))
        {
            DialogueNode->InitNodeInDialogueGraph(
                ViewportWidget->GetCurrentGraph()
            );
        }
    }

    //Update graph editor 
    ViewportWidget->NotifyGraphChanged();
    UObject* GraphOwner = EdGraph->GetOuter();
    if (GraphOwner)
    {
        GraphOwner->PostEditChange();
    }
}

bool FDialogueEditor::CanPasteNodes() const
{
    UE_LOG(LogTemp, Error, TEXT("Can paste?"));

    /** Note:
    * My grasp of pasting is still a little sketchy.
    * AIEditor.cpp is a good example resource.
    */
    if (ViewportWidget.IsValid() == false)
    {
        return false;
    }

    FString ClipboardContent;
    FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

    return FEdGraphUtilities::CanImportNodesFromText(
        ViewportWidget->GetCurrentGraph(), 
        ClipboardContent
    );
}

#undef LOCTEXT_NAMESPACE