// Copyright Epic Games, Inc. All Rights Reserved.

#include "AWSTestGameMode.h"
#include "AWSTest.h"
#include "Engine.h"
#include "EngineGlobals.h"
#include "AWSTestHUD.h"
#include "AWSTestCharacter.h"
#include "GameLiftServerSDK.h"
#include "UObject/ConstructorHelpers.h"

AAWSTestGameMode::AAWSTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AAWSTestHUD::StaticClass();
}

void AAWSTestGameMode::GameLiftInit()
{
	// �Ƃ肠����Amazon��Sample�p�N��܂��B
	// ServerTarget����GameLift���L���ȏꍇ�̂ݗL��
#if WITH_GAMELIFT
	
#endif
}
