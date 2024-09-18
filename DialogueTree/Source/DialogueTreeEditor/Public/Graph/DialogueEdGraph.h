// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
//Generated
#include "DialogueEdGraph.generated.h"

class UDialogue;
class UDialogueSpeakerSocket;
class UGraphNodeDialogue;
class UGraphNodeDialogueBase;

/**
* Struct representing default colors in the dialogue graph. 
*/

/**
 * The graph the user uses to edit a dialogue. 
 */
UCLASS()
class DIALOGUETREEEDITOR_API UDialogueEdGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	/** Constructor */
	UDialogueEdGraph();

public:
	/** UObject Implementation */
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditUndo() override;
	virtual void PostInitProperties() override;
	/** End UObject */

	/**
	* Sets the graph's root to be the provided node. 
	* 
	* @param InRoot - UGraphNodeDialogue*, new root.
	*/
	void SetGraphRoot(UGraphNodeDialogue* InRoot);

	/**
	* Retrieves the dialogue associated with this graph. Const.
	* 
	* @return UDialogue*, the dialogue asset. 
	*/
	UDialogue* GetDialogue() const;

	/**
	* Adds the given node to the graph's map. 
	* 
	* @param InNode - UGraphNodeDialogue*, node to add. 
	*/
	void AddToNodeMap(UGraphNodeDialogue* InNode);

	/**
	* Removes the node with the given ID from the graph's node map. 
	* 
	* @param RemoveID - FName, ID of the node to remove. 
	*/
	void RemoveFromNodeMap(FName RemoveID);

	/**
	* Checks if the graph contains a node with the given ID. 
	* 
	* @param InID - FName, the ID of the node. 
	* @return bool - True if the node is contained by the graph. False 
	* otherwise. 
	*/
	bool ContainsNode(FName InID) const;

	/**
	* Retrieves the node with the given ID from the graph if present. 
	* 
	* @param InID - FName, the ID of the node.
	* @return UGraphNodeDialogue* - the found node or nullptr if none. 
	*/
	UGraphNodeDialogue* GetNode(FName InID) const;

	/**
	* Retrieves an array of all dialogue nodes contained by the graph. 
	* 
	* @return TArray<UGraphNodeDialogue*> - all nodes in the graph. 
	*/
	TArray<UGraphNodeDialogue*> GetAllNodes() const;

	/**
	* Checks if the graph has a speaker with the given name. 
	* 
	* @param InName - FName, the name of the speaker to check for. 
	* @return bool - True if the graph has the speaker. False otherwise. 
	*/
	bool HasSpeaker(FName InName) const;

	/**
	* Retrieves the color associated with the given speaker. White if no 
	* speaker is found. 
	* 
	* @param InName - FName of the target speaker. 
	* @return FColor - the color associated with the speaker in the graph. 
	*/
	FColor GetSpeakerColor(FName InName) const;

	/**
	* Retrieves an array of all speaker sockets owned by the graph. 
	* 
	* @return TArray<UDialogueSpeakerSocket*> - found speakers. 
	*/
	TArray<UDialogueSpeakerSocket*> GetAllSpeakers() const;

	/**
	* Attempts to compile the dialogue graph into its dialogue asset. Sets
	* the asset's compile status to compiled if successful and failed otherwise.
	*/
	void CompileAsset();

	/**
	* Used to determine successful compilation of the dialogue. Checks if the 
	* dialogue graph is valid and can therefore be compiled. 
	* 
	* @return bool - True if the dialogue can be compiled without issues, false
	* otherwise. 
	*/
	bool CanCompileAsset() const;

	/**
	* Refreshes the visual representations of all nodes in the graph. 
	*/
	void UpdateAllNodeVisuals();

private: 
	/**
	* Clears the asset nodes for all graph nodes.
	*/
	void ClearAssetNodes();

	/**
	* Generates the asset nodes associated with each graph node in the 
	* dialogue graph. Used during compilation of the dialogue asset. 
	* 
	* @param InAsset - UDialogue*, asset to populate. 
	*/
	void CreateAssetNodes(UDialogue* InAsset);

	/**
	* Performs any final steps associated with compiling the various nodes
	* in the graph into their asset node equivalents.
	*/
	void FinalizeAssetNodes();

	/**
	* Traverse the tree of graph nodes recursively, linking up their asset 
	* nodes to construct an equivalent tree in the dialogue asset. 
	* 
	* @param InRoot - UGraphNodeDialogue*, the root of the current subtree.
	* @param VisitedNodes - TSet<UGraphNodeDialogue*>, the nodes we have
	* already visited. 
	*/
	void UpdateAssetTreeRecursive(UGraphNodeDialogue* InRoot, 
		TSet<UGraphNodeDialogue*> VisitedNodes);

	/**
	* Behaviors to trigger when the graph changes. 
	* 
	* @param EditAction - const FEdGraphEditAction&, the action that changed the
	* graph. 
	*/
	void OnDialogueGraphChanged(const FEdGraphEditAction& EditAction);

	/**
	* Function event to call when the speakers change. 
	*/
	UFUNCTION()
	void OnSpeakerRolesChanged();

private:
	/** The root node of the graph, where the dialogue starts playing */
	UPROPERTY()
	TObjectPtr<UGraphNodeDialogue> Root;

	/** The collection of dialogue nodes, keyed to their IDs for easy access */
	UPROPERTY()
	TMap<FName, TObjectPtr<UGraphNodeDialogue>> NodeMap;
};
