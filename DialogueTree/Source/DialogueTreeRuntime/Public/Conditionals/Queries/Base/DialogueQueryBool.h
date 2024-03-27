// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "DialogueQuery.h"
//Generated
#include "DialogueQueryBool.generated.h"

/**
* Abstract base class for dialogue queries that return a bool. 
*/
UCLASS(Abstract, EditInlineNew)
class DIALOGUETREERUNTIME_API UDialogueQueryBool : public UDialogueQuery
{
	GENERATED_BODY()

public:
	/**
	* Executes the bool query.
	*
	* @return bool - Value of the query.
	*/
	virtual bool ExecuteQuery();
};
