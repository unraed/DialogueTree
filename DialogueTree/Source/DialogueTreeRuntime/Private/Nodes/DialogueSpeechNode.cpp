// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Nodes/DialogueSpeechNode.h"
//Plugin
#include "Dialogue.h"
#include "DialogueSpeakerComponent.h"
#include "LogDialogueTree.h"
#include "Transitions/DialogueTransition.h"

void UDialogueSpeechNode::InitSpeechData(FSpeechDetails& InDetails,
	TSubclassOf<UDialogueTransition> TransitionType)
{
	check(TransitionType);
	Details = InDetails;
	Transition = NewObject<UDialogueTransition>(this, TransitionType);
	Transition->SetOwningNode(this);
}

FSpeechDetails UDialogueSpeechNode::GetDetails() const
{
	return Details;
}

UDialogueSpeakerComponent* UDialogueSpeechNode::GetSpeaker() const
{
	return Dialogue->GetSpeaker(Details.SpeakerName);
}

bool UDialogueSpeechNode::GetCanSkip() const
{
	return Details.bCanSkip;
}

void UDialogueSpeechNode::SelectOption(int32 InOptionIndex)
{
	Transition->SelectOption(InOptionIndex);
}

void UDialogueSpeechNode::EnterNode()
{
	//Call super
	Super::EnterNode();

	//Verify speaker is actually present
	if (!Dialogue->SpeakerIsPresent(Details.SpeakerName))
	{
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Terminating dialogue early: A participant speaker component was not found. Verify that the dialogue name property matches the speaker's role in the dialogue.")
		);

		Dialogue->EndDialogue();
		return;
	}

	if (!Details.bIgnoreContent)
	{
		//Display the current speech
		Dialogue->DisplaySpeech(Details);

		//Play any audio and set any flags for the speaker
		StartAudio();
	}
	
	//If no transition, throw an error and close the dialogue 
	if (!Transition)
	{
		UE_LOG(
			LogDialogueTree, 
			Error, 
			TEXT("Speech node is missing transition.")
		);
		Dialogue->EndDialogue();
		return; 
	}

	//Play the transition 
	Transition->StartTransition();
}

void UDialogueSpeechNode::Skip()
{
	if (Details.bCanSkip)
	{
		Transition->Skip();
	}
}

FDialogueOption UDialogueSpeechNode::GetAsOption()
{
	return FDialogueOption{ Details, this };
}

void UDialogueSpeechNode::StartAudio()
{
	UDialogueSpeakerComponent* Speaker = GetSpeaker();

	if (Speaker)
	{
		//Play any audio
		Speaker->Stop();

		if (Details.SpeechAudio)
		{
			Speaker->SetSound(Details.SpeechAudio);
			Speaker->Play();
		}

		//Set any behavior flags
		Speaker->SetBehaviorFlags(Details.BehaviorFlags);
	}
}