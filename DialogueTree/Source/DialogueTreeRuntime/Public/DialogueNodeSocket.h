// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//Generated
#include "DialogueNodeSocket.generated.h"

class UDialogueNode;

/**
 * Proxy object used to link dialogue nodes to their corresponding nodes in 
 * the graph editor. Useful for pickable nodes and such. 
 */
UCLASS()
class DIALOGUETREERUNTIME_API UDialogueNodeSocket : public UObject
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	/**
	* Sets the graph node to the provided edgraph node. 
	* 
	* @param InGraphNode - UEdGraphNode*, the new graph node. 
	*/
	void SetGraphNode(UEdGraphNode* InGraphNode);

	/**
	* Gets the graph node. 
	* 
	* @return UEdGraphNode*, the graph node. 
	*/
	UEdGraphNode* GetGraphNode();
#endif

	/**
	* Sets the display ID to the provided text.
	*
	* @param InText - FText, the new display text.
	*/
	void SetDisplayID(FText InText);

	/**
	* Gets the display ID for the current graph node.
	*
	* @return FText - the display ID.
	*/
	FText GetDisplayID();

	/**
	* Sets the dialogue node to the provided node. 
	* 
	* @param InNode - UDialogueNode*, the new node. 
	*/
	void SetDialogueNode(UDialogueNode* InNode);

	/**
	* Gets the dialogue node. 
	* 
	* @return UDialogueNode*, the dialogue node.
	*/
	UDialogueNode* GetDialogueNode();
	
private:
#if WITH_EDITORONLY_DATA
	/** The graph node associated with the socket */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TObjectPtr<UEdGraphNode> GraphNode = nullptr;
#endif 
	/** The display name for the graph node */
	UPROPERTY()
	FText DisplayID;

	/** The actual dialogue node associated with the socket */
	UPROPERTY()
	TObjectPtr<UDialogueNode> DialogueNode = nullptr;
};
