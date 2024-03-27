// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once 

//UE
#include "CoreMinimal.h"
//Generated
#include "PickableDialogueNode.generated.h"

/**
* Struct serving as a data container for dialogue graph nodes which 
* we want to select from within the dialogue graph. 
*/
USTRUCT()
struct FPickableDialogueNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	class UGraphNodeDialogue* Node;
};