// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueEventBase.generated.h"

class UDialogue;

DECLARE_DELEGATE(FDialogueEventSignature);

/**
 * Base class for dialogue events. Does not require a speaker. 
 */
UCLASS(Abstract, EditInlineNew)
class DIALOGUETREERUNTIME_API UDialogueEventBase : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	* Checks if the event has all of the information it needs to compile.
	*
	* @return bool - True if all requisite data is present; false otherwise.
	*/
	virtual bool HasAllRequirements() const;

	/**
	* Gets the graph description text for the event.
	* 
	* @return FText - the description.
	*/
	UFUNCTION(BlueprintNativeEvent)
	FText GetGraphDescription() const;
	virtual FText GetGraphDescription_Implementation() const;

	/**
	* Checks if the event is currently trying to block/in progress. 
	* 
	* @return bool - True if blocking, False otherwise. 
	*/
	UFUNCTION(BlueprintPure, Category="DialogueEvent")
	bool GetIsBlocking() const;

	/**
	* Sets the blocking status of the event to true. Where possible, the 
	* dialogue will attempt to wait until the event completes. StopBlocking() 
	* should be called to free up the dialogue to continue. 
	*/
	UFUNCTION(BlueprintCallable, Category="DialogueEvent")
	void StartBlocking();

	/**
	* Marks the event as complete/frees up the dialogue to continue. Only 
	* needs to be called if StartBlocking() has been called first. 
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueEvent")
	void StopBlocking();

	/**
	* User specified behavior for when a speech the event is attached
	* to gets skipped.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Dialogue")
	void OnSkipped();
	virtual void OnSkipped_Implementation() {};

	/**
	* Triggers the event.
	*/
	virtual void PlayEvent();

	/**
	* Sets the event's owning dialogue.
	*
	* @param InDialogue - UDialogue*, owning dialogue.
	*/
	void SetDialogue(UDialogue* InDialogue);

protected:
	/** Dialogue owning this event */
	UPROPERTY()
	TObjectPtr<UDialogue> Dialogue;

	/** Whether or not the event is still in progress/the dialogue should 
	* wait for it. */
	UPROPERTY()
	bool bBlocking = false;

public:
	FDialogueEventSignature OnStoppedBlocking;
};
