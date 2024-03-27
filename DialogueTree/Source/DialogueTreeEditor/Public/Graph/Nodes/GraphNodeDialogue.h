// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "GraphNodeDialogueBase.h"
//Generated
#include "GraphNodeDialogue.generated.h"

DECLARE_DELEGATE(FOnUpdateNode);

class UDialogueEdGraph;
class UDialogueNode;

/**
 * Abstract base node for all dialogue graph nodes that contain actual content.
 * Excludes redirects. 
 */
UCLASS(Abstract)
class DIALOGUETREEEDITOR_API UGraphNodeDialogue : public UGraphNodeDialogueBase
{
	GENERATED_BODY()

public:
	/** Constructor */
	UGraphNodeDialogue();

public:
	/** UObject Implementation */
	virtual void PostEditUndo() override;
	virtual void PostEditChangeProperty(
		struct FPropertyChangedEvent& PropertyChangedEvent) override;
	/** End UObject */

	/** UEdGraphNode Impl. */
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void ResizeNode(const FVector2D& NewSize) override;
	/** UEnd EdGraphNode */

	/**
	* Retrieves the node's ID.
	* 
	* @return FName - ID.
	*/
	FName GetID() const;
	
	/**
	* Retrieves the asset node for this graph node. 
	* 
	* @return UDialogueNode* - the asset node. 
	*/
	UDialogueNode* GetAssetNode() const;

	/**
	* Sets the asset node for this graph node to the provided dialogue node. 
	* 
	* @param InDialogueNode - UDialogueNode*, the new asset node. 
	*/
	void SetAssetNode(UDialogueNode* InDialogueNode);

	/**
	* Checks if this node can be compiled without problems. Virtual.
	* 
	* @return bool - True if the node can be compiled. False otherwise. 
	*/
	virtual bool CanCompileNode();

	/**
	* Retrieves the dialogue graph this node exists within. 
	* 
	* @return UDialogueEdGraph* - the dialogue graph. 
	*/
	UDialogueEdGraph* GetDialogueGraph() const;

	/**
	* Sets the delegate to use when updating the node's properties.
	* 
	* @param UpdateDelegate - FOnUpdateNode&, delegate to use to update the 
	* node's visuals. 
	*/
	void BindOnUpdateVisuals(FOnUpdateNode& UpdateDelegate);

	/**
	* Sets up a node within the dialogue graph. Primarily used
	* to set up the node ID and cache the graph when adding a new node. 
	* 
	* @param OwningGraph - UEdGraph*, the owning graph. 
	*/
	void InitNodeInDialogueGraph(UEdGraph* OwningGraph);

	/**
	* Regenerates the node's ID. 
	*/
	void ResetID();

	/**
	* Static. Sorts the provided nodes based on their horizontal position in
	* the graph from left to right. 
	* 
	* @param Nodes - TArray<UGraphNodeDialogue*>&, the nodes to sort. 
	*/
	static void SortNodesLeftToRight(TArray<UGraphNodeDialogue*>& Nodes);

	/**
	* Sets the error flag. 
	* 
	* @param InFlag - bool, the new value of the flag. 
	*/
	void SetErrorFlag(bool InFlag);

	/**
	* Checks if the node has a dialogue compilation error. 
	* 
	* @return bool - True if the node is flagged with an error. False otherwise.
	*/
	bool HasError() const;

	/**
	* Virtual. Retrieves the base ID associated with this type of node. 
	* 
	* @return FName - the base ID. 
	*/
	virtual FName GetBaseID() const;

	/**
	* Virtual. Generates the asset node associated with this graph node. 
	* 
	* @param InAsset - UDialogue*, the target dialogue asset. 
	*/
	virtual void CreateAssetNode(class UDialogue* InAsset) {};

	/**
	* Virtual. Performs last touches on the asset node after all asset nodes
	* have been created and linked together. 
	*/
	virtual void FinalizeAssetNode() {};

	/**
	* Links the asset node into the asset tree. 
	* 
	* @param InAsset - UDialogue*, the target dialogue asset. 
	*/
	void LinkAssetNode();

	/**
	* Clears the asset node. 
	*/
	void ClearAssetNode();

	/**
	* Retrieves all valid parent nodes. Excludes redirects. 
	* 
	* @param OutNodes - TArray<UGraphNodeDialogue*>, out parameter for found 
	* nodes.
	*/
	void GetParents(TArray<UGraphNodeDialogue*>& OutNodes) const;

	/**
	* Retrieves all valid child nodes. Excludes redirects.
	*
	* @param OutNodes - TArray<UGraphNodeDialogue*>, out parameter for found
	* nodes.
	*/
	void GetChildren(TArray<UGraphNodeDialogue*>& OutNodes) const;

	/**
	* Gets all children linked via the given pin. Excludes redirects. 
	* 
	* @param InPin - UEdGraphPin*, the target pin. 
	* @param OutNodes - TArray<UGraphNodeDialogue*>, out parameter for found
	* nodes.
	*/
	void GetPinChildren(UEdGraphPin* InPin, 
		TArray<UGraphNodeDialogue*>& OutNodes) const;

	/**
	* Marks the dialogue as needing to be compiled. 
	*/
	void MarkDialogueDirty();

	/**
	* Notifies subscribers that the node has changed. Used mainly to notify
	* Slate nodes. 
	*/
	virtual void UpdateDialogueNode();

protected:
	/**
	* Virtual. Links the asset node to the given parent node.
	*
	* @param InParent - UGraphNodeDialogue*, the target parent.
	*/
	void LinkToParent(UGraphNodeDialogue* InParent);

	/**
	* Virtual. Links the asset node to the given child node.
	*
	* @param InChild - UGraphNodeDialogue*, the target child.
	*/
	void LinkToChild(UGraphNodeDialogue* InChild);

private:
	/**
	* Helper used to recursively retrieve valid parent nodes. 
	* 
	* @param InParents - TArray<UGraphNodeDialogueBase*>, the current list of 
	* nodes we are recurring over. 
	* @param OutNodes - TArray<UGraphNodeDialogue*>&, out parameter for found 
	* nodes. 
	*/
	void GetParents(TArray<UGraphNodeDialogueBase*> InParents, 
		TArray<UGraphNodeDialogue*>& OutNodes) const;

	/**
	* Helper used to recursively retrieve valid child nodes.
	*
	* @param InParents - TArray<UGraphNodeDialogueBase*>, the current list of
	* nodes we are recurring over.
	* @param OutNodes - TArray<UGraphNodeDialogue*>&, out parameter for found
	* nodes.
	*/
	void GetChildren(TArray<UGraphNodeDialogueBase*> InChildren, 
		TArray<UGraphNodeDialogue*>& OutNodes) const;

private:
	/** The asset node associated with the graph node */
	UPROPERTY()
	UDialogueNode* AssetNode = nullptr;
	
	/** A flag indicating that the node has a compilation error */
	UPROPERTY()
	bool bDialogueError = false;

	/** The dialogue graph this node exists within */
	UPROPERTY()
	UDialogueEdGraph* DialogueGraph;

	/** The ID of the node within the graph */
	UPROPERTY()
	FName ID;

	/** Event delegate for when the node changes */
	FOnUpdateNode OnUpdateVisuals;

	/** Constants */
	const FLinearColor DEFAULT_GRAY = FLinearColor(105.f, 105.f, 105.f);
};
