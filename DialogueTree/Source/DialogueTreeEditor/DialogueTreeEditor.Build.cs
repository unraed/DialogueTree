// Copyright Zachary Brett, 2024. All rights reserved.

using UnrealBuildTool;

public class DialogueTreeEditor : ModuleRules
{
	public DialogueTreeEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				"DialogueTreeEditor/Private",
				"DialogueTreeEditor/Public"
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
				"EditorFramework",
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"DialogueTreeRuntime",
                "Slate",
                "SlateCore",
                "AssetTools",
				"InputCore", 
				"GraphEditor",
				"PropertyEditor",
				"EditorStyle",
				"EditorWidgets",
				"Kismet",
				"KismetWidgets",
				"ApplicationCore",
				"ToolMenus",
				"GameplayTags",
				"Projects"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
