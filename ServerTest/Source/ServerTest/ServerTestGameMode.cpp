// Copyright Epic Games, Inc. All Rights Reserved.

#include "ServerTestGameMode.h"
#include "ServerTestHUD.h"
#include "ServerTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AServerTestGameMode::AServerTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AServerTestHUD::StaticClass();
}
