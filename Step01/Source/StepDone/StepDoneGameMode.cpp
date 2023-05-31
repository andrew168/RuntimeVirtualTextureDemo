// Copyright Epic Games, Inc. All Rights Reserved.

#include "StepDoneGameMode.h"
#include "StepDoneHUD.h"
#include "StepDoneCharacter.h"
#include "UObject/ConstructorHelpers.h"

AStepDoneGameMode::AStepDoneGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AStepDoneHUD::StaticClass();
}
