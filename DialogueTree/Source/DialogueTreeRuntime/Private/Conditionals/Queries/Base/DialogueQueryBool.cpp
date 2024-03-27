// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Conditionals/Queries/Base/DialogueQueryBool.h"
//Plugin
#include "LogDialogueTree.h"

bool UDialogueQueryBool::ExecuteQuery()
{
    UE_LOG(
        LogDialogueTree,
        Error,
        TEXT("Should not use abstract bool query directly")
    );
    return false;
}
