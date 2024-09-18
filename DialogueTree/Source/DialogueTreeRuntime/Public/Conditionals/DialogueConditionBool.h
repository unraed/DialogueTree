// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "DialogueCondition.h"
//Generated
#include "DialogueConditionBool.generated.h"

class UDialogueQueryBool;

/**
 * Dialogue condition that takes a bool query. 
 */
UCLASS()
class DIALOGUETREERUNTIME_API UDialogueConditionBool : public UDialogueCondition
{
	GENERATED_BODY()

public:
	/** UDialogueCondition Impl. */
	virtual bool IsMet() const override;
	virtual void SetQuery(UDialogueQuery* InQuery) override;
	virtual void SetDialogue(UDialogue* InDialogue) override;
	virtual FText GetDisplayText(const TMap<FName, FText>& ArgTexts,
		const FText QueryText) const override;
	virtual FText GetGraphDescription(FText QueryText) override;
	virtual bool IsValidCondition() override;
	/** End UDialogueCondition */

private: 
	/** The query for the condition  */
	UPROPERTY()
	TObjectPtr<UDialogueQueryBool> Query;

	/** Whether the query should be true or false */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	bool QueryTrue = true; 
};
