// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//Plugin
#include "Nodes/DialogueSpeechNode.h"
//Generated
#include "Dialogue.generated.h"

class ADialogueController;
class UDialogueEntryNode;
class UDialogueNode;
class UDialogueSpeakerComponent;
class UDialogueSpeakerSocket;
class UEdGraph;

DECLARE_DELEGATE(FSpeakerRolesChangedSignature);

/**
* Enum defining various compile statuses for a dialogue.
*/
UENUM()
enum class EDialogueCompileStatus
{
	Compiled,
	Uncompiled,
	Failed
};

/**
* Struct representing a visual entry for Speaker data. Defined here to avoid
* having to reparent speakers on compiling the graph. 
*/
USTRUCT()
struct FSpeakerField
{
	GENERATED_BODY()

	UPROPERTY(NoClear, meta=(NoResetToDefault))
	TObjectPtr<UDialogueSpeakerSocket> SpeakerSocket = nullptr;

	UPROPERTY(EditAnywhere, NoClear, Category = "Dialogue", 
		meta=(NoResetToDefault))
	FColor GraphColor = FColor::White;
};

/**
* Struct defining default colors for dialogue speakers in the graph. Defined 
* here to avoid having to reparent speakers on compile. 
*/
USTRUCT()
struct FDefaultDialogueColors
{
	GENERATED_BODY()

public:
	FDefaultDialogueColors() {};

	FDefaultDialogueColors(TArray<FColor> InColors)
		: Colors(InColors)
	{};

public:
	/**
	* Retrieves the next color in the default colors array, cycling back around
	* to the beginning on reaching the end of the defined colors.
	*
	* @return FColor - the next default color.
	*/
	FColor PopColor();

private:
	/** Default colors for new speakers added into the graph */
	UPROPERTY()
	TArray<FColor> Colors
	{
		FColor(34, 139, 34), //Forest green
		FColor(65.f, 105.f, 225.f), //Royal blue
		FColor(178.f, 34.f, 34.f), //Firebrick Red
		FColor(255, 255, 0), //Yellow
		FColor(255, 165, 0), //Orange
		FColor(0, 255, 255), //Cyan
		FColor(199, 21, 133), //Violet
		FColor(147, 112, 219), //Purple
		FColor(255, 192, 203), //Pink
		FColor(152, 251, 152) //Pale green
	};

	/** The next color to pull from when getting the default color */
	UPROPERTY()
	uint32 ColorIndex = 0;
};

/**
 * Dialogue asset
 */
UCLASS(BlueprintType)
class DIALOGUETREERUNTIME_API UDialogue : public UObject
{
	GENERATED_BODY()

public:
	UDialogue();

public: 
#if WITH_EDITOR
	/** UObject Impl. */
	virtual void PostEditChangeProperty(
		struct FPropertyChangedEvent& PropertyChangedEvent) override;
	/** End UObject */
#endif

	/**
	* Sets the component value associated with the given name 
	* to the provided speaker component. BlueprintCallable. 
	* 
	* @param InName - FName, the dialogue's name for the speaker. 
	* Can differ from the component's display name. 
	* @param InSpeaker - UDialogueSpeakerComponent*, the component 
	* associated with the speaker. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetSpeaker(FName InName, UDialogueSpeakerComponent* InSpeaker);

	/**
	* Retrieves the speaker component associated with the given 
	* name by the dialogue. BlueprintCallable. 
	* 
	* @param InName - FName, name associated with the desired 
	* speaker
	* @return UDialogueSpeakerComponent*, component associated with 
	* the given speaker name. Nullptr if none found. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	UDialogueSpeakerComponent* GetSpeaker(FName InName) const;

	/**
	* Adds a speaker entry for the given name with a component value
	* of nullptr. 
	* 
	* @param InName - FName, name to create an entry for.
	*/
	void AddSpeakerEntry(FName InName);

	/**
	* Opens the dialogue at the given node ID. 
	* 
	* @param InNodeID - FName, the target node to start at. 
	* @param InController - ADialogueController*, DialogueController
	* actor. 
	* @param InSpeakers - TMap<FName, UDialogueSpeakerComponent*>, 
	* components to associate with expected speaker names. 
	*/
	void OpenDialogueAt(FName InNodeID, ADialogueController* InController,
		TMap<FName, UDialogueSpeakerComponent*> InSpeakers);

