// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "ToolMenuDelegates.h"
//Generated
#include "DialogueEdGraphSchema.generated.h"

class FConnectionDrawingPolicy;
class FDialogueEditor;
class FSlateWindowElementList;
class UEdGraph;
class UDialogue;
class UDialogueSpeakerSocket;
class UDialogueTransition;
class UGraphNodeDialogue;
class UGraphNodeDialogueBase;
class UGraphNodeDialogueReroute;
class UGraphNodeDialogueSpeech;

/** 
* Struct defines data for actions that break pin links
*/
USTRUCT()
struct FBreakLinkActionInfo
{
	GENERATED_BODY() 

	FText Label;
	FUIAction Action;
};

/**
* Schema action for creating a new node. 
*/
USTRUCT()
struct DIALOGUETREEEDITOR_API FNewDialogueNodeAction : 
	public FEdGraphSchemaAction
{
public:
	GENERATED_USTRUCT_BODY()

	/** Default Constructor */
	FNewDialogueNodeAction();

	/** 
	* Constructor
	* @param InNodeCategory - FText, Node category.  
	* @param InMenuDesc - FText, Menu description.
	* @param InToolTip - FText, Tooltip. 
	* @param InTemplateNode - UGraphNodeDialogue*, node to spawn.  
	*/
	FNewDialogueNodeAction(FText InNodeCategory, FText InMenuDesc,
		FText InToolTip, UGraphNodeDialogue* InTemplateNode);

private:
	/** Node to build up and spawn */
	UPROPERTY()
	TObjectPtr<UGraphNodeDialogue> TemplateNode;

public:
	/** FEdGraphSchemaAction Implementation */
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph,
		UEdGraphPin* FromPin, const FVector2D Location, 
		bool bSelectNewNode = true) override;
	
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	/** End FEdGraphSchemaAction */

	/**
	* Sets the template node to the provided node. Setting this in the 
	* constructor is preferred. 
	* 
	* @param InNode - UGraphNodeDialogue*, template node to spawn. 
	*/
	void SetTemplateNode(UGraphNodeDialogue* InNode);

private: 
	/**
	* Adds the template node to the graph. 
	* 
	* @param InParentGraph - UEdGraph*, graph we are adding the node to. 
	* @param InLocation - FVector2D, target location.
	* @param FromPin - The pin that was dragged off to create the node.
	*/
	void PlaceNodeInGraph(UEdGraph* InParentGraph, const FVector2D InLocation, 
		UEdGraphPin* FromPin);

	/**
	* Sets up the node's location in the graph after being placed.
	* 
	* @param InLocation - FVector2D, target location. 
	*/
	void SetNodeLocation(const FVector2D InLocation);
};

/**
 * Schema that controls common behaviors for the dialogue graph.
 */
UCLASS()
class DIALOGUETREEEDITOR_API UDialogueEdGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	/** UEdGraphSchema Implementation */
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(
		int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, 
		const FSlateRect& InClippingRect, 
		FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) 
		const override;
	virtual void OnPinConnectionDoubleCicked(UEdGraphPin* PinA, 
		UEdGraphPin* PinB, const FVector2D& GraphPosition) const override;
	virtual void GetGraphContextActions(
		FGraphContextMenuBuilder& ContextMenuBuilder) const override; 
	virtual void GetContextMenuActions(class UToolMenu* Menu, 
		class UGraphNodeContextMenuContext* Context) const override; 
	virtual const FPinConnectionResponse CanCreateConnection(
		const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) 
		const override;
	virtual bool IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID)
		const override;
	virtual int32 GetCurrentVisualizationCacheID() const override;
	virtual void ForceVisualizationCacheClear() const override;
	/** End UEdGraphSchema */

	/**
	* Static method used to retrieve the editor associated with a dialogue 
	* graph. Currently used by the paste action. 
	* 
	* @param InGraph - UEdGraph*, the target graph. 
	* @return FDialogueEditor*, the found dialogue editor. 
	*/
	static TSharedPtr<FDialogueEditor> GetGraphEditor(const UEdGraph* InGraph);

