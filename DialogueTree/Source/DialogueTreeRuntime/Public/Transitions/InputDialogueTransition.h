// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
//Plugin
#include "DialogueOption.h"
#include "DialogueTransition.h"
//Generated
#include "InputDialogueTransition.generated.h"

/**
 * Speech transition that presents options and waits for a player
 * choice. 
 */
UCLASS()
class DIALOGUETREERUNTIME_API UInputDialogueTransition : 
	public UDialogueTransition
{
	GENERATED_BODY()
	
public:
	/** DialogueTransition Implementation */
	virtual void StartTransition() override;
	virtual void TransitionOut() override;
	virtual void SelectOption(int32 InOptionIndex);
	virtual FText GetDisplayName() const override;
	virtual FText GetNodeCreationTooltip() const override;
	virtual EDialogueConnectionLimit GetConnectionLimit() const override;
	/** End DialogueTranstion */

private:
	/**
	* Displays the options for the user to select. 
	*/
	UFUNCTION()
	void ShowOptions();

	/**
	* Retrieves and caches the options for the transition. 
	*/
	UFUNCTION()
	void GetOptions();

private: 
	/** The available options for the player to choose */
	UPROPERTY()
	TArray<FDialogueOption> Options;
};
