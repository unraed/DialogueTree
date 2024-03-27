// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once 

//UE
#include "CoreMinimal.h"

/**
* Enum defining how many connections may be made by a dialogue node along a 
* given axis. 
*/
enum class DIALOGUETREERUNTIME_API EDialogueConnectionLimit
{
	Unlimited,
	Single,
	None
};
