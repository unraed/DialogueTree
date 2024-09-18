// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Slate/SGraphNodeDialogueOptionLock.h"
//Plugin
#include "Graph/Nodes/GraphNodeDialogueOptionLock.h"

#define LOCTEXT_NAMESPACE "SGraphNodeDialogueOptionLock"

void SGraphNodeDialogueOptionLock::Construct(const FArguments& InArgs, UEdGraphNode* InNode)
{
	//Cache the downcast lock node
	LockNode = Cast<UGraphNodeDialogueOptionLock>(InNode);
	check(LockNode);

	SGraphNodeDialogueBase::Construct(
		SGraphNodeDialogueBase::FArguments(),
		InNode
	);
}

TSharedRef<SWidget> SGraphNodeDialogueOptionLock::CreateNodeContentArea()
{
	check(LockNode);

	//Get text for all conditions
	TArray<FText> ConditionTexts = LockNode->GetConditionDisplayTexts();

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

FText SGraphNodeDialogueOptionLock::GetAnyAllText() const
{
	check(LockNode);
	if (LockNode->GetIfAny())
	{
		return LOCTEXT("AnyText", "Unlocked if Any:");
	}
	else
	{
		return LOCTEXT("AllText", "Unlocked if All:");
	}
}

TSharedRef<SWidget> SGraphNodeDialogueOptionLock::GetConditionBox(TArray<FText>& ConditionTexts, FText& AnyAllText) const
{
	TSharedRef<SVerticalBox> ConditionBox = SNew(SVerticalBox);

	//Add any vs all text
	ConditionBox->AddSlot()
	[
		SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle(
				"Bold", 
				BASE_FONT_SIZE
			))
			.Justification(TEXT_JUSTIFY)
			.Text(AnyAllText)
			.WrapTextAt(WRAP_TEXT_AT)
	];

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
