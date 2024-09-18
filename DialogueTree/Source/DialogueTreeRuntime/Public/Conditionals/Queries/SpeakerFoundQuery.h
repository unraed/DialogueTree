// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "Conditionals/Queries/Base/DialogueQueryBool.h"
//Generated
#include "SpeakerFoundQuery.generated.h"

class UDialogueSpeakerSocket;

/**
 * Query that checks if an expected speaker component is present in
 * the dialogue. This is useful when a dialogue can have variable 
 * speakers as participants (in a party based CRPG for example). 
 */
UCLASS(EditInlineNew)
class DIALOGUETREERUNTIME_API USpeakerFoundQuery : public UDialogueQueryBool
{
	GENERATED_BODY()
	
public:
	/** IDialogueQueryBool Impl. */
	virtual bool ExecuteQuery() override;
	virtual FText GetGraphDescription_Implementation() const override;
	virtual bool IsValidQuery() const override;
	/** End IDialogueQueryBool */

	/** UObject Impl. */
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	/** End UObject */

private:
	/** The speaker socket associated with the target speaker */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TObjectPtr<UDialogueSpeakerSocket> Speaker;
};
