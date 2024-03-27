// Copyright Zachary Brett, 2024. All rights reserved.

//Header
#include "DialogueAssetFactory.h"
//Plugin
#include "Dialogue.h"

UDialogueAssetFactory::UDialogueAssetFactory()
{
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UDialogue::StaticClass();
}

UObject* UDialogueAssetFactory::FactoryCreateNew(UClass* Class, 
    UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context,
    FFeedbackContext* Warn)
{
    check(Class->IsChildOf(SupportedClass));

    return NewObject<UDialogue>(InParent, Name, Flags | RF_Transactional);
}
