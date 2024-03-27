// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueEventBase.generated.h"

class UDialogue;

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
	class UDialogue* Dialogue;
};
