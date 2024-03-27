// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//Generated
#include "DialogueCondition.generated.h"

class UDialogue;
class UDialogueQuery;

/**
* Abstract base class of dialogue conditions. 
*/
UCLASS(Abstract)
class DIALOGUETREERUNTIME_API UDialogueCondition : public UObject
{
	GENERATED_BODY()

public:
	/**
	* Sets query for condition
	* 
	* @param InQuery - UDialogueQuery*, the query
	*/
	virtual void SetQuery(UDialogueQuery* InQuery);

	/**
	* Sets the dialogue for the condition and its query.
	*
	* @param InDialogue - UDialogue*, the owning dialogue.
	*/
	virtual void SetDialogue(UDialogue* InDialogue);

	/**
	* Determines if the condition is met or not
	* 
	* @return true if the condition is met, false otherwise
	*/
	virtual bool IsMet() const;

	/**
	* Assembles the display text for the condition
	* @param ArgTexts - TMap pairing FName of the condition's
	* public UProperty with the FText to display for it
	* @param QueryText - FText to display for the target Query
	* @return FText to display
	*/
	virtual FText GetDisplayText(const TMap<FName, FText>& ArgTexts,
		const FText QueryText) const; 

	/**
	* Retrieves the text to display for the condition in the graph editor.
	* 
	* @param QueryText - FText, the text to display for the query that 
	* belongs to this condition. 
	*/
	virtual FText GetGraphDescription(FText QueryText);

	/**
	* Checks if the condition is valid to be used. 
	*
	* @return bool - True if the condition is valid to be used. False 
	* otherwise.
	*/
	virtual bool IsValidCondition();
};
