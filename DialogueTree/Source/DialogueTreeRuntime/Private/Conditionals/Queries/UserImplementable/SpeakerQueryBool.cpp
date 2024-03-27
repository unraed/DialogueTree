// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Conditionals/Queries/UserImplementable/SpeakerQueryBool.h"
//Plugin
#include "Dialogue.h"
#include "DialogueSpeakerComponent.h"
#include "DialogueSpeakerSocket.h"
#include "LogDialogueTree.h"

bool USpeakerQueryBool::ExecuteQuery()
{
    //Try to get the speaker component
    UDialogueSpeakerComponent* SpeakerComponent =
        Speaker->GetSpeakerComponent(GetDialogue());

    //If not found, end the dialogue
    if (!SpeakerComponent)
    {
        UE_LOG(
            LogDialogueTree,
            Warning,
            TEXT("Query failed to execute because the specified speaker component was not found. Verify that the dialogue name property matches the speaker's role in the dialogue.")
        );

        GetDialogue()->EndDialogue();
        return false;
    }

    //Convert main speaker to entry struct
    FSpeakerActorEntry TargetSpeaker = SpeakerComponent->ToSpeakerActorEntry();

    //Repeat with any additional speakers
    TArray<FSpeakerActorEntry> OtherSpeakers;
    for (UDialogueSpeakerSocket* Socket : AdditionalSpeakers)
    {
        UDialogueSpeakerComponent* SocketComponent =
            Socket->GetSpeakerComponent(GetDialogue());
        if (!SocketComponent)
        {
            UE_LOG(
                LogDialogueTree,
                Warning,
                TEXT("Query failed to execute because the specified speaker component was not found. Verify that the dialogue name property matches the speaker's role in the dialogue. Returning false.")
            );
            return false;
        }

        FSpeakerActorEntry SocketEntry =
            SocketComponent->ToSpeakerActorEntry();
        OtherSpeakers.Add(SocketEntry);
    }

    //Query the speaker
    return QuerySpeaker(TargetSpeaker, OtherSpeakers);
}

bool USpeakerQueryBool::IsValidQuery() const
{
    return Speaker && Speaker->IsValidSocket() && IsValidSpeakerQuery();
}

UDialogueSpeakerSocket* USpeakerQueryBool::GetSpeakerSocket() const
{
    return Speaker;
}

TArray<UDialogueSpeakerSocket*> USpeakerQueryBool::GetAdditionalSpeakerSockets() const
{
    return AdditionalSpeakers;
}

bool USpeakerQueryBool::IsValidSpeakerQuery_Implementation() const
{
    return true;
}
