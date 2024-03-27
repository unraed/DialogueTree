// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "DialogueQuery.h"
//Generated
#include "DialogueQueryInt.generated.h"

/**
* Abstract base class for dialogue queries that return an integer value. 
*/
UCLASS(Abstract, EditInlineNew)
class DIALOGUETREERUNTIME_API UDialogueQueryInt : public UDialogueQuery
{
	GENERATED_BODY()

public:
	/**
	* Executes the integer query.
	*
	* @return int32 - Value of the query.
	*/
	virtual int32 ExecuteQuery();
};
