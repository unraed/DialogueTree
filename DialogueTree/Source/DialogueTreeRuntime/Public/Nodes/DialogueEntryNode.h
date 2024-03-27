// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "DialogueNode.h"
//Generated
#include "DialogueEntryNode.generated.h"

/**
 * Dialogue node that serves as an entry point into the dialogue. 
 */
UCLASS()
class DIALOGUETREERUNTIME_API UDialogueEntryNode : public UDialogueNode
{
	GENERATED_BODY()

public:
	/** UDialogueNode Impl. */
	virtual void EnterNode() override;
	/** End UDialogueNode */
};
