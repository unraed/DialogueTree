// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
//Plugin
#include "DialogueConnectionLimit.h"
//Generated
#include "GraphNodeDialogueBase.generated.h"

class UDialogueNode;

/**
 * Abstract base class for all dialogue graph nodes. This includes redirects
 * as well as true content nodes. 
 */
UCLASS(Abstract)
class DIALOGUETREEEDITOR_API UGraphNodeDialogueBase : public UEdGraphNode
{
	GENERATED_BODY()

public: 
	/** Constructor */
	UGraphNodeDialogueBase();

public:

	/** UEdGraphNode Implementation */
	virtual void AutowireNewNode(UEdGraphPin* FromPin);
	/** End UEdGraphNode */

	/**
	* Gets the connection limit associated with the node's input pins. Const.
	* 
	* @return EDialogueNodeConnectionLimit - the node's input pin connection 
	* limit. 
	*/
	virtual EDialogueConnectionLimit GetInputConnectionLimit() const;

	/**
	* Gets the connection limit associated with the node's output pins. Const.
	*
	* @return EDialogueNodeConnectionLimit - the node's output pin connection
	* limit.
	*/
	virtual EDialogueConnectionLimit GetOutputConnectionLimit() const;

	/**
	* Retrieves an array of all the node's input pins. Const. 
	* 
	* @return TArray<UEdGraphPin*> - the node's input pins. 
	*/
	virtual TArray<UEdGraphPin*> GetInputPins() const;

	/**
	* Retrieves an array of all the node's output pins. Const. 
	* 
	* @return TArray<UEdGraphPin*> - the node's output pins. 
	*/
	virtual TArray<UEdGraphPin*> GetOutputPins() const;

	/**
	* Retrieves all the node's direct parents. Includes redirects. Const. 
	* 
	* @return const TArray<UGraphNodeDialogueBase*> - found parents. 
	*/
	virtual const TArray<UGraphNodeDialogueBase*> GetDirectParents() const;

	/**
	* Retrieves all the node's direct children. Includes redirects. Const.
	*
	* @return const TArray<UGraphNodeDialogueBase*> - found children.
	*/
	virtual const TArray<UGraphNodeDialogueBase*> GetDirectChildren() const;

	/**
	* Retrieves the text label associated with the pin at the given index. 
	* 
	* @param PinIndex - int32, index of the target pin. 
	* @return FText - label to use for the given pin when needed. 
	*/
	virtual FText GetPinMenuLabel(int32 PinIndex) const;

	/**
	* Gets the context menu display name for the node. Used when spawning nodes
	* in. 
	* 
	* @return FText - context menu display name. 
	*/
	virtual FText GetContextMenuName() const;

private:
	/**
	* Retrieves all nodes connected to the given pin. Includes redirects. Const.
	* 
	* @param SourcePin - UEdGraphPin*, the target pin.
	* @return TArray<UGraphNodeDialogueBase*> - the nodes connected to the 
	* SourcePin. 
	*/
	const TArray<UGraphNodeDialogueBase*> GetPinConnectedNodes(
		UEdGraphPin* SourcePin) const;

	/**
	* Retrieves all of the node's pins that match the target direction. Const.
	* 
	* @param Direction - EEdGraphPinDirection we are interested in.
	* @return TArray<UEdGraphPin*> - the pins matching the direction. 
	*/
	TArray<UEdGraphPin*> GetPinsWithDirection(EEdGraphPinDirection Direction)
		const;
};
