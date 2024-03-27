// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Conditionals/Queries/Base/DialogueQueryFloat.h"
//Plugin
#include "LogDialogueTree.h"

double UDialogueQueryFloat::ExecuteQuery()
{
    UE_LOG(
        LogDialogueTree,
        Error,
        TEXT("Should not be using abstract float query directly")
    );
    return 0.0;
}
