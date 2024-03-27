// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "DialogueNode.h"
#include "SpeechDetails.h"
//Generated
#include "DialogueSpeechNode.generated.h"

class UDialogueSpeakerComponent;
class UDialogueTransition;

/**
 * Dialogue node that plays a speech. 
 */
UCLASS()
class DIALOGUETREERUNTIME_API UDialogueSpeechNode : public UDialogueNode
{
	GENERATED_BODY()

public:
	/**
	* Populates speech node data. Used when the node is created during 
	* compiling of the dialogue. 
	* 
	* @param Details - FSpeechDetails&, the data for the speech node. 
	* @param TransitionType - TSubclassOf<UDialogueTransition>, the type of 
	* transition to use. 
	*/
	void InitSpeechData(FSpeechDetails& InDetails, 
		TSubclassOf<UDialogueTransition> TransitionType);

	/**
	* Checks if the node is skippable by the player. 
	* 
	* @return bool, true if the speech can be skipped, else false. 
	*/
	bool GetCanSkip() const;

	/**
	* Retrieves the details struct for the speech.
	* 
	* @return FSpeechDetails, details for the speech. 
	*/
	FSpeechDetails GetDetails() const;

	/**
	* Retrieves the speaker component associated with the speech 
	* in the dialogue. Nullptr if none found. 
	* 
	* @return UDialogueSpeakerComponent*, associated speaker 
	* component. 
	*/
	UDialogueSpeakerComponent* GetSpeaker() const;

	/** DialogueNode Impl. */
	virtual void EnterNode() override;
	virtual FDialogueOption GetAsOption() override;
	virtual void SelectOption(int32 InOptionIndex) override;
	virtual void Skip() override;
	/** End DialogueNode */

private:
	/**
	* Tells the active speaker component to start speaking and 
	* sets any behavior flags associated with this speech. 
	*/
	void StartAudio();

private:
	/** The primary content of the speech */
	UPROPERTY()
	FSpeechDetails Details;

	/** The transition that governs how we leave the speech */
	UPROPERTY()
	UDialogueTransition* Transition = nullptr;
};
