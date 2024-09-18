// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once 

//UE
#include "CoreMinimal.h"
//Plugin
#include "SGraphNodeDialogueBase.h"

class UGraphNodeDialogueSpeech;

/**
* Governs the visual representation of a speech node in the dialogue graph.
*/
class DIALOGUETREEEDITOR_API SGraphNodeDialogueSpeech : 
	public SGraphNodeDialogueBase
{
public:
	//Slate Arguments 
	SLATE_BEGIN_ARGS(SGraphNodeDialogueSpeech) {}
	SLATE_END_ARGS()

	/** Slate Constructor */
	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);

protected:
	/** SGraphNode Implementation */
	virtual TSharedRef<SWidget> CreateNodeContentArea() override;
	/** End SGraphNode */

	/** SGraphNodeDialogueBase Implementation */
	virtual TSharedRef<SWidget> CreateNodeTitleWidget() override;
	/** End SGraphNodeDialogueBase */

private:
	/**
	* Retrieves the FSlateBrush icon matched to the speech node's transition 
	* type. 
	* 
	* @param SpeechNode - UGraphNodeDialogueSpeech*, the owning speech node. 
	* @return const FSlateBrush* - the transition icon. 
	*/
	const FSlateBrush* GetTransitionIcon() const;

	/**
	* Retrieves the display text for the speaker's name. 
	* 
	* @param SpeechNode - UGraphNodeDialogueSpeech*, the owning speech node.
	* @return FText - The speaker's name as text. 
	*/
	FText GetSpeakerNameText() const;

	/**
	* Retrieves the display text for the node's ID. 
	* 
	* @param SpeechNode - UGraphNodeDialogueSpeech*, the owning speech node.
	* @return FText - the node's ID text. 
	*/
	FText GetNodeSubtitleText() const;

	/**
	* Gets the text block widget for the speaker's name. 
	* 
	* @param SpeechNode - UGraphNodeDialogueSpeech*, the owning speech node.
	* @return TSharedRef<SWidget> - the built widget. 
	*/
	TSharedRef<SWidget> GetSpeakerNameWidget() const;

	/**
	* Gets the text block widget for the node's ID. 
	* 
	* @param SpeechNode - UGraphNodeDialogueSpeech*, the owning speech node.
	* @return TSharedRef<SWidget> - the built widget. 
	*/
	TSharedRef<SWidget> GetNodeSubtitleWidget() const;

	/**
	* Creates the widget to display the speaker name and the node ID. 
	* 
	* @param SpeechNode - UGraphNodeDialogueSpeech*, the owning speech node.
	* @return TSharedRef<SWidget> - the built widget. 
	*/
	TSharedRef<SWidget> GetTitleTextBox() const;

private:
	/** Cached speech node */
	TObjectPtr<UGraphNodeDialogueSpeech> SpeechNode;

	/** Constants */
	const float TITLE_BOX_PADDING = 10.f;
	const FMargin ICON_PADDING = FMargin(20.f, 0.f, 10.f, 0.f);
	const FMargin SUBTITLE_PADDING = FMargin(0.f, 3.f, 0.f, 0.f);
	const float SUBTITLE_WRAP_DIFFERENCE = 60.f;
};