// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "Base/DialogueQueryBool.h"
//Generated
#include "NodeVisitedQuery.generated.h"

class UDialogueNodeSocket;
class UDialogue;

/**
 * Dialogue query that checks if a specified node has already been
 * visited. 
 */
UCLASS(EditInlineNew)
class DIALOGUETREERUNTIME_API UNodeVisitedQuery : public UDialogueQueryBool
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

	/**
	* Gets the node socket for this query. 
	* 
	* @return UDialogueNodeSocket*, the socket. 
	*/
	UDialogueNodeSocket* GetSocket();

private: 
	/** Node to check */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TObjectPtr<UDialogueNodeSocket> TargetNode;
};
