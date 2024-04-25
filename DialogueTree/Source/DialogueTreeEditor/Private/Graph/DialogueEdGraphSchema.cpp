// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/DialogueEdGraphSchema.h"
//UE
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "Settings/EditorStyleSettings.h"
#include "Toolkits/IToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "ToolMenu.h"
//Plugin
#include "Dialogue.h"
#include "DialogueEditor.h"
#include "DialogueSpeakerSocket.h"
#include "Graph/DialogueEdGraph.h"
#include "Graph/DialogueTreeConnectionDrawingPolicy.h"
#include "Graph/Nodes/GraphNodeDialogueBranch.h"
#include "Graph/Nodes/GraphNodeDialogueEntry.h"
#include "Graph/Nodes/GraphNodeDialogueEvent.h"
#include "Graph/Nodes/GraphNodeDialogueJump.h"
#include "Graph/Nodes/GraphNodeDialogueReroute.h"
#include "Graph/Nodes/GraphNodeDialogueSpeech.h"
#include "Nodes/DialogueNode.h"
#include "Transitions/DialogueTransition.h"

#define LOCTEXT_NAMESPACE "DialogueEditorSchema"

//Default value for cache refresh ID
int32 UDialogueEdGraphSchema::CurrentCacheRefreshID = 0;

/** 
* NewDialogueNodeAction 
*/

FNewDialogueNodeAction::FNewDialogueNodeAction()
	: FEdGraphSchemaAction()
	, TemplateNode(nullptr)
{}

FNewDialogueNodeAction::FNewDialogueNodeAction(FText InNodeCategory, 
	FText InMenuDesc, FText InToolTip, UGraphNodeDialogue* InTemplateNode)
	: FEdGraphSchemaAction(
		MoveTemp(InNodeCategory), 
		MoveTemp(InMenuDesc),
		MoveTemp(InToolTip), 
		0)
	, TemplateNode(InTemplateNode)
{
	check(TemplateNode);
}


