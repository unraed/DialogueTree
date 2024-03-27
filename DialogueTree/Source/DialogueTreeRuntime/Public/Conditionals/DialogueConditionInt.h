// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "DialogueCondition.h"
//Generated
#include "DialogueConditionInt.generated.h"

class UDialogueQueryInt;

/**
* Enum defining comparison values for int conditions. 
*/
UENUM(BlueprintType)
enum class EIntComparison : uint8
{
	GreaterThan,
	LessThan,
	EqualTo
};

/**
 * Dialogue condition that takes an int query. 
 */
UCLASS()
class DIALOGUETREERUNTIME_API UDialogueConditionInt : public UDialogueCondition
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
	UDialogueQueryInt* Query;

	/** The comparison to make based on the query */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	EIntComparison Comparison = EIntComparison::EqualTo;

	/** The value to compare the query to */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	int32 CompareValue = 0;
};
