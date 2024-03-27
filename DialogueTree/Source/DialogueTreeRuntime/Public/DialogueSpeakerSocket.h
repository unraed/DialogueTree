// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//Generated
#include "DialogueSpeakerSocket.generated.h"

/**
* Object used to represent the notion of a "Speaker" when the actual
* SpeakerComponent may not be immediately available. 
*/
UCLASS()
class DIALOGUETREERUNTIME_API UDialogueSpeakerSocket : public UObject
{
	GENERATED_BODY()

public:
	/**
	* Sets the speaker's name to the provided value.
	* 
	* @param InName - FName, the new name 
	*/
	void SetSpeakerName(FName InName);

	/**
	* Retrieves the speaker's name. 
	* 
	* @return FName, the speaker's name
	*/
	UFUNCTION(BlueprintCallable, Category="Dialogue")
	FName GetSpeakerName() const;

	/**
	* Retrieve the component associated with this speaker from 
	* the provided dialogue. 
	* 
	* @param InDialogue*, UDialogue* to get the speaker component from.
	* @return UDialogueSpeakerComponent*, the component for the speaker in the 
	* given dialogue or nullptr if none found.
	*/
	class UDialogueSpeakerComponent* GetSpeakerComponent(
		class UDialogue* InDialogue) const;

	/**
	* Checks to see if the socket's value is valid. 
	* 
	* @return true if the socket's value is valid, false otherwise
	*/
	bool IsValidSocket() const;

private:
	/** Name of the speaker */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	FName SpeakerName;
};
