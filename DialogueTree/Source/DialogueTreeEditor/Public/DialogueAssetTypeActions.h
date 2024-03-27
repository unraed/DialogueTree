// Copyright Zachary Brett, 2024. All rights reserved.

#pragma once

//UE
#include "AssetTypeActions_Base.h"
#include "CoreMinimal.h"

/**
* Asset actions for the dialogue. Essentially defines registration
* details within the editor for the asset type. 
*/
class DIALOGUETREEEDITOR_API FDialogueAssetTypeActions : 
	public FAssetTypeActions_Base
{
public: 
	/** Constructor */
	FDialogueAssetTypeActions(EAssetTypeCategories::Type InCategory);

private:
	/** The dialogue asset's category */
	EAssetTypeCategories::Type AssetCategory;

public:
	/** FAssetTypeActions_Base Impl. */
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, 
		TSharedPtr<class IToolkitHost> EditWithinLevelEditor 
		= TSharedPtr<IToolkitHost>()) override;
	/** End FAssetTypeActions_Base */
};