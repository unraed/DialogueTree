// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueGraphCondition.generated.h"

class UDialogue;
class UDialogueCondition;
class UDialogueQuery;
class UNodeVisitedQuery;

/**
 * A graph condition serving as a data container for the condition data to 
 * be compiled.
 */
UCLASS(EditInlineNew)
class DIALOGUETREEEDITOR_API UDialogueGraphCondition : public UObject
{
	GENERATED_BODY()

public:
	/** UObject Impl. */
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	/** End UObject */
	
	/**
	* Refreshes the actual condition associated with the graph condition 
	* based on the graph condition's current query type. 
	*/
	void RefreshCondition();

	/**
	* Finalizes the graph condition, supplying any last minute info. Currently
	* primarily used to deal with any node sockets. 
	* 
	* @param InOuter - UDialogue*, the outer dialogue.
	*/
	void FinalizeCondition(UDialogue* InOuter);

	/**
	* Retrieves the current query associated with the graph condition. 
	* 
	* @return UDialogueQuery*, the current query. 
	*/
	UDialogueQuery* GetQuery();

	/**
	* Retrieves the current condition associated with the graph condition. 
	* 
	* @return UDialogueCondition* - the current condition. 
	*/
	UDialogueCondition* GetCondition();

	/**
	* Checks if the condition needs to be refreshed. 
	* 
	* @return bool - True if the condition needs to be refreshed; else false.
	*/
	bool ShouldRefreshCondition();

	/** UObject Impl. */
	virtual void PostEditChangeProperty(
		struct FPropertyChangedEvent& PropertyChangedEvent);
	/** End UObject */

private:
	/**
	* Handles a node visited query when finalizing. Adds the appropriate asset
	* node to the correct socket. 
	* 
	* @param InQuery - UNodeVisitedQuery*, the target query. 
	*/
	void HandleNodeVisitedQuery(UNodeVisitedQuery* InQuery);

public:
	/** The query for the condition */
	UPROPERTY(EditAnywhere, Instanced, Category="Dialogue")
	TObjectPtr<UDialogueQuery> Query;

	/** The condition itself */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TObjectPtr<UDialogueCondition> Condition;
};
