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
      "AIModule", 
      "EnhancedInput"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "Niagara", "EnhancedInput", "AITestSuite" });
    
    // 팀원 ROLE에 따라 나뉜 파트를 한번에 빌드할 때 사용
		// System.IO.Path.Combine(ModuleDirectory, "[ROLE]/Public") 형식으로 입력
    
		PublicIncludePaths.AddRange(new string[]
		{
			System.IO.Path.Combine(ModuleDirectory, "Boss/Public"),
			System.IO.Path.Combine(ModuleDirectory, "Player/Public"),
			System.IO.Path.Combine(ModuleDirectory, "Controller/Public"),
			System.IO.Path.Combine(ModuleDirectory, "GameMode/Public"),
			System.IO.Path.Combine(ModuleDirectory, "Item/Public"),
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
