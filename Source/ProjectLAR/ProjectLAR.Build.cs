// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ProjectLAR : ModuleRules
{
	public ProjectLAR(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"UMG",
			"Slate",
			"SlateCore",
			"Niagara",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "Niagara" });

		PublicIncludePaths.AddRange(new string[]
		{
			System.IO.Path.Combine(ModuleDirectory, "Boss/Public"),
			System.IO.Path.Combine(ModuleDirectory, "Player/Public"),
			System.IO.Path.Combine(ModuleDirectory, "Controller/Public"),
			System.IO.Path.Combine(ModuleDirectory, "GameMode/Public"),
			System.IO.Path.Combine(ModuleDirectory, "Skill/Public"),
			System.IO.Path.Combine(ModuleDirectory, "UI/Public")
			
		});
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
