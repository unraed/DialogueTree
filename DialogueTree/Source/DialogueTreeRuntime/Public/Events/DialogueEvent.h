// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "Events/DialogueEventBase.h"
//Plugin
#include "DialogueSpeakerComponent.h"
#include "SpeechDetails.h"
//Generated
#include "DialogueEvent.generated.h"

class UDialogue;
class UDialogueSpeakerSocket;

/**
 * An event playable from dialogue. Designed to be implemented in 
 * blueprint. 
 */
UCLASS(Abstract, Blueprintable)
class DIALOGUETREERUNTIME_API UDialogueEvent : public UDialogueEventBase
{
	GENERATED_BODY()

public:
	/** UDialogueEventBase Impl. */
	virtual void PlayEvent() override;
	virtual bool HasAllRequirements() const override;
	virtual FText GetGraphDescription_Implementation() const override;
	/** End UDialogueEventBase */

	/**
	* Checks if the event is valid and has all of the information it needs
	* to compile.
	*
	* @return bool - True if all requisite data is present; false otherwise.
	*/
	UFUNCTION(BlueprintNativeEvent)
	bool IsValidEvent() const;
	virtual bool IsValidEvent_Implementation() const;

	/**
	* Sets the event's speaker. 
	* 
	* @param InSpeaker - UDialogueSpeakerSocket*, the speaker. 
	*/
	void SetSpeaker(UDialogueSpeakerSocket* InSpeaker);

	/**
	* Retrieves the target speaker for the event.
	* 
	* @return UDialogueSpeakerSocket* - the target speaker. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	UDialogueSpeakerSocket* GetSpeakerSocket() const;

	/**
	* Retrieves the list of optional additional speaker sockets for the event.
	* 
	* @return TArray<UDialogueSpeakerSocket*> - the list of optional additional
	* speakers.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	TArray<UDialogueSpeakerSocket*> GetAdditionalSpeakerSockets() const;

	/**
	* Retrieves the speech details for the speech this event was 
	* called from. If not called from a speech, retrieves an empty 
	* speech details struct.
	* 
	* @param InDetails - FSpeechDetails, the new speech details. 
	*/
	UFUNCTION(BlueprintCallable, Category="Dialogue")
	const FSpeechDetails GetCurrentSpeechDetails() const;

protected:
	/**
	* User specified behavior for the event.
	* 
	* @param InSpeaker - FSpeakerActor, struct containing target speaker 
	* component and actor
	* @param OtherSpeakers - TArray<FSpeakerActor>, any additional speakers
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Dialogue")
	void OnPlayEvent(FSpeakerActorEntry InSpeaker, const TArray< FSpeakerActorEntry>& OtherSpeakers);
	virtual void OnPlayEvent_Implementation(FSpeakerActorEntry InSpeaker, const TArray<FSpeakerActorEntry>& OtherSpeakers) {};

	/**
	* Helper function to spawn an actor to the currently active
	* world via a dialogue event.
	*
	* @param ActorClass - TSubclassOf<AActor>, the actor type to
	* spawn.
	* @param Location - const FVector, the location at which to
	* spawn the actor.
	* @param Rotation - const FRotator, the rotation at which to
	* spawn the actor.
	* @param SpawnParams - const FActorSpawnParameters, any
	* additional spawn parameters.
	* @param Owner - AActor*, actor to set as the spawned actor's 
	* owner.
	* @param CollisionHandlingMethod - 
	* ESpawnActorCollisionHandlingMethod, how to handle collisions.
	* @return AActor* - the spawned actor, or nullptr if failed to
	* spawn.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueEvent")
	AActor* SpawnActorToCurrentWorld(TSubclassOf<AActor> ActorClass,
		const FVector Location, const FRotator Rotation, 
		AActor* Owner = nullptr, 
		ESpawnActorCollisionHandlingMethod CollisionHandlingMethod =
		ESpawnActorCollisionHandlingMethod::Undefined);

protected:
	/** Speaker socket for the speaker the event operates on */
	UPROPERTY(EditAnywhere, Category = "DialogueEvent")
	TObjectPtr<UDialogueSpeakerSocket> Speaker;

	/** Optional additional speakers to attach to the event */
	UPROPERTY(EditAnywhere, Category = "DialogueEvent")
	TArray<TObjectPtr<UDialogueSpeakerSocket>> AdditionalSpeakers;
};
