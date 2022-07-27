// Fill out your copyright notice in the Description page of Project Settings.


#include "AwsGameLift/MainMenuGameMode.h"
#include "AwsGameLift/MainMenuHUD.h"

AMainMenuGameMode::AMainMenuGameMode()
	: Super()
{
	HUDClass = AMainMenuHUD::StaticClass();
}