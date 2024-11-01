// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Transitions/DialogueTransition.h"
//UE
#include "Engine/World.h"
//Plugin
#include "Dialogue.h"
#include "DialogueConnectionLimit.h"
#include "DialogueSpeakerComponent.h"
#include "Nodes/DialogueNode.h"
#include "Nodes/DialogueSpeechNode.h"
#include "LogDialogueTree.h"

UDialogueTransition::UDialogueTransition()
{
	OnContentEnd.BindUFunction(this, "OnDonePlayingContent");
	OnTimerEnd.BindUFunction(this, "OnMinPlayTimeElapsed");
}

void UDialogueTransition::SetOwningNode(UDialogueSpeechNode* InNode)
{
	OwningNode = InNode;
}

void UDialogueTransition::StartTransition()
{
	//Reset end marker values
	bMinPlayTimeElapsed = false;
	bAudioFinished = false;

	//Verify owning node exists
	if (!OwningNode)
	{
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Transition failed to find owning node. Ending dialogue early."));

		OwningNode->GetDialogue()->EndDialogue();
		return;
	}

	//Get speaker
	UDialogueSpeakerComponent* Speaker = OwningNode->GetSpeaker();
	if (!Speaker)
	{
		UE_LOG(
			LogDialogueTree,
			Error, 
			TEXT("Transition failed to find speaker component. Ending dialogue early."));

		OwningNode->GetDialogue()->EndDialogue();
		return;
	}

	//Set timer for minimum play time
	float MinPlayTime = OwningNode->GetDetails().MinimumPlayTime;

	if (MinPlayTime > 0.01f)
	{
		Speaker->GetWorld()->GetTimerManager().SetTimer(
			MinPlayTimeHandle,
			OnTimerEnd,
			MinPlayTime,
			false
		);
	}
	//No minimum time
	else
	{
		bMinPlayTimeElapsed = true;
	}

	//Start listening to see when the audio content finishes 
	if (Speaker->IsPlaying())
	{
		Speaker->OnAudioFinished.AddUnique(OnContentEnd);
	}
	//No audio playing 
	else
	{
		bAudioFinished = true;
	}

	//If no minimum time or audio content, just transition out 
	if (bMinPlayTimeElapsed && bAudioFinished)
	{
		TransitionOut();
	}
}

void UDialogueTransition::Skip()
{
	if (!bAudioFinished)
	{
		OnDonePlayingContent();
	}

	if (!bMinPlayTimeElapsed)
	{
		OnMinPlayTimeElapsed();
	}

	//Let the speaker know we are skipping its speech
	OwningNode->GetSpeaker()->BroadcastSpeechSkipped(
		OwningNode->GetDetails()
	);
}

FText UDialogueTransition::GetDisplayName() const
{
	return FText::FromString(StaticClass()->GetName());
}

FText UDialogueTransition::GetNodeCreationTooltip() const
{
	return FText::GetEmpty();
}

EDialogueConnectionLimit UDialogueTransition::GetConnectionLimit() const
{
	return EDialogueConnectionLimit::Single;
}

void UDialogueTransition::CheckTransitionConditions()
{
	if (bAudioFinished && bMinPlayTimeElapsed && !OwningNode->GetIsBlocking())
	{
		TransitionOut();
	}
}

void UDialogueTransition::OnDonePlayingContent()
{
	//Unbind from audio event 
	UDialogueSpeakerComponent* Speaker = OwningNode->GetSpeaker();

	if (Speaker)
	{
		Speaker->Stop();
		Speaker->OnAudioFinished.Remove(OnContentEnd);
	}

	//Mark audio complete
	bAudioFinished = true;

	//See if we should transition out
	CheckTransitionConditions();
}

void UDialogueTransition::OnMinPlayTimeElapsed()
{
	// Invalidate the timer for the skipped nodes.
	const auto Speaker = OwningNode->GetSpeaker();
	if (Speaker)
	{
		Speaker->GetWorld()->GetTimerManager().ClearTimer(MinPlayTimeHandle);
	}

	//Mark min play time elapsed
	bMinPlayTimeElapsed = true;

	//Check if we should transition out
	CheckTransitionConditions();
}
