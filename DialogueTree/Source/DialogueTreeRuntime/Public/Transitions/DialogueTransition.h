// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "TimerManager.h"
#include "UObject/NoExportTypes.h"
//Plugin
#include "DialogueConnectionLimit.h"
//Generated
#include "DialogueTransition.generated.h"

class UDialogueSpeechNode;

/**
 * Abstract base class for speech transitions. Transitions govern
 * how and when a speech node transfers control on to a child node. 
 */
UCLASS(Abstract)
class DIALOGUETREERUNTIME_API UDialogueTransition : public UObject
{
	GENERATED_BODY()

public:
	//Constructor
	UDialogueTransition();
	
public:
	/**
	* Sets the owning node. 
	* @param InNode - UDialogueSpeechNode*, node to set.
	*/
	void SetOwningNode(UDialogueSpeechNode* InNode);

	/**
	* Called when traversing a node. Performs any initial 
	* behavior for the transition and queues up any deferred 
	* actions. 
	*/
	virtual void StartTransition();

	/**
	* Concludes the transition. Largely defined by child transitions.
	*/
	virtual void TransitionOut() {};

	/**
	* Called when the user selects an option. Transitions to the 
	* option with the given index, if applicable. 
	* 
	* @param InOptionIndex - int32, the index of the selection. 
	*/
	virtual void SelectOption(int32 InOptionIndex) {};

	/**
	* Attemps to skip the currently playing speech. Base
	* implementation stops the speech audio. 
	*/
	virtual void Skip();

	/**
	* Retrieves the display name for the transition. 
	* 
	* @return FText, display name. 
	*/
	virtual FText GetDisplayName() const;

	/**
	* Retrieves the tooltip to display when creating a node with 
	* this transition. 
	* 
	* @return FText, creation tooltip. 
	*/
	virtual FText GetNodeCreationTooltip() const;

	/**
	* Retrieves the outgoing connection limit for the node's output pin. 
	* Negative values indicate no limit. 
	* 
	* @return EDialogueConnectionLimit, the number of nodes we can connect
	* the transition out to. 
	*/
	virtual EDialogueConnectionLimit GetConnectionLimit() const;

	/**
	* Checks if the transition should exit, and triggers the transition out
	* if so.
	*/
	void CheckTransitionConditions();

private:

	/**
	* Called when the speech content has finished playing.
	*/
	UFUNCTION()
	void OnDonePlayingContent();

	/**
	* Called when the minimum play time has elapsed.
	*/
	UFUNCTION()
	void OnMinPlayTimeElapsed();

protected:
	/** The node upon which the transition operates*/
	UPROPERTY()
	TObjectPtr<UDialogueSpeechNode> OwningNode;

	/** Whether the min play time has elapsed yet */
	bool bMinPlayTimeElapsed = false;

	/** Whether the audio content has finished playing yet */
	bool bAudioFinished = false;

private:
	/** Delegate to call when the minimum play time for the node has elapsed */
	FTimerDelegate OnTimerEnd;

	/** Timer handle for timer that tracks minimum play time */
	FTimerHandle MinPlayTimeHandle;

	/** Delegate to call when the speech has finished playing */
	FScriptDelegate OnContentEnd;
};
