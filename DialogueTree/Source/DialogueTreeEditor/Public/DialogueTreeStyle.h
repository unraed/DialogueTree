// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateStyle.h"
//Generated
#include "DialogueTreeStyle.generated.h"

/**
* Struct representing details of a brush for dialogue system. 
*/
USTRUCT()
struct FRegisterBrushArgs
{
	GENERATED_BODY()

	/** The name of the property to register */
	FString PropertyName;

	/** The name of the source image, including filetype suffix */
	FString ImageName;

	/** Desired size of the image. Defaults to 22x22 pixels. */
	FVector2D ImageSize = FVector2D(22.f);

	/** Desired draw type for the image. Defaults to image. */
	ESlateBrushDrawType::Type DrawType = ESlateBrushDrawType::Image;

	/** Desired draw margin for the image */
	FMargin DrawMargin = FMargin();

	/** Desired color for the image. Defaults to white. */
	FSlateColor TintColor = FSlateColor(FColor::White);
};

/**
* Struct representing details needed to register a text style for the dialogue
* system. 
*/
USTRUCT()
struct FRegisterTextStyleArgs
{
	GENERATED_BODY()

	/** The base style for the new text style */
	FTextBlockStyle BaseStyle = 
		FEditorStyle::GetWidgetStyle<FTextBlockStyle>("Graph.Node.NodeTitle");

	/** The desired font size */
	int32 Size = 10; 

	/** The shadow offset for the text */
	FVector2D ShadowOffset = FVector2D(2.f, 2.f);

	/** The desired color for the text */
	FLinearColor TextColor = FLinearColor::White;
};

/**
* A place to cache images, text styles, etc. Visual features to be used by the 
* dialogue editor. 
*/
class DIALOGUETREEEDITOR_API FDialogueTreeStyle 
{
public:
	/** 
	* Initializes the style.  
	*/
	static void Initialize();

	/**
	* Shuts down the style. 
	*/
	static void Shutdown();

	/**
	* Gets the name of the target style set.
	* 
	* @return const FName& - retrieved name. 
	*/
	static const FName& GetStyleSetName();

	/**
	* Registers a slate brush with the style set. 
	* 
	* @param InArgs - FRegisterBrushArgs&, Brush arguments. 
	*/
	static void RegisterBrush(FRegisterBrushArgs& InArgs);

	/**
	* Retrieves a slate brush associated with the given name. 
	* 
	* @param PropertyName - the name of the brush. 
	* @return const FSlateBrush* - the retrieved brush. 
	*/
	static const FSlateBrush* GetBrush(FName PropertyName);

	/**
	* Retrieves a text style from the style set. 
	* 
	* @param PropertyName - FName, name of the sought text style. 
	* @return const FTextBlockStyle& - the retrieved style.
	*/
	static const FTextBlockStyle& GetTextStyle(FName PropertyName);

	/**
	* Retrieves a stored 2D vector from the style set. 
	* 
	* @param PropertyName - FName, name of the vector in the style
	* set. 
	* @return const FVector2D - the retrieved 2D vector. 
	*/
	static const FVector2D GetVector(FName PropertyName);

	/**
	* Verifies the style set is valid. 
	* 
	* @return bool - true if valid, false otherwise. 
	*/
	static bool IsValid();

private: 
	/**
	* Helper function for registering all desired text styles with
	* the style set. 
	*/
	static void RegisterTextStyles();

	/**
	* Helper function for modifying an existing text style. 
	* 
	* @param InArgs - FRegisterTextStyleArgs, struct representing 
	* desired modifications on style. 
	* @return FTextBlockStyle - The completed text style. 
	*/
	static FTextBlockStyle GetDerivativeTextStyle(
		FRegisterTextStyleArgs& InArgs);

	/**
	* Helper function for registering all desired slate brushes with
	* the style set. 
	*/
	static void RegisterIconBrushes();

private:
	/** The style set to fill */
	static TSharedPtr<FSlateStyleSet> TargetStyleSet;
};