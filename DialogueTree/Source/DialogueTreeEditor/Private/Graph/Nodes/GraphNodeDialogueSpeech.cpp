// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Nodes/GraphNodeDialogueSpeech.h"
//Plugin
#include "Dialogue.h"
#include "DialogueSpeakerSocket.h"
#include "Graph/DialogueEdGraph.h"
#include "Graph/DialogueEdGraphSchema.h"
#include "Nodes/DialogueSpeechNode.h"
#include "SpeechDetails.h"
#include "Transitions/InputDialogueTransition.h"

#define LOCTEXT_NAMESPACE "SpeechGraphNode"

UGraphNodeDialogueSpeech* UGraphNodeDialogueSpeech::MakeTemplate(UObject* Outer,
    UDialogueSpeakerSocket* InSpeaker, 
    TSubclassOf<UDialogueTransition> TransitionType)
{
    check(InSpeaker && Outer && TransitionType);

    UGraphNodeDialogueSpeech* NewSpeech =
        NewObject<UGraphNodeDialogueSpeech>(Outer);
    NewSpeech->SetSpeaker(InSpeaker);
    NewSpeech->SetTransitionType(TransitionType);

    return NewSpeech;
}

void UGraphNodeDialogueSpeech::PostEditChangeProperty(
    FPropertyChangedEvent& PropertyChangedEvent)
{
    FName PropertyName = PropertyChangedEvent.Property->GetFName();

    if (PropertyName.IsEqual("SpeechTitle"))
    {
        OnChangeSpeechTitle();
    }
    else if (PropertyName.IsEqual("TransitionType"))
    {
        OnChangeTransitionType();
    }

    Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UGraphNodeDialogueSpeech::AllocateDefaultPins()
{
    CreatePin(EGPD_Input, "MultipleNodes", FName());
    CreatePin(EGPD_Output, "MultipleNodes", FName());
}

FText UGraphNodeDialogueSpeech::GetTooltipText() const
{
    return LOCTEXT(
        "Tooltip", 
        "Represents a unit of dialogue to be performed by a Speaker"
    );
}

FName UGraphNodeDialogueSpeech::GetBaseID() const
{
    if (SpeechTitle.IsNone())
    {
        return FName("Speech");
    }
    else
    {
        return SpeechTitle;
    }
}

FLinearColor UGraphNodeDialogueSpeech::GetNodeTitleColor() const
{
    UDialogueEdGraph* Graph = GetDialogueGraph();
    if (!Graph || !Speaker.Speaker)
    {
        return FColor::White;
    }

    FName SpeakerName = Speaker.Speaker->GetSpeakerName();
    return Graph->GetSpeakerColor(SpeakerName);
}

FText UGraphNodeDialogueSpeech::GetContextMenuName() const
{
    return FText::FromString("Speech Node");
}

EDialogueConnectionLimit UGraphNodeDialogueSpeech::GetOutputConnectionLimit() 
    const
{
    check(TransitionType);
    UObject* DefaultObj = TransitionType->GetDefaultObject();
    UDialogueTransition* DefaultTransition =
        Cast<UDialogueTransition>(DefaultObj);
    check(DefaultTransition);

    return DefaultTransition->GetConnectionLimit();
}

void UGraphNodeDialogueSpeech::CreateAssetNode(UDialogue* InAsset)
{
    check(Speaker.Speaker);
    check(TransitionType);

    //Create node
    UDialogueSpeechNode* NewNode = 
        NewObject<UDialogueSpeechNode>(InAsset);
    SetAssetNode(NewNode);
    
    //Init data 
    FSpeechDetails SpeechDetails;
    SpeechDetails.SpeakerName = Speaker.Speaker->GetSpeakerName();
    SpeechDetails.bIgnoreContent = bIgnoreContent;
    SpeechDetails.SpeechText = SpeechText;
    SpeechDetails.SpeechAudio = SpeechAudio;
    SpeechDetails.MinimumPlayTime = MinimumPlayTime;
    SpeechDetails.bCanSkip = bCanSkip;
    SpeechDetails.BehaviorFlags = BehaviorFlags;

    NewNode->InitSpeechData(SpeechDetails, TransitionType);

    //Add node to the dialogue
    InAsset->AddNode(NewNode);
}

bool UGraphNodeDialogueSpeech::CanCompileNode()
{
    UDialogueEdGraph* Graph = GetDialogueGraph();

    if (TransitionType 
        && !TransitionType->HasAnyClassFlags(CLASS_Abstract)
        && Speaker.Speaker
        && Graph)
    {
        if (Graph->HasSpeaker(Speaker.Speaker->GetSpeakerName()))
        {
            SetErrorFlag(false);
            return true;
        }
    }

    SetErrorFlag(true);
    return false;
}

UClass* UGraphNodeDialogueSpeech::GetTransitionType() const
{
    check(TransitionType);
    return TransitionType;
}

void UGraphNodeDialogueSpeech::SetSpeaker(UDialogueSpeakerSocket* InSpeaker)
{
    Speaker.Speaker = InSpeaker;
}

FText UGraphNodeDialogueSpeech::GetSpeechText() const
{
    return SpeechText;
}

UDialogueSpeakerSocket* UGraphNodeDialogueSpeech::GetSpeaker() const
{
    if (Speaker.Speaker)
    {
        return Speaker.Speaker;
    }
    
    return nullptr;
}

void UGraphNodeDialogueSpeech::SetTransitionType(
    TSubclassOf<UDialogueTransition> InType)
{
    TransitionType = InType;
}

void UGraphNodeDialogueSpeech::OnChangeSpeechTitle()
{
    ResetID();
}

void UGraphNodeDialogueSpeech::OnChangeTransitionType()
{
    // Has more than one direct children (includes redirects)
    bool bMultipleDirectChildren = GetDirectChildren().Num() > 1;

    //Has more than one dialogue children (excludes redirects) 
    TArray<UGraphNodeDialogue*> DialogueChildren;
    GetChildren(DialogueChildren);
    bool bMultipleDialogueChildren = DialogueChildren.Num() > 1;

    //If too many linked nodes for new transition type 
    if (TransitionType 
        && !TransitionType->IsChildOf<UInputDialogueTransition>()
        && (bMultipleDirectChildren || bMultipleDialogueChildren))
    {
        //Break all outgoing pin links
        TArray<UEdGraphPin*> OutputPins = GetOutputPins();

        for (UEdGraphPin* Pin : OutputPins)
        {
            Pin->BreakAllPinLinks();
        }
    }
}

#undef LOCTEXT_NAMESPACE