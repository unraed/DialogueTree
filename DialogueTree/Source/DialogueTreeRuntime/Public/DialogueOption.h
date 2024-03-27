// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "SpeechDetails.h"
//Generated
#include "DialogueOption.generated.h"

/**
* Struct used for representing a selectable dialogue option. 
*/
USTRUCT()
struct FDialogueOption
{
	GENERATED_BODY()

	FSpeechDetails Details = FSpeechDetails();
	class UDialogueNode* TargetNode = nullptr;
};