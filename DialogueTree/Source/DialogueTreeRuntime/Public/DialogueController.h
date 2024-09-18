// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//Plugin
#include "Dialogue.h"
//Generated
#include "DialogueController.generated.h"

class UDialogue;
class UDialogueSpeakerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogueControllerDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FDialogueControllerSpeechDelegate, FSpeechDetails, SpeechDetails);

/**
* Struct used to extract node visited data for a single dialogue.
* Primarily useful for saving/loading.
*/

USTRUCT(BlueprintType)
struct FDialogueNodeVisits
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FName DialogueFName;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Dialogue")
	TSet<FName> VisitedNodeIDs;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Dialogue")
	FName ResumeNodeID = NAME_None;
};

/**
* Struct used to extract the node visit "memory" of one or more dialogues.
* Used for saving and loading.
*/
USTRUCT(BlueprintType)
struct FDialogueRecords
{
	GENERATED_BODY()

	/** Map of dialogue FNames to their records of visited nodes */
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Dialogue")
	TMap<FName, FDialogueNodeVisits> Records;
};

/**
* Actor that serves as a controller linking the various aspects of
* dialogue behavior: user input, dialogue widgets (or other display
* schemes), and the dialogue itself.
*/
UCLASS(Abstract, notplaceable, BlueprintType, Blueprintable)
class DIALOGUETREERUNTIME_API ADialogueController : public AActor
{
	GENERATED_BODY()

public:
	/** Constructor */
	ADialogueController();

public:
	/**
	* Notifies the dialogue that the user is attempting to select
	* the option at the given index. BlueprintCallable.
	*
	* @param InOptionIndex - int32, index of the selection.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	virtual void SelectOption(int32 InOptionIndex) const;

	/**
	* Retrieves the map of names to speaker components for the
	* current dialogue. Empty if the current dialogue is invalid.
	* BlueprintCallable.
	*
	* @return TMap<FName, UDialogueSpeakerComponent*>, the map of speakers for
	* the current dialogue.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	virtual TMap<FName, UDialogueSpeakerComponent*> GetSpeakers() const;

	/**
	* Starts the provided dialogue with the provided speaker
	* components. Matches speakers to dialogue roles using
	* the provided name-speaker pairings.
	*
	* @param InDialogue - UDialogue*, the dialogue to start.
	* @param InSpeakers - TMap<FName, UDialogueSpeakerComponent*>,
	* Speaker Components mapped to their names in dialogue.
	* @param bResume - bool - If true, the dialogue will resume from the marked
	* resume node (if any). If false, the dialogue will start over.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogueWithNames(UDialogue* InDialogue,
		TMap<FName, UDialogueSpeakerComponent*> InSpeakers,
		bool bResume = false);

	/**
	* Starts the provided dialogue with the provided speaker
	* components.Matches the speakers to dialogue roles using their dialogue
	* names. Duplicate or unfilled names not allowed.
	*
	* @param InDialogue - UDialogue*, the dialogue to start.
	* @param InSpeakers - TArray<UDialogueSpeakerComponent*>, Speaker
	* Components to use.
	* @param bResume - bool - If true, the dialogue will resume from the marked
	* resume node (if any). If false, the dialogue will start over.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(UDialogue* InDialogue,
		TArray<UDialogueSpeakerComponent*> InSpeakers, bool bResume = false);

	/**
	* Starts the provided dialogue with the provided speaker
	* components at the given dialogue node ID. Matches speakers to dialogue
	* roles using the provided name-speaker pairings.
	*
	* @param InDialogue - UDialogue*, the dialogue to start.
	* @param InSpeakers - TMap<FName, UDialogueSpeakerComponent*>,
	* Speaker Components mapped to their names in dialogue.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogueWithNamesAt(UDialogue* InDialogue, FName NodeID,
		TMap<FName, UDialogueSpeakerComponent*> InSpeakers);

	/**
	* Starts the provided dialogue with the provided speaker
	* components at the given dialogue node ID. Matches the speakers to
	* dialogue roles using their dialogue names. Duplicate or unfilled names
	* not allowed.
	*
	* @param InDialogue - UDialogue*, the dialogue to start.
	* @param InSpeakers - TArray<UDialogueSpeakerComponent*>, Speaker
	* Components to use.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogueAt(UDialogue* InDialogue, FName NodeID,
		TArray<UDialogueSpeakerComponent*> InSpeakers);

	/**
	* Ends the current dialogue. BlueprintCallable.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();

	/**
	* Tells the dialogue we want to skip the current speech, if
	* possible. BlueprintCallable.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void Skip() const;

	/**
	* Clears all previous visits from the current dialogue's record.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ClearNodeVisits();

	/**
	* Adds the designated speaker to the target dialogue, using the provided
	* name as a key. If no such name is present, or if the target dialogue
	* has not been set, does nothing.
	*
	* @param InName - FName, the target name.
	* @param InSpeaker - UDialogueSpeakerComponent*, the speaker component to
	* set.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetSpeaker(FName InName, UDialogueSpeakerComponent* InSpeaker);

	/**
	* Exports a dialogue records struct containing the node visits for
	* all dialogues in the game. Useful for saving.
	*
	* @return FDialogueMemory - record of all node visits in the game.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue")
	FDialogueRecords GetDialogueRecords() const;

	/**
	* Clears node visitation info from all dialogues.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ClearDialogueRecords();

	/**
	* Imports a dialogue records struct, adding any recorded visits to the
	* appropriate dialogues.
	*
	* @param InRecords - const FDialogueRecords&, the records to load.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ImportDialogueRecords(FDialogueRecords InRecords);

	/**
	* Checks if the specified speaker is a participant in the current dialogue.
	*
	* @param TargetSpeaker - UDialogueSpeakerComponent*
	* @return True if the target speaker is a part of the active dialogue.
	* False otherwise.
	*/
	bool SpeakerInCurrentDialogue(UDialogueSpeakerComponent* TargetSpeaker)
		const;

