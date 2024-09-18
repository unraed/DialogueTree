// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "SpeechDetails.h"
//Generated
#include "DialogueOption.generated.h"

class UDialogueNode;

/**
* Struct used for representing a selectable dialogue option. 
*/
USTRUCT()
struct FDialogueOption
{
	GENERATED_BODY()

	/** The speech details associated with the option */
	UPROPERTY()
	FSpeechDetails Details = FSpeechDetails();

	/** The node the option transitions to */
	UPROPERTY()
	TObjectPtr<UDialogueNode> TargetNode = nullptr;
};