// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
//Plugin
#include "DialogueSettings.h"
//Generated
#include "DialogueManagerSubsystem.generated.h"

class ADialogueController;

/**
 * Subsystem used to manage dialogue following a Singleton-like pattern.
 * Lifespan follows the world. Essentially serves as a casing for the
 * polymorphic Dialogue Controller. Abstract to allow Blueprint subclass to
 * handle default values.
 */
UCLASS()
class DIALOGUETREERUNTIME_API UDialogueManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** UWorldSubsystem Impl. */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	/** End UWorldSubsystem */

	/**
	* Retrieves the associated dialogue controller actor. 
	* 
	* @return ADialogueController*, the dialogue controller. 
	*/
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	ADialogueController* GetCurrentController();

	/**
	* Retrieves the settings for the plugin. 
	* 
	* @return UDialogueSettings*, the settings for the plugin.
	*/
	UFUNCTION(BlueprintPure, Category="Dialogue")
	const UDialogueSettings* GetSettings();

private:
	/** The String type of dialogue controller that will be used if none is
	 * supplied in the project settings for the plugin.
	 * A little more brittle than I would like, but it should be fairly
	 * stable, given that it is just one part of the fallback chain. */
	const FString DefaultControllerTypeName = "BP_BasicDialogueController_C";

	/** The active dialogue controller */
	TObjectPtr<ADialogueController> DialogueController;
};
