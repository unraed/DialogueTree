// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "DialogueQuery.h"
//Generated
#include "DialogueQueryFloat.generated.h"

/**
* Abstract base class for dialogue queries that return a double/floating point
* value. 
*/
UCLASS(Abstract, EditInlineNew)
class DIALOGUETREERUNTIME_API UDialogueQueryFloat : public UDialogueQuery
{
	GENERATED_BODY()

public:
	/**
	* Executes the floating point query.
	*
	* @return double - Value of the query.
	*/
	virtual double ExecuteQuery();
};