UEdGraphNode* FNewDialogueNodeAction::PerformAction(UEdGraph* ParentGraph,
	UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	if (TemplateNode != nullptr)
	{
		//Begin transaction 
		const FScopedTransaction Transaction(LOCTEXT("AddNode", "Add node"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}
		TemplateNode->SetFlags(RF_Transactional);

		//Setup the node within the graph 
		PlaceNodeInGraph(ParentGraph, Location, FromPin);

		return TemplateNode;
	}

	//No result node was created 
	return nullptr;
}

void FNewDialogueNodeAction::AddReferencedObjects(
	FReferenceCollector& Collector)
{
	//Makes sure objects don't get prematurely garbage collected
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(TemplateNode);
}

void FNewDialogueNodeAction::SetTemplateNode(UGraphNodeDialogue* InNode)
{
	check(InNode);
	TemplateNode = InNode;
}

void FNewDialogueNodeAction::PlaceNodeInGraph(UEdGraph* InParentGraph, 
	const FVector2D InLocation, UEdGraphPin* FromPin)
{
	//Add template node to graph
	TemplateNode->Rename(nullptr, InParentGraph, REN_NonTransactional);
	InParentGraph->AddNode(TemplateNode, true);
	TemplateNode->CreateNewGuid();
	TemplateNode->PostPlacedNewNode();

	//Set the node's location, pins and ID
	TemplateNode->InitNodeInDialogueGraph(InParentGraph);
	SetNodeLocation(InLocation);
	TemplateNode->AllocateDefaultPins();
	TemplateNode->AutowireNewNode(FromPin);
}

void FNewDialogueNodeAction::SetNodeLocation(const FVector2D InLocation)
{
	TemplateNode->NodePosX = InLocation.X;
	TemplateNode->NodePosY = InLocation.Y;
	TemplateNode->SnapToGrid(GetDefault<UEditorStyleSettings>()->GridSnapSize);
}

/** Schema */

FConnectionDrawingPolicy* UDialogueEdGraphSchema::CreateConnectionDrawingPolicy(
	int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, 
	const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	UEdGraph* InGraphObj) const
{
	return new FDialogueTreeConnectionDrawingPolicy(
		InBackLayerID, 
		InFrontLayerID, 
		InZoomFactor, 
		InClippingRect, 
		InDrawElements
	);
}

void UDialogueEdGraphSchema::OnPinConnectionDoubleCicked(
	UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const
{
	//Set up transaction
	const FScopedTransaction Transaction(
		LOCTEXT("CreateRerouteNodeOnWire", "Create Reroute Node")
	);
	check(PinA && PinB);
	check(PinA->GetOwningNode() && PinB->GetOwningNode());
	UEdGraph* ParentGraph = PinA->GetOwningNode()->GetGraph();
	check(ParentGraph);
	ParentGraph->Modify();
	PinA->GetOwningNode()->Modify();
	PinB->GetOwningNode()->Modify();

	//Create and place node
	PlaceRerouteNode(ParentGraph, GraphPosition, PinA, PinB);
}

void UDialogueEdGraphSchema::GetGraphContextActions(
	FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	//Get actions for adding various node types 
	GetSpeechNodeMenuActions(ContextMenuBuilder);
	GetConditionalNodeMenuActions(ContextMenuBuilder);
	GetEventNodeMenuActions(ContextMenuBuilder);
	GetJumpNodeMenuActions(ContextMenuBuilder);
}

void UDialogueEdGraphSchema::GetContextMenuActions(UToolMenu* Menu, 
	UGraphNodeContextMenuContext* Context) const
{
	if (Context->Node)
	{
		//Get the context menu for the node itself
		GetNodeContextMenu(Menu);

		//Get the context menu for each of the node's pins
		const UGraphNodeDialogueBase* DialogueNode = 
			Cast<UGraphNodeDialogueBase>(Context->Node);
		check(DialogueNode);

		TArray<UEdGraphPin*> AllPins = DialogueNode->Pins;

		for (int32 i = 0; i < AllPins.Num(); ++i)
		{
			FText PinText = DialogueNode->GetPinMenuLabel(i);
			GetPinContextMenu(Menu, AllPins[i], PinText);
		}
	}

	UEdGraphSchema::GetContextMenuActions(Menu, Context);
}

const FPinConnectionResponse UDialogueEdGraphSchema::CanCreateConnection(
	const UEdGraphPin* A, const UEdGraphPin* B) const
{
	//Response to return 
	FPinConnectionResponse Response;

	//Connection values to work with 
	FConnectionArgs ConnectionArgs;
	ConnectionArgs.PinA = A;
	ConnectionArgs.PinB = B;
	ConnectionArgs.NodeA = Cast<UGraphNodeDialogueBase>(
		A->GetOwningNode()
	);
	ConnectionArgs.NodeB = Cast<UGraphNodeDialogueBase>(
		B->GetOwningNode()
	);

	//Check for disallowed connections
	CheckForDisallowedConnection(ConnectionArgs, Response);

	if (Response.Response == CONNECT_RESPONSE_DISALLOW)
	{
		return Response;
	}

	//Connection is allowed - either limited or unlimited
	if (!ConnectionHasConnectionLimit(ConnectionArgs, Response))
	{
		//No connection limit 
		Response.Response = CONNECT_RESPONSE_MAKE;
		Response.Message = LOCTEXT("AllowConnection", "Connect nodes");
	}
	return Response;
}

void UDialogueEdGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<UGraphNodeDialogueEntry> NodeCreator(Graph);
	UGraphNodeDialogueEntry* TargetNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(TargetNode, FNodeMetadata::DefaultGraphNode);

	//Connect to root 
	UDialogueEdGraph* DialogueGraph = Cast<UDialogueEdGraph>(&Graph);
	TargetNode->InitNodeInDialogueGraph(DialogueGraph);
	DialogueGraph->SetGraphRoot(TargetNode);
}

FLinearColor UDialogueEdGraphSchema::GetPinTypeColor(
	const FEdGraphPinType& PinType) const
{
	return FLinearColor::White;
}

bool UDialogueEdGraphSchema::IsCacheVisualizationOutOfDate(
	int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UDialogueEdGraphSchema::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UDialogueEdGraphSchema::ForceVisualizationCacheClear() const
{
	++CurrentCacheRefreshID;
}

TSharedPtr<FDialogueEditor> UDialogueEdGraphSchema::GetGraphEditor(
	const UEdGraph* InGraph)
{
	if (InGraph)
	{
		const UDialogueEdGraph* DialogueGraph = Cast<UDialogueEdGraph>(InGraph);

		if (DialogueGraph && DialogueGraph->GetDialogue())
		{
			TSharedPtr<IToolkit> FoundAssetEditor = 
				FToolkitManager::Get().FindEditorForAsset(
					DialogueGraph->GetDialogue()
				);

			return StaticCastSharedPtr<FDialogueEditor>(FoundAssetEditor);
		}
	}

	return nullptr;
}

void UDialogueEdGraphSchema::GetPinContextMenu(UToolMenu* Menu, 
	UEdGraphPin* InPin, FText PinText) const 
{
	//Only add menu if there are any links
	if (InPin->LinkedTo.Num() > 0)
	{
		//Menu header
		FToolMenuSection& PinMenuSection = Menu->AddSection(
			FName(PinText.ToString()), 
			PinText
		);

		//Add an entry to break all pin links at once
		GetBreakAllPinLinksOption(PinMenuSection, InPin);

		//Set up the submenu for breaking individual links 
		GetBreakIndividualPinLinksOption(PinMenuSection, InPin);
	}
}

void UDialogueEdGraphSchema::GetBreakAllPinLinksOption(
	FToolMenuSection& MenuSection, UEdGraphPin* InPin) const
{
	//Add an entry to break all pin links at once
	FExecuteAction BreakAllLinksAction =
		FExecuteAction::CreateUObject(
			this,
			&UDialogueEdGraphSchema::BreakPinLinksContextMenu,
			InPin
		);

	FText BreakAllLinksText = 
		LOCTEXT("BreakAllPinLinksText", "Break all pin links...");

	MenuSection.AddMenuEntry(
		NAME_None,
		BreakAllLinksText,
		BreakAllLinksText,
		FSlateIcon(),
		FUIAction(BreakAllLinksAction, FCanExecuteAction())
	);
}

void UDialogueEdGraphSchema::GetBreakIndividualPinLinksOption(
	FToolMenuSection& MenuSection, UEdGraphPin* InPin) const
{
	TArray<FBreakLinkActionInfo> BreakActions = GetBreakLinkActions(InPin);
	FNewToolMenuDelegate SubMenuDelegate = GetBreakLinkSubMenuDelegate(
		BreakActions
	);

	FText BreakLinkSubmenuLabel = LOCTEXT(
		"BreakLinkSubmenuLabel",
		"Break link to..."
	);

	MenuSection.AddSubMenu(
		NAME_None,
		BreakLinkSubmenuLabel,
		BreakLinkSubmenuLabel,
		FNewToolMenuChoice(SubMenuDelegate)
	);
}

TArray<FBreakLinkActionInfo> UDialogueEdGraphSchema::GetBreakLinkActions(
	UEdGraphPin* InPin) const
{
	TArray<FBreakLinkActionInfo> BreakActions;

	//Add an entry to break each link separately
	for (UEdGraphPin* LinkedPin : InPin->LinkedTo)
	{
		check(LinkedPin->GetOwningNode());
		UGraphNodeDialogue* LinkedNode =
			Cast<UGraphNodeDialogue>(
				LinkedPin->GetOwningNode()
			);

		if (LinkedNode)
		{
			FBreakLinkActionInfo CurrentBreakAction;

			CurrentBreakAction.Label = LinkedNode->GetNodeTitle(
				ENodeTitleType::MenuTitle
			);

			CurrentBreakAction.Action = FUIAction(
				FExecuteAction::CreateUObject(
					this,
					&UDialogueEdGraphSchema::BreakSinglePinLink,
					InPin,
					LinkedPin
				)
			);

			BreakActions.Add(CurrentBreakAction);
		}
	}

	return BreakActions;
}

FNewToolMenuDelegate UDialogueEdGraphSchema::GetBreakLinkSubMenuDelegate(
	TArray<FBreakLinkActionInfo>& BreakActions) const
{
	FText BreakLinkTooltip = LOCTEXT(
		"BreakLinkTooltip",
		"Break the link to the specified node..."
	);

	return FNewToolMenuDelegate::CreateLambda(
		[BreakActions, BreakLinkTooltip] (UToolMenu* SubMenu)
		{
			FToolMenuSection& Section = SubMenu->AddSection(
				NAME_None,
				LOCTEXT("BreakLinkSectionLabel", "Break link to")
			);

			for (FBreakLinkActionInfo Action : BreakActions)
			{
				Section.AddMenuEntry(
					NAME_None,
					Action.Label,
					BreakLinkTooltip,
					FSlateIcon(),
					Action.Action
				);
			}
		}
	);
}

void UDialogueEdGraphSchema::GetNodeContextMenu(UToolMenu* Menu) const
{
	FToolMenuSection& NodeMenuSection = Menu->AddSection(
		FName(TEXT("DialogueTreeGraphSchemaNodeActions")), 
		LOCTEXT("NodeActionCategory", "Node Actions")
	);

	NodeMenuSection.AddMenuEntry(FGenericCommands::Get().Delete);
	NodeMenuSection.AddMenuEntry(FGenericCommands::Get().Cut);
	NodeMenuSection.AddMenuEntry(FGenericCommands::Get().Copy);
	NodeMenuSection.AddMenuEntry(FGenericCommands::Get().Duplicate);
	NodeMenuSection.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
}

void UDialogueEdGraphSchema::GetSpeechNodeMenuActions(
	FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	//Fetch the current graph 
	const UDialogueEdGraph* TargetGraph = 
		Cast<UDialogueEdGraph>(ContextMenuBuilder.CurrentGraph);

	if (!TargetGraph)
	{
		return;
	}

	//Loop through the list of speakers 
	for (UDialogueSpeakerSocket* Speaker : TargetGraph->GetAllSpeakers())
	{
		if (Speaker->GetSpeakerName().IsNone())
		{
			continue;
		}

		//Add one create node action for each transition type 
		for (TObjectIterator<UClass> TypeIterator; TypeIterator; ++TypeIterator)
		{
			if (TypeIterator->IsChildOf(UDialogueTransition::StaticClass())
				&& !TypeIterator->HasAnyClassFlags(CLASS_Abstract))
			{
				TSubclassOf<UDialogueTransition> CurrentType =
					*TypeIterator;

				TSharedPtr<FNewDialogueNodeAction> NewNodeAction =
					MakeCreateSpeechNodeAction(
						Speaker,
						CurrentType,
						ContextMenuBuilder.OwnerOfTemporaries
					);
				
				//Create action and add to menu 
				ContextMenuBuilder.AddAction(NewNodeAction);
			}
		}
	}
}

TSharedPtr<FNewDialogueNodeAction> UDialogueEdGraphSchema::
	MakeCreateSpeechNodeAction(UDialogueSpeakerSocket* Speaker, 
		TSubclassOf<UDialogueTransition> TransitionType, UObject* Outer) const
{
	check(Speaker && TransitionType && Outer);

	//Get context menu text
	UDialogueTransition* DefaultTransitionObj =
		TransitionType->GetDefaultObject<UDialogueTransition>();

	FText MenuCategory = LOCTEXT("SpeechNodeMenuCategory",
		"SpeechNodes");
	FText MenuText = FText::Format(
		LOCTEXT("SpeechNodeCreationText", "{0}, {1}"),
		FText::FromName(Speaker->GetSpeakerName()),
		DefaultTransitionObj->GetDisplayName()
	);
	FText MenuTooltip =
		DefaultTransitionObj->GetNodeCreationTooltip();

	//Get template node
	UGraphNodeDialogueSpeech* TemplateObject =
		UGraphNodeDialogueSpeech::MakeTemplate(Outer, Speaker, TransitionType);
	check(TemplateObject);

	//Assemble action 
	TSharedPtr<FNewDialogueNodeAction> NewAction(
		new FNewDialogueNodeAction(
			MenuCategory,
			MenuText,
			MenuTooltip,
			TemplateObject
		)
	);

	return NewAction;
}

void UDialogueEdGraphSchema::GetConditionalNodeMenuActions(
	FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	//Category
	FText MenuCategory = LOCTEXT("ConditionalNodeMenuCategory",
		"Conditionals");

	//Add branch node 
	UGraphNodeDialogueBranch* TemplateBranch = 
		UGraphNodeDialogueBranch::MakeTemplate(
			ContextMenuBuilder.OwnerOfTemporaries
		);

	FText MenuText = LOCTEXT(
		"BranchNodeCreationText",
		"Branch"
	);
	FText MenuTooltip = LOCTEXT(
		"BranchNodeCreationTooltip",
		"Creates an if/else style branch node."
	);

	TSharedPtr<FNewDialogueNodeAction> NewAction(new FNewDialogueNodeAction(
		MenuCategory,
		MenuText,
		MenuTooltip,
		TemplateBranch
	));
	ContextMenuBuilder.AddAction(NewAction);
}

void UDialogueEdGraphSchema::GetEventNodeMenuActions(
	FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	//Category
	FText MenuCategory = LOCTEXT("EventNodeMenuCategory",
		"Events");

	//Add event node 
	UGraphNodeDialogueEvent* TemplateEvent =
		UGraphNodeDialogueEvent::MakeTemplate(
			ContextMenuBuilder.OwnerOfTemporaries
		);

	FText MenuText = LOCTEXT("EventNodeCreationText",
		"Event");
	FText MenuTooltip = LOCTEXT("EventNodeCreationTooltip",
		"Creates a node that can play dialogue events.");

	TSharedPtr<FNewDialogueNodeAction> NewAction(new FNewDialogueNodeAction(
		MenuCategory,
		MenuText,
		MenuTooltip,
		TemplateEvent
	));
	ContextMenuBuilder.AddAction(NewAction);
}

void UDialogueEdGraphSchema::GetJumpNodeMenuActions(
	FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	//Category
	FText MenuCategory = LOCTEXT("JumpNodeMenuCategory",
		"Jump");

	//Add event node 
	UGraphNodeDialogueJump* TemplateJump =
		UGraphNodeDialogueJump::MakeTemplate(
			ContextMenuBuilder.OwnerOfTemporaries
		);

	FText MenuText = LOCTEXT(
		"JumpNodeCreationText",
		"Jump"
	);
	FText MenuTooltip = LOCTEXT(
		"JumpNodeCreationTooltip",
		"Creates a node which jumps control to a specified other node."
	);

	TSharedPtr<FNewDialogueNodeAction> NewAction(new FNewDialogueNodeAction(
		MenuCategory,
		MenuText,
		MenuTooltip,
		TemplateJump
	));
	ContextMenuBuilder.AddAction(NewAction);
}

void UDialogueEdGraphSchema::BreakPinLinksContextMenu(
	UEdGraphPin* TargetPin) const
{
	BreakPinLinks(*TargetPin, true);
}

void UDialogueEdGraphSchema::CheckForDisallowedConnection(
	const FConnectionArgs& InArgs, FPinConnectionResponse& OutResponse) const
{
	check(InArgs.PinA && InArgs.PinB);

	//Same pin direction
	if (InArgs.PinA->Direction == InArgs.PinB->Direction)
	{
		OutResponse.Response = CONNECT_RESPONSE_DISALLOW;
		OutResponse.Message = LOCTEXT(
			"SamePinDirConnectionError",
			"Cannot connect two input or two output pins"
		);
	}
	//Invalid or missing nodes
	else if (!InArgs.NodeA || !InArgs.NodeB)
	{
		OutResponse.Response = CONNECT_RESPONSE_DISALLOW;
		OutResponse.Message = LOCTEXT(
			"WrongNodeTypeConnectionError",
			"No connectable nodes found extending UGraphNodeDialogueBase"
		);
	}
	//Attempting to connect to self 
	else if (InArgs.NodeA == InArgs.NodeB)
	{
		OutResponse.Response = CONNECT_RESPONSE_DISALLOW;
		OutResponse.Message = LOCTEXT(
			"SelfConnectConnectionError",
			"Unable to connect node to itself"
		);
	}
	//Attempting to connect to direct parent 
	else if (NodeIsDirectParent(InArgs.NodeA, InArgs.NodeB))
	{
		OutResponse.Response = CONNECT_RESPONSE_DISALLOW;
		OutResponse.Message = LOCTEXT(
			"DirectParentConnectionError",
			"Unable to connect node to its direct parent"
		);
	}
}

bool UDialogueEdGraphSchema::NodeIsDirectParent(
	const UGraphNodeDialogueBase* NodeA, 
	const UGraphNodeDialogueBase* NodeB) const
{
	check(NodeA && NodeB);

	//If either node is a direct parent of the other
	const TArray<UGraphNodeDialogueBase*> ParentNodesA =
		NodeA->GetDirectParents();
	const TArray<UGraphNodeDialogueBase*> ParentNodesB =
		NodeB->GetDirectParents();

	if (ParentNodesB.Contains(NodeA)
		|| ParentNodesA.Contains(NodeB))
	{
		return true;
	}

	return false;
}

bool UDialogueEdGraphSchema::ConnectionHasConnectionLimit(
	const FConnectionArgs& InArgs, FPinConnectionResponse& OutResponse) const
{
	check(InArgs.NodeA && InArgs.NodeB);

	//If either node has a connection limit, respect it 
	bool bASinglyConnectable = InArgs.NodeA->GetOutputConnectionLimit() 
		== EDialogueConnectionLimit::Single;
	bool bBSinglyConnectable = InArgs.NodeB->GetInputConnectionLimit() 
		== EDialogueConnectionLimit::Single;

	if (bASinglyConnectable && bBSinglyConnectable)
	{
		OutResponse.Response =
			CONNECT_RESPONSE_BREAK_OTHERS_AB;
		OutResponse.Message = LOCTEXT(
			"AllowConnectionABExclusive",
			"Connect nodes"
		);
	}
	else if (bASinglyConnectable)
	{
		OutResponse.Response = CONNECT_RESPONSE_BREAK_OTHERS_A;
		OutResponse.Message = LOCTEXT(
			"AllowConnectionAExclusive",
			"Connect nodes"
		);
	}
	else if (bBSinglyConnectable)
	{
		OutResponse.Response = CONNECT_RESPONSE_BREAK_OTHERS_B;
		OutResponse.Message = LOCTEXT(
			"AllowConnectionBExclusive",
			"Connect nodes"
		);
	}

	return bASinglyConnectable || bBSinglyConnectable;
}

void UDialogueEdGraphSchema::PlaceRerouteNode(UEdGraph* InParentGraph,
	const FVector2D& InLocation, UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	//Get the adjusted spawn location
	const FVector2D NodeOffset(21.0f, 12.0f);
	const FVector2D RerouteTopLeft = InLocation - NodeOffset;

	//Create and connect reroute node
	UGraphNodeDialogueReroute* NewReroute = 
		GetRerouteNode(InParentGraph, RerouteTopLeft);
	ConnectRerouteNode(NewReroute, PinA, PinB);
}

UGraphNodeDialogueReroute* UDialogueEdGraphSchema::GetRerouteNode(
	UEdGraph* InParentGraph, const FVector2D& InRerouteTopLeft) const
{
	UGraphNodeDialogueReroute* NewReroute =
		NewObject<UGraphNodeDialogueReroute>(InParentGraph);

	//Setup with parent graph 
	NewReroute->SetFlags(RF_Transactional);
	InParentGraph->AddNode(NewReroute, true, true /** Select new node*/);
	NewReroute->CreateNewGuid();
	NewReroute->PostPlacedNewNode();

	//Set new node's position
	NewReroute->NodePosX = InRerouteTopLeft.X;
	NewReroute->NodePosY = InRerouteTopLeft.Y;
	NewReroute->SnapToGrid(GetDefault<UEditorStyleSettings>()->GridSnapSize);

	//Setup pins 
	NewReroute->AllocateDefaultPins();

	return NewReroute;
}

void UDialogueEdGraphSchema::ConnectRerouteNode(
	UGraphNodeDialogueReroute* InRerouteNode, UEdGraphPin* PinA, 
	UEdGraphPin* PinB) const
{
	//Get reroute input and output
	UEdGraphPin* NodeInputPin = InRerouteNode->GetInputPins()[0];
	UEdGraphPin* NodeOutputPin = InRerouteNode->GetOutputPins()[0];
	check(NodeInputPin && NodeOutputPin);

	//Connect the reroute
	PinA->BreakLinkTo(PinB);
	PinA->MakeLinkTo(
		PinA->Direction == EGPD_Output ? NodeInputPin : NodeOutputPin
	);
	PinB->MakeLinkTo(
		PinB->Direction == EGPD_Output ? NodeInputPin : NodeOutputPin
	);
}

#undef LOCTEXT_NAMESPACE
