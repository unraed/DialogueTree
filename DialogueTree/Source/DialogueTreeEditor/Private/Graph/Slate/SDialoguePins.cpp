// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Slate/SDialoguePins.h"
//UE
#include "Widgets/Layout/SWrapBox.h"
//Plugin
#include "DialogueTreeStyle.h"

void SDialoguePinBase::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments().SideToSideMargin(0.0f), InPin);
	CachePinIcons();
}

TSharedRef<SWidget> SDialoguePinBase::GetDefaultValueWidget()
{
	return SNew(SSpacer); //Empty value widget (no fill box with pin)
}

const FSlateBrush* SDialoguePinBase::GetPinIcon() const
{
	return DialoguePinImage;
}

void SDialogueStandardPin::Construct(const FArguments& InArgs,
	UEdGraphPin* InPin)
{
	SDialoguePinBase::Construct(SDialoguePinBase::FArguments(), InPin);
}

void SDialogueStandardPin::CachePinIcons()
{
	check(FDialogueTreeStyle::IsValid());
	DialoguePinImage = FDialogueTreeStyle::GetBrush("StandardPin");
}

void SDialogueExitPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SDialoguePinBase::Construct(SDialoguePinBase::FArguments(), InPin);
}

void SDialogueExitPin::CachePinIcons()
{
	check(FDialogueTreeStyle::IsValid());
	DialoguePinImage = FDialogueTreeStyle::GetBrush("ExitPin");
}
