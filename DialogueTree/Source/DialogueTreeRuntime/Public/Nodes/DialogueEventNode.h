// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "Nodes/DialogueNode.h"
//Generated
#include "DialogueEventNode.generated.h"

class UDialogueEventBase;

/**
 * Dialogue node that plays a user-specified sequence of 
 * dialogue events. 
 */
UCLASS()
class DIALOGUETREERUNTIME_API UDialogueEventNode : public UDialogueNode
{
	GENERATED_BODY()

public:
	/** UDialogueNode Implementation */
	virtual void EnterNode() override;
	virtual FDialogueOption GetAsOption() override;
	virtual void Skip() override;
	/** End UDialogueNode */

	/**
	* Sets the node's events when building the dialogue asset. 
	* 
	* @param InEvents - TArray<UDialogueEvent*>&, the events for the node. 
	*/
	void SetEvents(TArray<UDialogueEventBase*>& InEvents);

	/**
	* Checks if there an ongoing event is blocking. 
	* 
	* @return bool - True if an event is blocking; False otherwise.
	*/
	bool GetIsBlocking() const;

protected: 
	/**
	* Plays the node's events. 
	*/
	void PlayEvents();

	/**
	* Transitions out of the node if all of its events have completed.
	*/
	UFUNCTION()
	virtual void TransitionIfNotBlocking() const;

private:
	/** Events to play */
	UPROPERTY()
	TArray<TObjectPtr<UDialogueEventBase>> Events;
};
