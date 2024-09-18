// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Slate/SGraphNodeDialogueSpeech.h"
//Plugin
#include "DialogueSpeakerSocket.h"
#include "DialogueTreeStyle.h"
#include "Graph/Nodes/GraphNodeDialogueSpeech.h"
#include "Graph/Slate/SDialoguePins.h"
#include "Transitions/InputDialogueTransition.h"

#define LOCTEXT_NAMESPACE "SGraphNodeDialogueSpeech"

void SGraphNodeDialogueSpeech::Construct(const FArguments& InArgs, 
	UEdGraphNode* InNode)
{
	//Cache the downcast speech node 
	SpeechNode = Cast<UGraphNodeDialogueSpeech>(InNode);
	check(SpeechNode);

	SGraphNodeDialogueBase::Construct(
		SGraphNodeDialogueBase::FArguments(), 
		InNode
	);
}

TSharedRef<SWidget> SGraphNodeDialogueSpeech::CreateNodeContentArea()
{
	check(SpeechNode);
	FText SpeechText = SpeechNode->GetSpeechText();

	if (SpeechText.IsEmptyOrWhitespace())
	{
		return SGraphNodeDialogueBase::CreateNodeContentArea();
	}
	else
	{
		return SNew(SOverlay)
		+ SOverlay::Slot()
		.Padding(10.f, 7.f, 10.f, 7.f)
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", BASE_FONT_SIZE))
			.Justification(TEXT_JUSTIFY)
			.Text(SpeechText)
			.WrapTextAt(WRAP_TEXT_AT)
		]
		+ SOverlay::Slot()
		[
			SGraphNodeDialogueBase::CreateNodeContentArea()
		];
	}
}

TSharedRef<SWidget> SGraphNodeDialogueSpeech::CreateNodeTitleWidget()
{
	//Get graph
	check(SpeechNode);
	UDialogueEdGraph* DialogueGraph = 
		SpeechNode->GetDialogueGraph();
	check(DialogueGraph);

	//Assemble widget 
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(TITLE_BOX_PADDING)
		.FillWidth(1.f)
		[
			GetTitleTextBox()
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(ICON_PADDING)
		[
			SNew(SImage)
			.Image(GetTransitionIcon())
		];
}

const FSlateBrush* SGraphNodeDialogueSpeech::GetTransitionIcon() const
{
	check(SpeechNode);
	UClass* TransitionType = SpeechNode->GetTransitionType();
	if (TransitionType == UInputDialogueTransition::StaticClass())
	{
		return FDialogueTreeStyle::GetBrush("InputTransitionIcon");
	}
	else
	{
		return FDialogueTreeStyle::GetBrush("AutoTransitionIcon");
	}
}

FText SGraphNodeDialogueSpeech::GetSpeakerNameText() const
{
	check(SpeechNode);
	UDialogueSpeakerSocket* SpeakerSocket =
		SpeechNode->GetSpeaker();
	FText SpeakerNameText = LOCTEXT(
		"ErrorNameText",
		"Error: Speaker Removed"
	);

	if (SpeakerSocket && !SpeakerSocket->GetSpeakerName().IsNone())
	{
		SpeakerNameText =
			FText::FromName(SpeakerSocket->GetSpeakerName());
	}

	return SpeakerNameText;
}

FText SGraphNodeDialogueSpeech::GetNodeSubtitleText() const
{
	check(SpeechNode);

	FText SubtitleText = FText::FromName(SpeechNode->GetID());

	if (SpeechNode->GetNumEvents() > 0)
	{
		SubtitleText = FText::Format(
			LOCTEXT("EventSubtitle", "{0}  (+Events)"),
			SubtitleText
		);
	}

	return SubtitleText;
}

TSharedRef<SWidget> SGraphNodeDialogueSpeech::GetSpeakerNameWidget() const
{
	FTextBlockStyle TitleStyle =
		FDialogueTreeStyle::GetTextStyle(
			"NodeTitleStyle"
		);

	return SNew(STextBlock)
		.TextStyle(&TitleStyle)
		.Justification(TEXT_JUSTIFY)
		.Text(GetSpeakerNameText());
}

TSharedRef<SWidget> SGraphNodeDialogueSpeech::GetNodeSubtitleWidget() const
{
	FTextBlockStyle SubTitleStyle =
		FDialogueTreeStyle::GetTextStyle(
			"NodeTitleStyle"
		);
	SubTitleStyle.SetFontSize(BASE_FONT_SIZE);

	return SNew(STextBlock)
		.TextStyle(&SubTitleStyle)
		.Justification(TEXT_JUSTIFY)
		.Text(GetNodeSubtitleText())
		.WrapTextAt(WRAP_TEXT_AT - SUBTITLE_WRAP_DIFFERENCE);
}

TSharedRef<SWidget> SGraphNodeDialogueSpeech::GetTitleTextBox() const
{
	return SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	[
		GetSpeakerNameWidget()
	]
	+ SVerticalBox::Slot()
	.Padding(SUBTITLE_PADDING)
	.AutoHeight()
	[
		GetNodeSubtitleWidget()
	];
}

#undef LOCTEXT_NAMESPACE