private:
	/**
	* Sets up the context menu entry associated with the given
	* pin. 
	* 
	* @param Menu - UToolMenu* we are adding the context menu to.
	* @param InPin - The target pin. 
	* @param PinText - FText, display text for the pin. 
	*/
	void GetPinContextMenu(UToolMenu* Menu, UEdGraphPin* InPin, FText PinText) 
		const;

	/**
	* Adds an option to the given FToolMenuSection that breaks all links on the
	* given pin. 
	* 
	* @param MenuSection - FToolMenuSection&, the menu section we are extending.
	* @param InPin - UEdGraphPin*, the pin we want the option to break all links
	* on. 
	*/
	void GetBreakAllPinLinksOption(FToolMenuSection& MenuSection, 
		UEdGraphPin* InPin) const;

	/**
	* Adds the options to break a pin's links indiviudally. 
	* 
	* @param MenuSection - FToolMenuSection&, the menu section we are extending.
	* @param InPin - UEdGraphPin*, the pin we want the option to break all links
	* on. 
	*/
	void GetBreakIndividualPinLinksOption(FToolMenuSection& MenuSection, 
		UEdGraphPin* InPin) const;

	/**
	* Creates the actual actions used to break a pin's links. 
	* 
	* @param InPin - UEdGraphPin*, the pin we want the option to break all links
	* on. 
	* @return TArray<FBreakLinkActionInfo> - the retrieved actions. 
	*/
	TArray<FBreakLinkActionInfo> GetBreakLinkActions(UEdGraphPin* InPin) const;

	/**
	* Creates the submenu delegate used to populate the break individual pin 
	* link menu. 
	* 
	* @param BreakActions - TArray<FBreakLinkActionInfo>, the break actions. 
	* @return FNewToolMenuDelegate - the completed delegate. 
	*/
	FNewToolMenuDelegate GetBreakLinkSubMenuDelegate(
		TArray<FBreakLinkActionInfo>& BreakActions) const;

	/**
	* Sets up the context menu entry associated with nodes. 
	* 
	* @param Menu - UToolMenu*, the menu to add to. 
	*/
	void GetNodeContextMenu(UToolMenu* Menu) const;

	/**
	* Sets up context menu for speech node creation. 
	* 
	* @param ContextMenuBuilder - FGraphContextMenuBuilder&, builder of the 
	* current context menu. 
	*/
	void GetSpeechNodeMenuActions(FGraphContextMenuBuilder& ContextMenuBuilder)
		const;

	/**
	* Creates and sets up a New Speech Node Action for use in the context menu. 
	* 
	* @param Speaker - UDialogueSpeakerSocket*, the speaker for the node.
	* @param TransitionType - TSubclassOf<UDialogueTransition>, the type of 
	* transition for the node. 
	* @param Outer - UObject*, the outer. 
	* @return TSharedPtr<FNewDialogueNodeAction>, the node spawner action. 
	*/
	TSharedPtr<FNewDialogueNodeAction> MakeCreateSpeechNodeAction(
		UDialogueSpeakerSocket* Speaker, 
		TSubclassOf<UDialogueTransition> TransitionType, 
		UObject* Outer) const;

	/**
	* Sets up context menu for conditional node creation.
	*
	* @param ContextMenuBuilder - FGraphContextMenuBuilder&, builder of the
	* current context menu.
	*/
	void GetConditionalNodeMenuActions(FGraphContextMenuBuilder& 
		ContextMenuBuilder) const;

	/**
	* Sets up context menu for event node creation.
	*
	* @param ContextMenuBuilder - FGraphContextMenuBuilder&, builder of the
	* current context menu.
	*/
	void GetEventNodeMenuActions(FGraphContextMenuBuilder& ContextMenuBuilder) 
		const;

	/**
	* Sets up context menu for jump node creation.
	*
	* @param ContextMenuBuilder - FGraphContextMenuBuilder&, builder of the
	* current context menu.
	*/
	void GetJumpNodeMenuActions(FGraphContextMenuBuilder& ContextMenuBuilder) 
		const;

	/**
	* Sets up context menu for option lock node creation.
	*
	* @param ContextMenuBuilder - FGraphContextMenuBuilder&, builder of the
	* current context menu.
	*/
	void GetOptionLockNodeMenuActions(FGraphContextMenuBuilder&
		ContextMenuBuilder) const;

	/**
	* Sets up context menu for breaking a pin's links. Note: this proved 
	* necessary because pushing the pins out of the node prevents them being 
	* right clicked and thus requires a custom pin link breakage menu. 
	*
	* @param TargetPin - UEdGraphPin* whose links we want to break.
	*/
	void BreakPinLinksContextMenu(UEdGraphPin* TargetPin) const;

	/**
	* Places a reroute node in the graph and connects it to the given pins. 
	* 
	* @param InParentGraph - UEdGraph*, parent graph.  
	* @param InLocation - const FVector2D&, the graph location to spawn the 
	* node at. 
	* @param PinA - UEdGraphPin*, from pin. 
	* @param PinB - UEdGraphPin*, to pin. 
	*/
	void PlaceRerouteNode(UEdGraph* InParentGraph, const FVector2D& InLocation,
		UEdGraphPin* PinA, UEdGraphPin* PinB) const;

	/**
	* Creates a reroute node and initializes it in the graph at the given 
	* location. 
	* 
	* @param InParentGraph - UEdGraph*, parent graph. 
	* @param InRerouteTopLeft - const FVector2D&, the spawn position in the 
	* graph, adjusted for the size of the reroute node. 
	*/
	UGraphNodeDialogueReroute* GetRerouteNode(UEdGraph* InParentGraph, 
		const FVector2D& InRerouteTopLeft) const;

	/**
	* Connects a new reroute node to the given pins. 
	* 
	* @param InRerouteNode - UGraphNodeDialogueReroute*, new node. 
	* @param PinA - UEdGraphPin*, from pin. 
	* @param PinB - UEdGraphPin*, to pin. 
	*/
	void ConnectRerouteNode(UGraphNodeDialogueReroute* InRerouteNode, 
		UEdGraphPin* PinA, UEdGraphPin* PinB) const;

	/** Arguments for creating a connection */
	struct FConnectionArgs
	{
		const UEdGraphPin* PinA;
		const UEdGraphPin* PinB;
		const UGraphNodeDialogueBase* NodeA;
		const UGraphNodeDialogueBase* NodeB;
	};

	/**
	* Checks for a situation where the connection of two pins should not be 
	* permitted. If one is found, the OutResponse's response is changed to 
	* disallowed and its message is set to reflect the reason the connection 
	* could not be made. 
	* 
	* @param InArgs - const FConnectionArgs&, the pins and nodes under 
	* consideration.
	* @param OutResponse - FPinConnectionResponse&, the response we are 
	* populating. 
	*/
	void CheckForDisallowedConnection(const FConnectionArgs& InArgs, 
		FPinConnectionResponse& OutResponse) const;

	/**
	* Returns true if either provided node is the direct parent of the other. 
	* Used to disallow such connections. 
	* 
	* @param NodeA - const UGraphNodeDialogueBase* 
	* @param NodeB - const UGraphNodeDialogueBase* 
	* @return bool - whether one node is the direct parent of the other. 
	*/
	bool NodeIsDirectParent(const UGraphNodeDialogueBase* NodeA, 
		const UGraphNodeDialogueBase* NodeB) const;

	/**
	* Checks to see if the connection should be limited in number/potentially 
	* involve a breaking of other links. If so, updates the connection response 
	* and returns true. 
	* 
	* @param InArgs - const FConnectionArgs&, the pins and nodes we are trying
	* to connect. 
	* @param OutResponse - FPinConnectionResponse&, the response we are 
	* populating. 
	* @return bool - whether a connection limit is in effect. 
	*/
	bool ConnectionHasConnectionLimit(const FConnectionArgs& InArgs, 
		FPinConnectionResponse& OutResponse) const;

private:
	/** Id number used to check if graph visualization is dirty */
	static int32 CurrentCacheRefreshID;
};
