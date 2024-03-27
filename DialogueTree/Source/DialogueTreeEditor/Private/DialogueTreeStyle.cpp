// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "DialogueTreeStyle.h"
//UE
#include "Styling/SlateStyleRegistry.h"

//Init the style set to be empty
TSharedPtr<FSlateStyleSet> FDialogueTreeStyle::TargetStyleSet = nullptr;

void FDialogueTreeStyle::Initialize()
{
	//Check if already registered
	if (TargetStyleSet.IsValid())
	{
		return;
	}

	//Get brush directory 
	TSharedPtr<IPlugin> FoundPlugin = 
		IPluginManager::Get().FindPlugin("DialogueTree");
	check(FoundPlugin.IsValid());

	FString TargetDirectory = FoundPlugin->GetBaseDir()
		/ TEXT("/Resources");

	//Spawn style set
	TargetStyleSet = MakeShareable(
		new FSlateStyleSet(TEXT("DialogueTreeStyle"))
	);
	check(TargetStyleSet);
	TargetStyleSet->SetContentRoot(TargetDirectory);

	//Register style set features
	RegisterTextStyles();
	RegisterIconBrushes();
	TargetStyleSet->Set(
		FName(TEXT("PinSize")), FVector2D(15.f)
	);

	//Register style set 
	FSlateStyleRegistry::RegisterSlateStyle(*TargetStyleSet.Get());
}

void FDialogueTreeStyle::Shutdown()
{
	if (TargetStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*TargetStyleSet.Get());
		ensure(TargetStyleSet.IsUnique());
		TargetStyleSet.Reset();
	}
}

const FName& FDialogueTreeStyle::GetStyleSetName()
{
	return TargetStyleSet->GetStyleSetName();
}

void FDialogueTreeStyle::RegisterBrush(FRegisterBrushArgs& InArgs)
{
	//Get Target Directory
	TSharedPtr<IPlugin> FoundPlugin =
		IPluginManager::Get().FindPlugin("DialogueTree");
	FString TargetDirectory = FoundPlugin->GetBaseDir()
		/ TEXT("/Resources");

	//Set up the brush
	FSlateImageBrush* ImageBrush = new FSlateImageBrush(
		TargetDirectory / InArgs.ImageName,
		InArgs.ImageSize
	);
	ImageBrush->DrawAs = InArgs.DrawType;
	ImageBrush->Margin = InArgs.DrawMargin;
	ImageBrush->TintColor = InArgs.TintColor;

	//Register
	TargetStyleSet->Set(
		FName(InArgs.PropertyName),
		ImageBrush
	);
}

const FSlateBrush* FDialogueTreeStyle::GetBrush(FName PropertyName)
{
	return TargetStyleSet->GetBrush(PropertyName);
}

const FTextBlockStyle& FDialogueTreeStyle::GetTextStyle(FName PropertyName)
{
	return TargetStyleSet->GetWidgetStyle<FTextBlockStyle>(PropertyName);
}

const FVector2D FDialogueTreeStyle::GetVector(FName PropertyName)
{
	return TargetStyleSet->GetVector(PropertyName);
}

bool FDialogueTreeStyle::IsValid()
{
	return TargetStyleSet.IsValid();
}

void FDialogueTreeStyle::RegisterTextStyles()
{
	check(TargetStyleSet.IsValid());

	//Register title text style 
	FRegisterTextStyleArgs TitleTextArgs;
	TitleTextArgs.Size = 12;

	TargetStyleSet->Set(
		"NodeTitleStyle",
		GetDerivativeTextStyle(TitleTextArgs)
	);

	//Register subtitle text style 
	FRegisterTextStyleArgs SubtitleTextArgs;
	SubtitleTextArgs.Size = 10;

	TargetStyleSet->Set(
		"NodeSubtitleStyle",
		GetDerivativeTextStyle(SubtitleTextArgs)
	);
}

FTextBlockStyle FDialogueTreeStyle::GetDerivativeTextStyle(
	FRegisterTextStyleArgs& InArgs)
{
	FSlateFontInfo Font = InArgs.BaseStyle.Font;
	Font.Size = 12;

	return FTextBlockStyle(InArgs.BaseStyle)
		.SetFont(Font)
		.SetColorAndOpacity(InArgs.TextColor)
		.SetShadowOffset(InArgs.ShadowOffset)
		.SetShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.7f));
}

void FDialogueTreeStyle::RegisterIconBrushes()
{
	//Standard Pin
	FRegisterBrushArgs StandardPinArgs;
	StandardPinArgs.PropertyName = "StandardPin";
	StandardPinArgs.ImageName = "CirclePin.png";
	RegisterBrush(StandardPinArgs);

	//Exit pin 
	FRegisterBrushArgs ExitPinArgs;
	ExitPinArgs.PropertyName = "ExitPin";
	ExitPinArgs.ImageName = "CirclePin.png";
	ExitPinArgs.TintColor = FSlateColor(FColor::Red);
	RegisterBrush(ExitPinArgs);

	//Auto Transition Icon
	FRegisterBrushArgs AutoTransitionArgs;
	AutoTransitionArgs.PropertyName = "AutoTransitionIcon";
	AutoTransitionArgs.ImageName = "DoubleArrowIcon.png";
	RegisterBrush(AutoTransitionArgs);

	//Input Transition Icon
	FRegisterBrushArgs InputTransitionArgs;
	InputTransitionArgs.PropertyName = "InputTransitionIcon";
	InputTransitionArgs.ImageName = "InputArrowIcon.png";
	RegisterBrush(InputTransitionArgs);

	//Node Color Spill
	FRegisterBrushArgs NodeColorSpillArgs;
	NodeColorSpillArgs.PropertyName = "NodeHeaderColorSpill";
	NodeColorSpillArgs.ImageName = "NodeHeaderColorSpill.png";
	NodeColorSpillArgs.ImageSize = FVector2D(64.f, 32.f);
	NodeColorSpillArgs.DrawType = ESlateBrushDrawType::Box;
	NodeColorSpillArgs.DrawMargin = FMargin(0.2f);
	RegisterBrush(NodeColorSpillArgs);

	//Node Gloss
	FRegisterBrushArgs NodeGlossArgs;
	NodeGlossArgs.PropertyName = "NodeHeaderGloss";
	NodeGlossArgs.ImageName = "NodeHeaderGloss.png";
	NodeGlossArgs.ImageSize = FVector2D(64.f, 25.f);
	NodeGlossArgs.DrawType = ESlateBrushDrawType::Box;
	NodeGlossArgs.DrawMargin = FMargin(0.2f);
	RegisterBrush(NodeGlossArgs);
}

