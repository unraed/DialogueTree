// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "DialogueTreeEditorModule.h"
//UE
#include "AssetToolsModule.h"
#include "IAssetTypeActions.h"
//Plugin
#include "CustomDetails/DialogueGraphCustomization.h"
#include "CustomDetails/DialogueGraphConditionCustomization.h"
#include "CustomDetails/DialogueNodeSocketCustomization.h"
#include "CustomDetails/DialogueSpeakerSocketCustomization.h"
#include "CustomDetails/GraphDialogueEventCustomization.h"
#include "DialogueAssetTypeActions.h"
#include "DialogueNodeSocket.h"
#include "DialogueSpeakerSocket.h"
#include "DialogueTreeStyle.h"
#include "Graph/DialogueEdGraph.h"
#include "Graph/DialogueGraphCondition.h"
#include "Graph/DialogueTreeNodeFactory.h"
#include "Graph/Nodes/GraphNodeDialogueBranch.h"
#include "Graph/Nodes/GraphNodeDialogueEvent.h"
#include "Graph/Nodes/GraphNodeDialogueSpeech.h"
#include "Graph/PickableDialogueNode.h"
#include "Graph/PickableDialogueSpeaker.h"

#define LOCTEXT_NAMESPACE "FDialogueTreeEditorModule"

void FDialogueTreeEditorModule::StartupModule()
{
	RegisterNodeFactory();
	RegisterAssets();
	RegisterDetailsCustomizers();

	//Register Style Set
	FDialogueTreeStyle::Initialize();
}

void FDialogueTreeEditorModule::ShutdownModule()
{
	UnregisterNodeFactory();
	UnregisterAssets();
	UnregisterDetailsCustomizers();

	// Unregister Style Set
	FDialogueTreeStyle::Shutdown();
}

void FDialogueTreeEditorModule::RegisterNodeFactory()
{
	NodeFactory = MakeShareable(new FDialogueTreeNodeFactory());
	FEdGraphUtilities::RegisterVisualNodeFactory(NodeFactory);
}

void FDialogueTreeEditorModule::RegisterAssets()
{
	//Get module
	IAssetTools& AssetToolsModule =
		FModuleManager::LoadModuleChecked<FAssetToolsModule>(
			"AssetTools"
		).Get();

	//Register category
	DialogueAssetCategory = AssetToolsModule.RegisterAdvancedAssetCategory
	(
		FName(TEXT("DialogueAssets")),
		LOCTEXT("DialogueAssetCategory", "Dialogue Tree")
	);

	//Create and register asset type actions 
	TSharedPtr<IAssetTypeActions> DialogueActions = MakeShareable(
		new FDialogueAssetTypeActions(DialogueAssetCategory)
	);

	AssetToolsModule.RegisterAssetTypeActions(
		DialogueActions.ToSharedRef()
	);
	AssetTypeActionsList.Add(DialogueActions);
}

void FDialogueTreeEditorModule::RegisterDetailsCustomizers()
{
	//Get module
	FPropertyEditorModule& PropertyModule =
		FModuleManager::GetModuleChecked<FPropertyEditorModule>(
			"PropertyEditor"
		);

	//Graph
	PropertyModule.RegisterCustomClassLayout(
		UDialogueEdGraph::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(
			&FDialogueGraphCustomization::MakeInstance
		)
	);

	//Graph condition 
	PropertyModule.RegisterCustomPropertyTypeLayout(
		UDialogueGraphCondition::StaticClass()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(
			&FDialogueGraphConditionCustomization::MakeInstance
		)
	);

	//Graph event
	PropertyModule.RegisterCustomPropertyTypeLayout(
		FGraphDialogueEvent::StaticStruct()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(
			&FGraphDialogueEventCustomization::MakeInstance
		)
	);

	//Node socket 
	PropertyModule.RegisterCustomPropertyTypeLayout(
		UDialogueNodeSocket::StaticClass()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(
			&FDialogueNodeSocketCustomization::MakeInstance
		)
	);

	//Speaker socket
	PropertyModule.RegisterCustomPropertyTypeLayout(
		UDialogueSpeakerSocket::StaticClass()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(
			&FDialogueSpeakerSocketCustomization::MakeInstance
		)
	);
}

void FDialogueTreeEditorModule::UnregisterNodeFactory()
{
	if (NodeFactory.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(NodeFactory);
		NodeFactory.Reset();
	}
}

void FDialogueTreeEditorModule::UnregisterAssets()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		//Get module 
		IAssetTools& AssetToolsModule =
			FModuleManager::GetModuleChecked<FAssetToolsModule>(
				"AssetTools"
			).Get();

		//Unregister all asset type actions in list 
		for (int32 i = 0; i < AssetTypeActionsList.Num(); ++i)
		{
			AssetToolsModule.UnregisterAssetTypeActions(
				AssetTypeActionsList[i].ToSharedRef()
			);
		}

		//Clear the array of actions 
		AssetTypeActionsList.Empty();
	}
}

void FDialogueTreeEditorModule::UnregisterDetailsCustomizers()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		//Get module
		FPropertyEditorModule& PropertyModule =
			FModuleManager::GetModuleChecked<FPropertyEditorModule>(
				"PropertyEditor"
			);

		//Graph 
		PropertyModule.UnregisterCustomClassLayout(
			UDialogueEdGraph::StaticClass()->GetFName()
		);

		//Graph condition 
		PropertyModule.UnregisterCustomPropertyTypeLayout(
			UDialogueGraphCondition::StaticClass()->GetFName()
		);

		//Graph event
		PropertyModule.UnregisterCustomPropertyTypeLayout(
			FGraphDialogueEvent::StaticStruct()->GetFName()
		);

		//Node socket 
		PropertyModule.UnregisterCustomPropertyTypeLayout(
			UDialogueNodeSocket::StaticClass()->GetFName()
		);

		//Speaker socket
		PropertyModule.UnregisterCustomPropertyTypeLayout(
			UDialogueSpeakerSocket::StaticClass()->GetFName()
		);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogueTreeEditorModule, DialogueTreeEditor)