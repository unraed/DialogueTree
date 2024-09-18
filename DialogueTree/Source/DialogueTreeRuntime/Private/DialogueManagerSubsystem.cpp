// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "DialogueManagerSubsystem.h"
//UE
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
//Plugin
#include "DialogueController.h"
#include "DialogueSettings.h"
#include "LogDialogueTree.h"

void UDialogueManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UDialogueManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UDialogueManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	//Remove any existing dialogue controllers from the world 
	TArray<AActor*> ExtantControllers;
	UGameplayStatics::GetAllActorsOfClass(
		&InWorld,
		ADialogueController::StaticClass(),
		ExtantControllers
	);

	for (AActor* OldController : ExtantControllers)
	{
		UE_LOG(
			LogDialogueTree,
			Warning,
			TEXT("Removing existing dialogue controller from world. Note that dialogue controllers manually placed in the level will not be used and can be safely deleted.")
		);

		OldController->Destroy();
	}

	//Get settings
	TSubclassOf<ADialogueController> ControllerType;
	UDialogueSettings* DLGSettings = GetMutableDefault<UDialogueSettings>();

	//Attempt to load controller type
	if (DLGSettings)
	{
		ControllerType = 
			DLGSettings->DialogueControllerType.LoadSynchronous();
	}

	//Attempt to spawn controller
	if (ControllerType)
	{
		DialogueController =
			InWorld.SpawnActor<ADialogueController>(ControllerType);
	}

	//If no controller was spawned, print error message
	if (!DialogueController)
	{
		UE_LOG(
			LogDialogueTree,
			Error,
			TEXT("Failed to spawn dialogue controller. Please specify a controller type under ProjectSettings>>DialogueTree>>DialogueControllerType")
		);
	}
}

ADialogueController* UDialogueManagerSubsystem::GetCurrentController()
{
	return DialogueController;
}

const UDialogueSettings* UDialogueManagerSubsystem::GetSettings()
{
	return GetDefault<UDialogueSettings>();
}
