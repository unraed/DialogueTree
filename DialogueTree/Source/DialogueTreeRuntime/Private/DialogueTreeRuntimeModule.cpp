// Copyright Zachary Brett, 2024. All rights reserved.

#include "DialogueTreeRuntimeModule.h"

#define LOCTEXT_NAMESPACE "FDialogueTreeRuntimeModule"

void FDialogueTreeRuntimeModule::StartupModule()
{
	//Currently empty. 
}

void FDialogueTreeRuntimeModule::ShutdownModule()
{
	//Currently empty. 
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogueTreeRuntimeModule, DialogueTreeRuntime)