	/**
	* Clears the current dialogue controller. Used when closing 
	* dialogue. 
	*/
	void ClearController();

	/**
	* Calls on the controller to end the dialogue. 
	*/
	void EndDialogue() const;

	/**
	* Calls on the controller to display the given speech. 
	* 
	* @param InDetails - const FSpeechDetails&, details for the 
	* target speech. 
	*/
	void DisplaySpeech(const FSpeechDetails& InDetails) const;

	/**
	* Calls on the controller to display the given dialogue options
	* for the user to select from. 
	* 
	* @param InOptions - TArray<FDialogueOption>, options to
	* display.
	*/
	void DisplayOptions(TArray<FDialogueOption> InOptions) const;

	/**
	* Attempts to select a dialogue option at the given index. 
	* 
	* @param InOptionIndex - int32, index of the selection. 
	*/
	void SelectOption(int32 InOptionIndex) const;

	/**
	* Attempts to skip through the current dialogue node, if and 
	* to the extent allowable by the node itself. 
	*/
	void Skip() const;

	/**
	* Attempts to traverse the given node. Closes the dialogue if 
	* anything goes wrong. 
	* 
	* @param InNode - UDialogueNode*, node to traverse. 
	*/
	void TraverseNode(UDialogueNode* InNode);

	/**
	* Retrieves the dialogue's current compile status.
	* 
	* @return EDialogueCompileStatus 
	*/
	EDialogueCompileStatus GetCompileStatus() const;

	/**
	* Retrieves the entire map of expected speaker names to their 
	* speaker components. 
	* 
	* @return TMap<FName, UDialogueSpeakerComponent*>, the map of 
	* speaker names to components. 
	*/
	UFUNCTION(BlueprintCallable, Category="Dialogue")
	TMap<FName, UDialogueSpeakerComponent*> GetAllSpeakers() const;

	/**
	* Checks if a speaker with the given name is currently present/valid for 
	* the dialogue. 
	* 
	* @param SpeakerName - FName, name of the target speaker. 
	* @return bool - True if the speaker is valid/present; false otherwise. 
	*/
	UFUNCTION(BlueprintCallable, Category="Dialogue")
	bool SpeakerIsPresent(const FName SpeakerName) const;
	
	/**
	* Checks if the given node has already been visited by the dialogue 
	* controller. If dialogue is inactive, returns false. 
	* 
	* @param TargetNode - UDialogueNode*, the node we are interested in. 
	* @return bool - True if node was visited, False otherwise. 
	*/
	bool WasNodeVisited(UDialogueNode* TargetNode) const;

	/**
	* Marks the given node visited or unvisited as specified. 
	* 
	* @param TargetNode - UDialogueNode* to change visited status for. 
	* @param bVisited - bool, True for visited, False for unvisited. 
	*/
	void MarkNodeVisited(UDialogueNode* TargetNode, bool bVisited);

	/**
	* Marks all nodes in the dialogue unvisited. 
	*/
	void ClearAllNodeVisits();

	/**
	* Checks if the given node ID corresponds to a node in the dialogue. 
	* 
	* @param NodeID - FName, the target node id. 
	* @return True if the dialogue contains the node, False otherwise.
	*/
	bool HasNode(FName NodeID) const;

	/**
	* Marks the given node as the dialogue's resume node if possible.
	* 
	* @param InNode - UDialogueNode* - the node to resume from. 
	*/
	void SetResumeNode(UDialogueNode* InNode);

	/**
	* Retrieves the dialogue's root node.
	*
	* @return UDialogueNode*, the root of the dialogue.
	*/
	UDialogueNode* GetRootNode() const;

#if WITH_EDITOR
public:
	/**
	* Retrieve the editor graph associated with this dialogue. 
	* 
	* @return UEdGraph*, the editor graph for the dialogue. 
	*/
	UEdGraph* GetEdGraph() const;

	/**
	* Set the editor graph associated with this dialogue. 
	* 
	* @param InEdGraph - UEdGraph*, graph to set. 
	*/
	void SetEdGraph(UEdGraph* InEdGraph);

	/**
	* Retrieves a map of speaker names to their role structs in the dialogue. 
	* 
	* @return const TMap<FName, FSpeakerField>& - the speaker role map.
	*/
	const TMap<FName, FSpeakerField>& GetSpeakerRoles() const;

