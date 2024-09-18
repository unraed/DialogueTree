// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once 

//UE
#include "CoreMinimal.h"
//Generated
#include "PickableDialogueSpeaker.generated.h"

class UDialogueSpeakerSocket;

/**
* Struct serving as a data container for speaker sockets that 
* can be selected from the dialogue graph. 
*/
USTRUCT()
struct FPickableDialogueSpeaker
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TObjectPtr<UDialogueSpeakerSocket> Speaker;
};