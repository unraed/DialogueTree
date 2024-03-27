// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Slate/SGraphNodeDialogueJump.h"
//Plugin
#include "Graph/Nodes/GraphNodeDialogueJump.h"

#define LOCTEXT_NAMESPACE "SGraphNodeDialogueJump"

void SGraphNodeDialogueJump::Construct(const FArguments& InArgs, 
	UEdGraphNode* InNode)
{
	/** Cache the downcast jump node */
	JumpNode = Cast<UGraphNodeDialogueJump>(InNode);
	check(JumpNode);
	
	SGraphNodeDialogueBase::Construct(
		SGraphNodeDialogueBase::FArguments(), 
		InNode
	);
}

TSharedRef<SWidget> SGraphNodeDialogueJump::CreateNodeContentArea()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(CONTENT_PADDING)
		[
			SNew(STextBlock)
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", BASE_FONT_SIZE))
			.Justification(TEXT_JUSTIFY)
			.Text(GetDisplayText())
			.WrapTextAt(WRAP_TEXT_AT)
		];
}

FText SGraphNodeDialogueJump::GetDisplayText() const
{ 
	check(JumpNode);
	if (JumpNode->GetJumpTarget() == nullptr)
	{
		return LOCTEXT(
			"DefaultDisplayText", 
			"Jump Target Not Set"
		);
	}
	else
	{
		FText BaseText = LOCTEXT("BaseText", "Jump to {0}");
		FText TargetName = 
			FText::FromName(JumpNode->GetJumpTarget()->GetID());
		return FText::Format(BaseText, TargetName);
	}
}

#undef LOCTEXT_NAMESPACE
