// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Slate/SGraphNodeDialogueEvent.h"
//Plugin
#include "Events/DialogueEvent.h"
#include "Graph/Nodes/GraphNodeDialogueEvent.h"

#define LOCTEXT_NAMESPACE "SGraphNodeDialogueEvent"

void SGraphNodeDialogueEvent::Construct(const FArguments& InArgs, 
	UEdGraphNode* InNode)
{
	//Cache the downcast event node
	EventNode = Cast<UGraphNodeDialogueEvent>(InNode);
	check(EventNode);
	
	SGraphNodeDialogueBase::Construct(
		SGraphNodeDialogueBase::FArguments(),
		InNode
	);
}

TSharedRef<SWidget> SGraphNodeDialogueEvent::CreateNodeContentArea()
{
	check(EventNode);
	TArray<FText> EventTexts = EventNode->GetGraphDescriptions();

	//If no event texts were found, return default
	if (EventTexts.IsEmpty())
	{
		return SGraphNodeDialogueBase::CreateNodeContentArea();
	}

	//Assemble widget
	return SNew(SOverlay)
		+ SOverlay::Slot()
		.Padding(EVENT_TEXT_PADDING)
		[
			GetDisplayTextBox(EventTexts)
		]
		+ SOverlay::Slot()
		[
			SGraphNodeDialogueBase::CreateNodeContentArea()
		];
}

TSharedRef<SWidget> SGraphNodeDialogueEvent::GetDisplayTextBox(
	TArray<FText>& InTexts) const
{
	TSharedRef<SVerticalBox> TextsBox = SNew(SVerticalBox);
	for (int32 i = 0; i < InTexts.Num(); ++i)
	{
		FText FormattedText = InTexts[i];

		//If not the last entry, add a comma to the end of the line
		if (i < InTexts.Num() - 1)
		{
			FormattedText = FText::Format(
				LOCTEXT("BaseEventText", "{0}{1}"),
				InTexts[i],
				LOCTEXT("Comma", ",")
			);
		}

		TextsBox->AddSlot()
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", BASE_FONT_SIZE))
			.Justification(TEXT_JUSTIFY)
			.Text(FormattedText)
			.WrapTextAt(WRAP_TEXT_AT)
		];
	}

	return TextsBox;
}

#undef LOCTEXT_NAMESPACE