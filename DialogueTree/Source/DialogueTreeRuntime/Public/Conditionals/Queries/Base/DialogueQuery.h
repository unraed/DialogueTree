// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//Generated
#include "DialogueQuery.generated.h"

class UDialogue;

/**
* Abstract base class for all dialogue queries. 
*/
UCLASS(Abstract, EditInlineNew)
class DIALOGUETREERUNTIME_API UDialogueQuery : public UObject
{
	GENERATED_BODY()

public:
	/**
	* Sets the dialogue object for the query. Provided dialogue object must 
	* not be nullptr. 
	* 
	* @param InDialogue - Dialogue to set
	*/
	void SetDialogue(UDialogue* InDialogue);

	/**
	* Retrieves the dialogue associated with this query. Ensures that the 
	* dialogue is valid. 
	* 
	* @return UDialogue*, the dialogue. 
	*/
	UDialogue* GetDialogue() const;

	/**
	* Retrieves the text to display for the query in the graph editor. Should
	* be implemented by the user as a blueprint native event. 
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Dialogue")
	FText GetGraphDescription() const;
	virtual FText GetGraphDescription_Implementation() const;

	/**
	* Checks if the query has all the information it needs to be valid for 
	* compilation. 
	* 
	* @return bool - True if valid, False otherwise. 
	*/
	virtual bool IsValidQuery() const;

private:
	UPROPERTY()
	TObjectPtr<UDialogue> Dialogue;
};
