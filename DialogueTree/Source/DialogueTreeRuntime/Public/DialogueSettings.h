// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
//Plugin
#include "DialogueController.h"
//Generated
#include "DialogueSettings.generated.h"

class UUserWidget;

/**
* Enum used to track the desired input mode when switching into and out of 
* dialogue.
*/
UENUM(BlueprintType)
enum class EDLGInputMode : uint8
{
	GameOnly,
	GameAndUI,
	UIOnly
};

/**
* Helper enum for caching desired mouse lock mode. Necessary because the
* engine's mouse lock mode does not seem fully exposed to blueprint. 
* Effectively mirrors that enum. 
*/
UENUM(BlueprintType)
enum class EDLGMouseLockMode : uint8
{
	DoNotLock,
	LockOnCapture,
	LockAlways,
	LockInFullscreen
};

/**
* Struct defining the settings used to switch into an input mode by the 
* dialogue controller. 
*/
USTRUCT(BlueprintType)
struct DIALOGUETREERUNTIME_API FDLGInputModeSettings
{
	GENERATED_BODY()

	/** The target input mode to set */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	EDLGInputMode InputMode = EDLGInputMode::GameOnly;

	/** Whether the input should be flushed on changing the input mode */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	bool FlushInput = false;

	/** Whether the cursor should be hidden during capture. Applied only for
	* Game and UI mode. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	bool HideCursorDuringCapture = false;

	/** Mouse locking behavior. Mirrors EMouseLockMode. Not used for Game Only 
	* mode. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	EDLGMouseLockMode MouseLockMode = EDLGMouseLockMode::DoNotLock;
};

/**
 * Manages general settings/options for the Dialogue Tree plugin as a whole.
 */
UCLASS(Config=Engine, DefaultConfig, meta=(DisplayName="Dialogue Tree"))
class DIALOGUETREERUNTIME_API UDialogueSettings : public UDeveloperSettings
{
	GENERATED_BODY()

private: 
	/**
	* String path used to fetch default controller type. Slightly awkward but
	* allows users to skip the step of specifying if they want to use the 
	* default controller. If retrieval fails, an error message will ask them 
	* to manually specify as normal. 
	*/
	const FString DefaultControllerCoords = "/Script/Engine.Blueprint'/DialogueTree/Blueprints/Controllers/BP_BasicDialogueController.BP_BasicDialogueController_C'";
	
public:
	/** The type of dialogue controller to use for managing dialogue */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="General")
	TSoftClassPtr<ADialogueController> DialogueControllerType = 
		LoadClass<ADialogueController>(nullptr, *DefaultControllerCoords);

	/** The default minimum play time for a speech in dialogue. Can be 
	* overridden on individual speeches. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	float DefaultMinimumPlayTime = 3.f;

	/** 
	* The type of dialogue widget used to represent dialogue when using the 
	* default controller. Defaults to W_BasicDialogueDisplay if none. 
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, 
		Category = "DefaultController", 
		meta = (MustImplement = "/DialogueTree/DisplayWidgets/BI_SimpleDialogueDisplay.BI_SimpleDialogueDisplay_C"))
	TSoftClassPtr<UUserWidget> DialogueWidgetType;

	/** 
	* The type of dialogue option widget to be used in the default dialogue 
	* controller. Defaults to W_BasicDialogueOption.
	*/
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly,
		Category = "DefaultController",
		meta = (MustImplement = "/DialogueTree/DisplayWidgets/BI_SimpleDialogueDisplay.BI_SimpleDialogueOption_C"))
	TSoftClassPtr<UUserWidget> DialogueOptionWidgetType;

	/** The ZOrder of the widget in the player viewport when using the default 
	* controller. Should be set above most other widgets, as some widgets can 
	* consume UI input, even when collapsed. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, 
		Category = "DefaultController")
	int32 WidgetZOrder = 10000;

	/** The input mode settings to revert to when exiting dialogue
	* (when using the default controller) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly,
		Category = "DefaultController")
	FDLGInputModeSettings DefaultInputMode;

	/** Whether normal game input is permitted while navigating dialogue 
	* (when using the default controller) */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly,
		Category = "DefaultController")
	bool AllowGameInputInDialogue = true;
};
