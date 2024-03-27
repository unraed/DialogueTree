// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Conditionals/Queries/UserImplementable/SpeakerQueryFloat.h"
//Plugin
#include "Dialogue.h"
#include "DialogueSpeakerComponent.h"
#include "DialogueSpeakerSocket.h"
#include "LogDialogueTree.h"

double USpeakerQueryFloat::ExecuteQuery()
{
    //Try to get the speaker component
    UDialogueSpeakerComponent* SpeakerComponent =
        Speaker->GetSpeakerComponent(GetDialogue());
    if (!SpeakerComponent)
    {
        UE_LOG(
            LogDialogueTree,
            Warning,
            TEXT("Query failed to execute because the specified speaker component was not found. Verify that the dialogue name property matches the speaker's role in the dialogue. Returning false.")
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

bool USpeakerQueryFloat::IsValidQuery() const
{
    return Speaker && Speaker->IsValidSocket() && IsValidSpeakerQuery();
}

UDialogueSpeakerSocket* USpeakerQueryFloat::GetSpeakerSocket() const
{
    return Speaker;
}

TArray<UDialogueSpeakerSocket*> USpeakerQueryFloat::GetAdditionalSpeakerSockets() const
{
    return AdditionalSpeakers;
}

bool USpeakerQueryFloat::IsValidSpeakerQuery_Implementation() const
{
    return true;
}
