// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThreadOfFateGameMode.h"
#include "ThreadOfFateCharacter.h"
#include "UObject/ConstructorHelpers.h"

AThreadOfFateGameMode::AThreadOfFateGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
