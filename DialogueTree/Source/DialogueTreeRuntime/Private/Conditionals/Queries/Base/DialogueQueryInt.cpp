// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Conditionals/Queries/Base/DialogueQueryInt.h"
//Plugin
#include "LogDialogueTree.h"

int32 UDialogueQueryInt::ExecuteQuery()
{
    UE_LOG(
        LogDialogueTree,
        Error,
        TEXT("Should not be using abstract int dialogue query directly")
    );
    return 0;
}
