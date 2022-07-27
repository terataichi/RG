// Fill out your copyright notice in the Description page of Project Settings.


#include "AwsGameLift/UnrealFpsGameHUD.h"
#include "Blueprint/UserWidget.h"

AUnrealFpsGameHUD::AUnrealFpsGameHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> GameObj(TEXT("/Game/MyContent/wedget/WB_OnGame"));
	gameWidgetClass_ = GameObj.Class;
}

void AUnrealFpsGameHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();

	if (playerController == nullptr)
	{
		check(!"playerController‚ªnullptr");
		return;
	}

	UUserWidget* gameWidget = CreateWidget<UUserWidget>(GetWorld(), gameWidgetClass_);

	if (gameWidget == nullptr)
	{
		check(!"gameWidget‚ªnullptr");
		return;
	}

	gameWidget->AddToViewport();
}
