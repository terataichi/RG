// Fill out your copyright notice in the Description page of Project Settings.


#include "OfflineGameMode.h"
#include "../AWSTestCharacter.h"
#include "UObject/ConstructorHelpers.h"


AOfflineGameMode::AOfflineGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}