	/**
	* Add the given node to the dialogue. 
	* 
	* @param InNode - UDialogueNode*, node to add. 
	*/
	void AddNode(UDialogueNode* InNode);

	/**
	* Remove the given node from the dialogue. 
	* 
	* @param InNode - UDialogueNode*, node to remove. 
	*/
	void RemoveNode(UDialogueNode* InNode);

	/**
	* Sets the root node for the dialogue. 
	* 
	* @param InNode - UDialogueNode*, node to set as the root. 
	*/
	void SetRootNode(UDialogueNode* InNode);

	/**
	* Completely empties the current dialogue of all of its nodes, 
	* rendering it blank. 
	*/
	void ClearDialogue();

	/**
	* Functionality to call at the beginning of compiling the dialogue.
	*/
	void PreCompileDialogue();

	/**
	* Sets the compile status for the dialogue. Only a status of
	* "Compiled" will allow the dialogue to play in game.
	*
	* @param InStatus - EDialogueCompileStatus, new compile status.
	*/
	void SetCompileStatus(EDialogueCompileStatus InStatus);
#endif

private: 
	/**
	* Adds the default set of speakers into the graph.
	*/
	void AddDefaultSpeakers();

	/**
	* Behaviors to trigger when the speakers map changes in some way.
	*
	* @param ChangeType - const EPropertyChangeType::Type&, the type of change.
	*/
	void OnChangeSpeakers(const EPropertyChangeType::Type& ChangeType);

	/**
	* Behaviors for when when a new speaker's entry is added to the graph.
	*/
	void OnAddSpeaker();

	/**
	* Behaviors for when a speaker's entry is removed from the graph.
	*/
	void OnRemoveSpeaker();

	/**
	* Behaviors for when a speaker's entry is changed in some way.
	*/
	void OnChangeSingleSpeaker();

	/**
	* Checks if the dialogue is ready to play. Fills the provided 
	* error message if not. 
	* 
	* @param InController - ADialogueController*, controller that
	* would interface with the dialogue
	* @param OutErrorMessage - FString&, error message to fill if
	* the dialogue cannot play. 
	* @return bool, whether the dialogue can play or not. 
	*/
	bool CanPlay(ADialogueController* InController, FString& OutErrorMessage) 
		const;

	/**
	* Refreshes the speakers, plugging in the provided components. 
	* 
	* @param InSpeakers - TMap<FName, UDialogueSpeakerComponent*>,
	* the speaker components to enter, matched to their expected
	* names
	*/
	void FillSpeakers(TMap<FName, UDialogueSpeakerComponent*> InSpeakers);

private:
	/** Editable speaking roles for the graph */
	UPROPERTY(EditAnywhere, NoClear, Category = "Dialogue", 
		meta=(NoResetToDefault))
	TMap<FName, FSpeakerField> SpeakerRoles;

	/** The list of all nodes in the dialogue */
	UPROPERTY()
	//TArray<UDialogueNode*> DialogueNodes;
	TMap<FName, TObjectPtr<UDialogueNode>> DialogueNodes;

	/** The entry node for the dialogue */
	UPROPERTY()
	TObjectPtr<UDialogueEntryNode> RootNode; 

	/** The currently active node in the dialogue */
	UPROPERTY()
	TObjectPtr<UDialogueNode> ActiveNode;

	/** A mapping of speaker names to their found components */
	UPROPERTY()
	TMap<FName, TObjectPtr<UDialogueSpeakerComponent>> Speakers;

	/** The controlling actor for the dialogue */
	UPROPERTY()
	TObjectPtr<ADialogueController> DialogueController;

	/** Thhe current compile status of the dialogue */
	UPROPERTY()
	EDialogueCompileStatus CompileStatus = EDialogueCompileStatus::Uncompiled;

	/** The default colors for the speakers in the graph */
	UPROPERTY()
	FDefaultDialogueColors DefaultSpeakerColors;

#if WITH_EDITORONLY_DATA

	/** The editor graph associated with this dialogue */
	UPROPERTY()
	TObjectPtr<UEdGraph> EdGraph = nullptr;

#endif

public:
	/** Delegate called when speaker roles changed */
	FSpeakerRolesChangedSignature OnSpeakerRolesChanged;
};
