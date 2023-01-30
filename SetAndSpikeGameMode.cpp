// Copyright Epic Games, Inc. All Rights Reserved.

#include "SetAndSpikeGameMode.h"
#include "SetAndSpikeCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASetAndSpikeGameMode::ASetAndSpikeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
