// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ThreadOfFate : ModuleRules
{
	public ThreadOfFate(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
