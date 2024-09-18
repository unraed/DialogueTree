// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "Sound/SoundBase.h"
#include "GameplayTagContainer.h"
//Generated
#include "SpeechDetails.generated.h"

/**
* Struct used to pass information about an individual dialogue 
* speech. 
*/
USTRUCT(BlueprintType)
struct FSpeechDetails
{
	GENERATED_BODY()

	/** The text of the speech */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FText SpeechText = FText();

	/** The name of the speaker */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FName SpeakerName = NAME_None;

	/** The audio associated with the speech */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<USoundBase> SpeechAudio = nullptr;

	/** The minimum time for the speech to play before transitioning (unless
	* skipped) */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	float MinimumPlayTime = 0.f;

	/** Any behavior flags associated with the speech as gameplay tags */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FGameplayTagContainer GameplayTags;

	/** Whether the speech content should be ignored when entering the node */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	bool bIgnoreContent = false;

	/** Whether the player is allowed to skip the speech */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	bool bCanSkip = true;

	/** Used when an option. Whether the option is locked or free to 
	* be selected */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	bool bIsLocked = false;

	/** Additional message associated with the speech when used as an option.
	* For example, can provide an optional reason for being locked. */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FText OptionMessage = FText();
};