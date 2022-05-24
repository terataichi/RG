// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyFps/MyFpsGameModeBase.h"

void AMyFpsGameModeBase::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World,this is MyFps"));
}