// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuHUD.h"
#include "BluePrint/UserWidget.h"

AMainMenuHUD::AMainMenuHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> mainMenuObj(TEXT("/Game/MyContent/wedget/WB_MainMenu.WB_MainMenu"));
	mainMenuWidget_ = mainMenuObj.Class;
}

void AMainMenuHUD::BeginPlay() {
	Super::BeginPlay();

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (playerController == nullptr)
	{
		ensure(!"playerController��nullptr");
	}
	// �}�E�X�J�[�\����\������悤�ɐݒ�
	playerController->bShowMouseCursor = true;

	if (mainMenuWidget_ == nullptr)
	{
		ensure(!"mainMenuWidget_��nullptr");
	}
	UUserWidget* mainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), mainMenuWidget_);
	if (mainMenuWidget == nullptr)
	{
		ensure(!"mainMenuWidget�������s");
	}

	mainMenuWidget->AddToViewport();
	mainMenuWidget->SetFocus();

}
