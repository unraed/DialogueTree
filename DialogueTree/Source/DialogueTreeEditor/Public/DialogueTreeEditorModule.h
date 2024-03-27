// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "AssetTypeCategories.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FDialogueTreeNodeFactory;
class IAssetTypeActions;

/**
* Loads and unloads the dialogue system's editor module. 
*/
class FDialogueTreeEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface Impl. */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	/** End IModuleInterface */

private:
	/**
	* Registers the node factory on startup. 
	*/
	void RegisterNodeFactory();

	/**
	* Registers the asset category and asset type actions on
	* startup.
	*/
	void RegisterAssets();

	/**
	* Registers details panel customizations for structs and 
	* classes on startup.
	*/
	void RegisterDetailsCustomizers();

	/**
	* Unregisters the node factory on shutdown. 
	*/
	void UnregisterNodeFactory();

	/**
	* Unregisters the asset category and asset type actions on 
	* shutdown.
	*/
	void UnregisterAssets();

	/**
	* Unregisters any customizers for the details panel on shutdown.
	*/
	void UnregisterDetailsCustomizers();

private:
	/** Asset category under which to situate the dialogue asset */
	EAssetTypeCategories::Type DialogueAssetCategory;

	/** Asset type actions to unregister when shutting down */
	TArray<TSharedPtr<IAssetTypeActions>> AssetTypeActionsList;

	/** The factory for dialogue graph nodes */
	TSharedPtr<FDialogueTreeNodeFactory> NodeFactory;
};
