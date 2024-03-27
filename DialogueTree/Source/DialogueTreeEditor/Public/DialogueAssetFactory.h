// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "Factories/Factory.h"
//Generated
#include "DialogueAssetFactory.generated.h"

class UDialogue;

/**
 * Factory for creating new dialogues. 
 */
UCLASS()
class DIALOGUETREEEDITOR_API UDialogueAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	/** Constructor */
	UDialogueAssetFactory();

public:
	/** UFactory Impl. */
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, 
		FName Name, EObjectFlags Flags, UObject* Context, 
		FFeedbackContext* Warn) override;
	/** End UFactory */
};
