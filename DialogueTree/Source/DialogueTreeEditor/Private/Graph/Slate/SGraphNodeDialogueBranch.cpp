// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Slate/SGraphNodeDialogueBranch.h"
//Plugin
#include "DialogueTreeStyle.h"
#include "Graph/Nodes/GraphNodeDialogue.h"
#include "Graph/Nodes/GraphNodeDialogueBranch.h"

#define LOCTEXT_NAMESPACE "SGraphNodeDialogueBranch"

void SGraphNodeDialogueBranch::Construct(const FArguments& InArgs, 
	UEdGraphNode* InNode)
{
	//Cache the downcast branch node
	BranchNode = Cast<UGraphNodeDialogueBranch>(InNode);
	check(BranchNode);
	
	SGraphNodeDialogueBase::Construct(
		SGraphNodeDialogueBase::FArguments(), 
		InNode
	);
}

TSharedRef<SWidget> SGraphNodeDialogueBranch::CreateNodeContentArea()
{
	check(BranchNode);

	//Get text for all conditions
	TArray<FText> ConditionTexts = BranchNode->GetConditionDisplayTexts();

	//If no condition texts, return default content area
	if (ConditionTexts.IsEmpty())
	{
		return SGraphNodeDialogueBase::CreateNodeContentArea();
	}

	//Get the any/all header text
	FText AnyAllText = GetAnyAllText();

	//Return completed widget
	return SNew(SOverlay)
		+ SOverlay::Slot()
		.Padding(CONDITION_BOX_PADDING)
		[
			GetConditionBox(ConditionTexts, AnyAllText)
		]
		+ SOverlay::Slot()
		[
			SGraphNodeDialogueBase::CreateNodeContentArea()
		];
}

TSharedRef<SWidget> SGraphNodeDialogueBranch::CreateOutputPinArea()
{
	FTextBlockStyle PinLabelStyle =
		FDialogueTreeStyle::GetTextStyle(
			"NodeSubtitleStyle"
		);

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(STextBlock)
				.TextStyle(&PinLabelStyle)
				.Text(LOCTEXT("IfText", "If"))
				.Justification(PIN_LABEL_JUSTIFY)
			]
			+ SHorizontalBox::Slot()
			[
				SNew(STextBlock)
				.TextStyle(&PinLabelStyle)
				.Text(LOCTEXT("ElseText", "Else"))
				.Justification(PIN_LABEL_JUSTIFY)
			]
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(OutputPinBox, SHorizontalBox)
		];
}

float SGraphNodeDialogueBranch::GetOutputPinYPadding() const
{
	return OUTPUT_PIN_PUSH_AMOUNT;
}

FText SGraphNodeDialogueBranch::GetAnyAllText() const
{
	check(BranchNode);
	if (BranchNode->GetIfAny())
	{
		return LOCTEXT("AnyText", "If Any:");
	}
	else
	{
		return LOCTEXT("AllText", "If All:");
	}
}

TSharedRef<SWidget> SGraphNodeDialogueBranch::GetConditionBox(
	TArray<FText>& ConditionTexts, FText& AnyAllText) const
{
	TSharedRef<SVerticalBox> ConditionBox = SNew(SVerticalBox);

	//Add any vs all text
	if (ConditionTexts.Num() > 1)
	{
		ConditionBox->AddSlot()
			[
				SNew(STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", BASE_FONT_SIZE))
				.Justification(TEXT_JUSTIFY)
				.Text(AnyAllText)
				.WrapTextAt(WRAP_TEXT_AT)
			];
	}

	//Add all condition texts 
	for (int32 i = 0; i < ConditionTexts.Num(); ++i)
	{
		bool bIsLast = i == ConditionTexts.Num() - 1;
		FText JoinText = bIsLast ? FText::FromString("") 
								 : FText::FromString(",");

		FText ConditionEntryText = FText::Format(
			LOCTEXT("BaseConditionText", "{0}{1}"),
			ConditionTexts[i],
			JoinText
		);

		ConditionBox->AddSlot()
			[
				SNew(STextBlock)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", BASE_FONT_SIZE))
				.Justification(TEXT_JUSTIFY)
				.Text(ConditionEntryText)
				.WrapTextAt(WRAP_TEXT_AT)
			];
	}

	return ConditionBox;
}

#undef LOCTEXT_NAMESPACE