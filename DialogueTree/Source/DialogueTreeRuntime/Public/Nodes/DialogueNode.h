// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//Plugin
#include "DialogueOption.h"
//Generated
#include "DialogueNode.generated.h"

class UDialogue;

/**
 * Abstract base class for all runtime dialogue nodes. 
 */
UCLASS(Abstract)
class DIALOGUETREERUNTIME_API UDialogueNode : public UObject
{
	GENERATED_BODY()

public:
	//Getters and Setters
	/**
	* Retrieves the owning dialogue. 
	* 
	* @return UDialogue*, owning dialogue. 
	*/
	UDialogue* GetDialogue() const;

	/**
	* Sets the owning dialogue to the given dialogue object. 
	* 
	* @param InDialogue - UDialogue*, dialogue to set as owner. 
	*/
	void SetDialogue(UDialogue* InDialogue);

	/**
	* Adds a dialogue node as a parent of this node. 
	* 
	* @param InParent - UDialogueNode*, parent node to add. 
	*/
	void AddParent(UDialogueNode* InParent);

	/**
	* Adds a dialogue node as a child of this node. 
	* 
	* @param InChild - UDialogueNode*, child node to add. 
	*/
	void AddChild(UDialogueNode* InChild);

	/**
	* Retrieves a TArray of all parents for this node. 
	* 
	* @return TArray<UDialogueNode*>, parent nodes. 
	*/
	TArray<UDialogueNode*> GetParents() const;

	/**
	* Retrieves a TArray of all children of this node. 
	* 
	* @return TArray<UDialogueNode*>, child nodes. 
	*/
	TArray<UDialogueNode*> GetChildren() const;

	/**
	* Gets an FDialogueOption struct representing this node as a
	* selectable option. 
	* 
	* @return FDialogueOption, option struct. 
	*/
	virtual FDialogueOption GetAsOption();

	/**
	* Plays standard behavior for the given node. 
	*/
	virtual void EnterNode() {};

	/**
	* Attempts to select the option at the given index, if 
	* applicable. 
	* 
	* @param InOptionIndex - int32, selection index. 
	*/
	virtual void SelectOption(int32 InOptionIndex) {};

	/**
	* If allowable, attempts to skip some or all of the node's 
	* content. 
	*/
	virtual void Skip() {};

	/**
	* Retrieves the id for the node in dialogue
	* 
	* @return FName - NodeID
	*/
	FName GetNodeID() const;

	/**
	* Sets the id of the node in the dialogue.
	*
	* @param Name, FName, the new id for the node
	*/
	void SetNodeID(FName InID);

protected:
	/** The owning dialogue */
	UPROPERTY()
	TObjectPtr<UDialogue> Dialogue;

	/** The ID of the node within the dialogue */
	UPROPERTY()
	FName NodeID;

	/** Any direct parent nodes in the dialogue */
	UPROPERTY()
	TArray<TObjectPtr<UDialogueNode>> Parents;

	/** Any direct child nodes in the dialogue */
	UPROPERTY()
	TArray<TObjectPtr<UDialogueNode>> Children;
};
