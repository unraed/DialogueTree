// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "Components/AudioComponent.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
//Plugin
#include "SpeechDetails.h"
//Generated
#include "DialogueSpeakerComponent.generated.h"

class ADialogueController;

/**
* Delegate used to pass data about gameplay tag changes. 
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnDialogueGameplayTagsChanged,
	FGameplayTagContainer,
	InTags
);

/** Delegate used to pass data about speeches that are played */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FSpeakerSpeechSignature,
	FSpeechDetails,
	InSpeechDetails
);

/**
* Helper struct used to condense a speaker component and the actor which owns
* it into a single parameter value for easier access. 
*/
USTRUCT(BlueprintType)
struct FSpeakerActorEntry
{
	GENERATED_BODY()

	/** The actor which owns the speaker component */
	UPROPERTY(BlueprintReadOnly, Category="Dialogue")
	TObjectPtr<AActor> Actor = nullptr;

	/** The speaker component under consideration */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<UDialogueSpeakerComponent> SpeakerComponent = nullptr;
};

/**
* A component representing a "speaker" or participant in a dialogue.
* Serves as a liason between the dialogue and the game world.
*/
UCLASS( Abstract, Blueprintable, ClassGroup=(Custom), 
	meta=(BlueprintSpawnableComponent) )
class DIALOGUETREERUNTIME_API UDialogueSpeakerComponent : 
	public UAudioComponent
{
	GENERATED_BODY()

public:
	/** Constructor */
	UDialogueSpeakerComponent();

public:
	/** UAudioComponent Impl. */
	virtual void BeginPlay() override;
	/** End UAudioComponent */

	/**
	* Sets the speaker's display name to the provided text.
	* 
	* @param InDisplayName - FText, the new display name. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetDisplayName(FText InDisplayName);

	/**
	* Sets the speaker's dialogue name (the name to use for matching it 
	* automatically into a dialogue role). 
	* 
	* @param InDialogueName - FName, the new dialogue name. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetDialogueName(FName InDialogueName);

	/** 
	* Retrieves the display name for the speaker.
	*
	* @return FText, the display name. 
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue")
	FText GetDisplayName() const;

	/**
	* Retrieves the dialogue name for the speaker. This is used for matching
	* the speaker component to its role in a dialogue.
	* 
	* @return FName - the speaker's dialogue name. 
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue")
	FName GetDialogueName() const;

	/**
	* Sets the speaker's owned dialogue to the provided dialogue asset. 
	* 
	* @param InDialogue - UDialogue*, the target dialogue. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetOwnedDialogue(UDialogue* InDialogue);

	/**
	* Retrieves the speaker's owned dialogue. 
	* 
	* @return UDialogue*, the owned dialogue. 
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dialogue")
	UDialogue* GetOwnedDialogue();

	/**
	* Gets gameplay tag container marking tags for any ongoing 
	* speech. 
	* 
	* @return FGameplayTagContainer
	*/
	UFUNCTION(BlueprintPure, Category="Dialogue")
	FGameplayTagContainer GetCurrentGameplayTags();

	/**
	* Ends the dialogue the speaker is currently participating in, if 
	* applicable. Does nothing if the speaker is not engaged in dialogue.
	*/
	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void EndCurrentDialogue();

	/**
	* Attempts to skip the current speech that is playing. Does nothing
	* if the speaker component is not engaged in dialogue. 
	*/
	UFUNCTION(BlueprintCallable, Category="Dialogue")
	void TrySkipSpeech();

	/**
	* Plays the given audio clip. Exposed to blueprint to be
	* user-overridable. 
	* 
	* @param InAudio - USoundBase*, the audio clip. 
	*/
	UFUNCTION(BlueprintNativeEvent, Category="Dialogue")
	void PlaySpeechAudioClip(USoundBase* InAudio);
	virtual void PlaySpeechAudioClip_Implementation(
		USoundBase* InAudio);

	/**
	* Changes out the speaker's current gameplay tags to the 
	* provided set. Primarily meant to be called from dialogue side.
	* 
	* @param InTags - FGameplayTagContainer, the new tags to set. 
	*/
	void SetCurrentGameplayTags(FGameplayTagContainer InTags);

	/**
	* Clears the gameplay tags. 
	*/
	void ClearGameplayTags();

	/**
	* Starts the default dialogue for this speaker component. Uses the provided
	* name-speaker pairings for matching with target dialogue. 
	* 
	* @param InSpeakers - TMap<FName, UDialogueSpeakerComponent*>,
	* the speaker components to pass to the dialogue. 
	* @param bResume - bool - If true, the dialogue will resume from the marked
	* resume node (if any). If false, the dialogue will start over.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartOwnedDialogueWithNames(
		TMap<FName, UDialogueSpeakerComponent*> InSpeakers, 
		bool bResume = false);

	/**
	* Starts the default dialogue for this speaker component. Attempts to 
	* match the provided speaker's dialogue names to those expected
	* by the dialogue. Duplicate or unfilled names not allowed. 
	* 
	* @param InSpeakers - TArray<UDialogueSpeakerComponent*>, the conversing 
	* speakers. 
	* @param bResume - bool - If true, the dialogue will resume from the marked
	* resume node (if any). If false, the dialogue will start over.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartOwnedDialogue(TArray<UDialogueSpeakerComponent*> InSpeakers, 
		bool bResume = false);

	/**
	* Starts the given dialogue. Uses the provided name-speaker pairings for
	* matching with target dialogue.
	* 
	* @param InDialogue - UDialogue*, the dialogue to start. 
	* @param InSpeakers - TMap<FName, UDialogueSpeakerComponent*>,
	* the speaker components to pass to the dialogue. 
	* @param bResume - bool - If true, the dialogue will resume from the marked
	* resume node (if any). If false, the dialogue will start over.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogueWithNames(UDialogue* InDialogue, 
		TMap<FName, UDialogueSpeakerComponent*> InSpeakers,
		bool bResume = false);

	/**
	* Starts the given dialogue. Uses the provided speakers' dialogue names 
	* for matching with target dialogue. Duplicate or unfilled dialogue names
	* not allowed.
	* 
	* @param InDialogue - UDialogue*, the dialogue to start. 
	* @param InSpeakers - TArray<UDialogueSpeakerComponent*> the conversing
	* speakers. 
	* @param bResume - bool - If true, the dialogue will resume from the marked
	* resume node (if any). If false, the dialogue will start over.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(UDialogue* InDialogue, 
		TArray<UDialogueSpeakerComponent*> InSpeakers, bool bResume = false);

	/**
	* Starts the default dialogue for this speaker component at the given 
	* NodeID location. Uses the provided name-speaker pairings for matching 
	* with target dialogue.
	*
	* @param InNodeID - FName, the target node ID to start at. 
	* @param InSpeakers - TMap<FName, UDialogueSpeakerComponent*>,
	* the speaker components to pass to the dialogue.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartOwnedDialogueWithNamesAt(FName InNodeID, 
		TMap<FName, UDialogueSpeakerComponent*> InSpeakers);

	/**
	* Starts the default dialogue for this speaker component at the given node 
	* ID. Attempts to match the provided speaker's dialogue names to those 
	* expected by the dialogue. Duplicate or unfilled names not allowed.
	*
	* @param InNodeID - FName, the target node to start at. 
	* @param InSpeakers - TArray<UDialogueSpeakerComponent*>, the conversing
	* speakers.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartOwnedDialogueAt(FName InNodeID, 
		TArray<UDialogueSpeakerComponent*> InSpeakers);

	/**
	* Starts the given dialogue at the given node ID. Uses the provided 
	* name-speaker pairings for matching with target dialogue.
	*
	* @param InDialogue - UDialogue*, the dialogue to start.
	* @param InNodeID - FName, the target node to start at. 
	* @param InSpeakers - TMap<FName, UDialogueSpeakerComponent*>,
	* the speaker components to pass to the dialogue.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogueWithNamesAt(UDialogue* InDialogue, FName InNodeID,
		TMap<FName, UDialogueSpeakerComponent*> InSpeakers);

	/**
	* Starts the given dialogue at the given node ID. Uses the provided 
	* speakers' dialogue names for matching with target dialogue. Duplicate 
	* or unfilled dialogue names not allowed.
	*
	* @param InNodeID - FName, the target node to start at. 
	* @param InDialogue - UDialogue*, the dialogue to start.
	* @param InSpeakers - TArray<UDialogueSpeakerComponent*> the conversing
	* speakers.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogueAt(UDialogue* InDialogue, FName InNodeID,
		TArray<UDialogueSpeakerComponent*> InSpeakers);

public:
	FSpeakerActorEntry ToSpeakerActorEntry();

	/**
	* Notifies subscribers that the given speech was skipped.
	* 
	* @param SkippedSpeech - FSpeechDetails, the skipped speech. 
	*/
	void BroadcastSpeechSkipped(FSpeechDetails SkippedSpeech);

private:
	void BroadcastCurrentGameplayTags();

protected:
	/** The name to display for this speaker in dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FText DisplayName;

	/** The "key" name used to match the speaker to its dialogue role */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FName DialogueName;

	/** The default dialogue to start when initiating dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TObjectPtr<UDialogue> OwnedDialogue;

	/** Tags associated with a speech in dialogue. Used for animation, etc. 
	* Set up as maps for ease of access and greater flexibility. */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue", 
		meta = (AllowPrivateAccess = true))
	FGameplayTagContainer GameplayTags;

public:
	/** Currently active dialogue controller */
	UPROPERTY(BlueprintReadOnly, Category="Dialogue")
	TObjectPtr<ADialogueController> DialogueController;

	/** Delegate used to let others know when gameplay tags change */
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnDialogueGameplayTagsChanged OnGameplayTagsChanged;

	/** 
	* Delegate used to let others know when this speaker's speech
	* was skipped. 
	*/
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FSpeakerSpeechSignature OnSpeechSkipped;
};
