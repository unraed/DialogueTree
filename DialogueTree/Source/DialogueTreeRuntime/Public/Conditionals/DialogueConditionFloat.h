// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "DialogueCondition.h"
//Generated
#include "DialogueConditionFloat.generated.h"

class UDialogueQueryFloat;

/**
 * Enum for float dialogue comparisons. 
 */
UENUM(BlueprintType)
enum class EFloatComparison : uint8
{
	GreaterThan,
	LessThan
};

/**
 * Dialogue condition that takes a floating point value. 
 */
UCLASS()
class DIALOGUETREERUNTIME_API UDialogueConditionFloat : 
	public UDialogueCondition
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
	/** The query for the condition */
	UPROPERTY()
	TObjectPtr<UDialogueQueryFloat> Query;

	/** The comparison to make against the query */
	UPROPERTY(EditAnywhere, Category="Dialogue")
	EFloatComparison Comparison = EFloatComparison::GreaterThan;

	/** The value to compare the query to */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	double CompareValue = 0.0;
};
