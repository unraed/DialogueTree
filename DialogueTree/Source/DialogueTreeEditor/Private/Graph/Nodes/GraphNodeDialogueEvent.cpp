// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "Graph/Nodes/GraphNodeDialogueEvent.h"
//Plugin
#include "Dialogue.h"
#include "DialogueNodeSocket.h"
#include "Events/DialogueEventBase.h"
#include "Events/ResetNodeVisits.h"
#include "Graph/DialogueEdGraph.h"
#include "Nodes/DialogueEventNode.h"

#define LOCTEXT_NAMESPACE "GraphNodeDialogueEvent"

UGraphNodeDialogueEvent* UGraphNodeDialogueEvent::MakeTemplate(UObject* Outer)
{
	return NewObject<UGraphNodeDialogueEvent>(Outer);
}

void UGraphNodeDialogueEvent::PostEditChangeProperty(
	FPropertyChangedEvent& PropertyChangedEvent)
{
	UpdateDialogueNode();
}

FText UGraphNodeDialogueEvent::GetTooltipText() const
{
	return LOCTEXT("NodeTooltip", "Plays a sequence of dialogue events");
}

void UGraphNodeDialogueEvent::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FName());
	CreatePin(EGPD_Output, "MultipleNodes", FName());
}

void UGraphNodeDialogueEvent::PostPasteNode()
{
	Super::PostPasteNode();

	//Verify that there is a dialogue to attach events to 
	check(GetDialogueGraph());
	UDialogue* DialogueAsset = GetDialogueGraph()->GetDialogue();
	check(DialogueAsset);

	//Copy events so they are not shared with copied node
	TArray<UDialogueEventBase*> CopiedEvents;
	for (FGraphDialogueEvent GraphEvent : Events)
	{
		if (GraphEvent.Event)
		{
			UDialogueEventBase* CopiedEvent = 
				DuplicateObject(GraphEvent.Event, DialogueAsset);
			if (CopiedEvent)
			{
				CopiedEvents.Add(CopiedEvent);
			}
		}
	}

	//Replace old node's events with copied ones
	Events.Empty();
	for (UDialogueEventBase* Event : CopiedEvents)
	{
		FGraphDialogueEvent NewGraphEvent;
		NewGraphEvent.Event = Event;
		Events.Add(NewGraphEvent);
	}
}

void UGraphNodeDialogueEvent::CreateAssetNode(UDialogue* InAsset)
{
	//Create asset node 
	UDialogueEventNode* NewNode = 
		NewObject<UDialogueEventNode>(InAsset);
	check(NewNode);

	//Store the completed node as the asset node 
	SetAssetNode(NewNode);

	//Add to the target dialogue. 
	InAsset->AddNode(NewNode);
}

void UGraphNodeDialogueEvent::FinalizeAssetNode()
{
	check(GetAssetNode());
	UDialogueEventNode* TargetNode = 
		CastChecked<UDialogueEventNode>(GetAssetNode());

	//Get the dialogue 
	check(GetDialogueGraph());
	UDialogue* TargetDialogue = GetDialogueGraph()->GetDialogue();
	check(TargetDialogue);

	//Retrieve events 
	TArray<UDialogueEventBase*> FinalEvents;
	for (const FGraphDialogueEvent& Event : Events)
	{
		if (Event.Event)
		{
			//Change outer 
			Event.Event->Rename(nullptr, TargetDialogue);

			Event.Event->SetDialogue(TargetDialogue);
			FinalEvents.Add(Event.Event);

			//If the event is of the right type, handle the node socket 
			if (Event.Event->IsA<UResetNodeVisits>())
			{
				FinalizeNodeSocket(Event.Event);
			}
		}
	}

	TargetNode->SetEvents(FinalEvents);
}

bool UGraphNodeDialogueEvent::CanCompileNode()
{
	for (const FGraphDialogueEvent& Event : Events)
	{
		if (!Event.Event 
			|| !Event.Event->HasAllRequirements())
		{
			SetErrorFlag(true);
			return false;
		}
	}

	SetErrorFlag(false);
	return true;
}

FName UGraphNodeDialogueEvent::GetBaseID() const
{
	return FName("Event");
}

TArray<FText> UGraphNodeDialogueEvent::GetGraphDescriptions() const
{
	TArray<FText> EventTexts;

	for (const FGraphDialogueEvent& Event : Events)
	{
		if (Event.Event == nullptr)
		{
			continue;
		}

		if (Event.Event->HasAllRequirements())
		{
			EventTexts.Add(Event.Event->GetGraphDescription());
		}
		else
		{
			EventTexts.Add(LOCTEXT("InvalidEventText", "Invalid Event"));
		}
	}

	return EventTexts;
}

void UGraphNodeDialogueEvent::FinalizeNodeSocket(UDialogueEventBase* InEvent)
{
	if (!InEvent)
	{
		return;
	}

	UResetNodeVisits* VisitsEvent = Cast<UResetNodeVisits>(InEvent);
	
	if (!VisitsEvent)
	{
		return;
	}

	UDialogueNodeSocket* TargetSocket = VisitsEvent->GetTargetSocket();
	check(TargetSocket);
	check(TargetSocket->GetGraphNode());

	UGraphNodeDialogue* GraphNode = 
		Cast<UGraphNodeDialogue>(TargetSocket->GetGraphNode());
	check(GraphNode);
	check(GraphNode->GetAssetNode());

	TargetSocket->SetDialogueNode(GraphNode->GetAssetNode());
}

#undef LOCTEXT_NAMESPACE
