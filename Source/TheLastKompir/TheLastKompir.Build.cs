// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheLastKompir : ModuleRules
{
	public TheLastKompir(TargetInfo Target)
	{
PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "GameplayTasks", "UMG" });

       PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore"});
    }
}