	/**
	* Marks the given node visited in the controller's memory.
	*
	* @param TargetDialogue, UDialogue*
	* @param TargetNodeID, FName
	*/
	void MarkNodeVisited(UDialogue* TargetDialogue, FName TargetNodeID);

	/**
	* Marks the given node unvisited in the controller's memory.
	*
	* @param TargetDialogue, UDialogue*
	* @param TargetNodeID, FName
	*/
	void MarkNodeUnvisited(UDialogue* TargetDialogue, FName TargetNodeID);

	/**
	* Clears all node visits for the given dialogue.
	*
	* @param TargetDialogue, UDialogue*
	*/
	void ClearAllNodeVisitsForDialogue(UDialogue* TargetDialogue);

	/**
	* Checks if the given node has already been visited.
	*
	* @param TargetDialogue, UDialogue*
	* @param TargetNodeID, FName
	* @return bool - True if the node was visited, False otherwise.
	*/
	bool WasNodeVisited(const UDialogue* TargetDialogue,
		FName TargetNodeID) const;

	/**
	* Sets the resume node for the target dialogue to the target node. Called
	* from the dialogue.
	*
	* @param InDialogue - UDialogue*, the target dialogue.
	* @param InNodeID - FName, the target node ID to resume from.
	*/
	void SetResumeNode(UDialogue* InDialogue, FName InNodeID);

public:
	/**
	* Opens the user-defined dialogue display.
	* BlueprintImplementable.
	*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable,
		Category = "Dialogue")
	void OpenDisplay();

	/**
	* Closes the user-defined dialogue display.
	* BlueprintImplementable.
	*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable,
		Category = "Dialogue")
	void CloseDisplay();

	/**
	* Displays the specified speech in dialogue.
	* BlueprintImplementable.
	*
	* @param InSpeechDetails - FSpeechDetails, struct defining
	* speech details
	* @param InSpeaker - UDialogueSpeakerComponent*, speaker
	* component associated with the target speech.
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void DisplaySpeech(FSpeechDetails InSpeechDetails,
		UDialogueSpeakerComponent* InSpeaker);

	/**
	* Displays a set of options for the user to select from
	* in dialogue. BlueprintImplementable.
	*
	* @param InOptions - const TArray<FSpeechDetails>&, the speech
	* details to display as options.
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayOptions(const TArray<FSpeechDetails>& InOptions);

	/**
	* Checks if we can open the user-defined dialogue display.
	* BlueprintImplementable.
	*/
	UFUNCTION(BlueprintImplementableEvent)
	bool CanOpenDisplay() const;

	/**
	* User-defined behavior for when a listed speaker component is
	* not provided at dialogue start.
	*
	* @param MissingName, const FName&, the name of the missing
	* speaker component.
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void HandleMissingSpeaker(const FName& MissingName);

protected:
	/** The dialogue currently being played. */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<UDialogue> CurrentDialogue = nullptr;

private:
	/** Controller's memory of visited nodes */
	FDialogueRecords DialogueRecords;

public:
	/** Delegate event call for when a new dialogue is started.*/
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FDialogueControllerDelegate OnDialogueStarted;

	/** Delegate event call for when a dialogue ends.*/
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FDialogueControllerDelegate OnDialogueEnded;

	/** Delegate event call for when a speech plays.*/
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FDialogueControllerSpeechDelegate OnDialogueSpeechDisplayed;
};
