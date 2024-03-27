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
	/** End UDialogueNode */

	/**
	* Sets the node's events when building the dialogue asset. 
	* 
	* @param InEvents - TArray<UDialogueEvent*>&, the events for the node. 
	*/
	void SetEvents(TArray<UDialogueEventBase*>& InEvents);

private: 
	/**
	* Plays the node's events. 
	*/
	void PlayEvents();

private:
	/** Events to play */
	UPROPERTY()
	TArray<UDialogueEventBase*> Events;
};
