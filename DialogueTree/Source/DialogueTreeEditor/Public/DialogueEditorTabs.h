// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once 

//UE
#include "CoreMinimal.h"

/**
* App name for the plugin. 
*/
const FName AppName = FName(TEXT("DialogueTreeEditor"));

/**
* Struct for tab identifiers. 
*/
struct FDialogueEditorTabs
{
    /** The main graph/viewport ID */
    static const FName ViewportTabID;

    /** The selected node details panel ID */
    static const FName NodeDetailsTabID; 

    /** The graph's properties panel ID */
    static const FName GraphPropertiesTabID; 
};

/**
* Define tab identifiers. 
*/
const FName FDialogueEditorTabs::ViewportTabID = 
    FName(TEXT("DialogueEditorViewportTab"));
const FName FDialogueEditorTabs::NodeDetailsTabID = 
    FName(TEXT("DialogueEditorDetailsTab"));
const FName FDialogueEditorTabs::GraphPropertiesTabID = 
    FName(TEXT("DialogueEditorPropertiesTab"));