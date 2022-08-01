// Fill out your copyright notice in the Description page of Project Settings.


#include "AwsGameLift/UnrealFpsGameWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"

#include "AwsGameLift/UnrealFpsGamePlayerState.h"
#include "AwsGameLift/UnrealFpsGameGameState.h"
#include "AwsGameLift/UnrealFpsGameInstance.h"
#include "Kismet/GameplayStatics.h"

#include "AwsGameLift/LatencyRecorder.h"


void UUnrealFpsGameWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();
	InitTextBlocks();
	InitTimers();
}

void UUnrealFpsGameWidget::NativeDestruct()
{
	ClearTimers();
	UUserWidget::NativeDestruct();
}


void UUnrealFpsGameWidget::SetLatesEvent()
{
	AGameStateBase* gameState = GetWorld()->GetGameState();

	if (gameState == nullptr)
	{
		check(!"gameState‚ªnullptr");
		return;
	}

	AUnrealFpsGameGameState* fpsGameState = Cast<AUnrealFpsGameGameState>(gameState);

	if (fpsGameState == nullptr)
	{
		check(!"fpsGameState‚ªnullptr");
		return;
	}
	const FString& latesEvent = fpsGameState->GetLatesEvent();
	const FString& winningTeam = fpsGameState->GetWinningTeam();

	if (latesEvent.Len() <= 0 || winningTeam.Len() <= 0)
	{
		return;
	}

	if (latesEvent.Equals("GameEnded"))
	{
		FString owningPlayerTeam = GetOwingPlayerTeamName();
		FString gameOverMessage = "You and the " + owningPlayerTeam;
		FString mes;
		if (owningPlayerTeam.Equals(winningTeam))
		{
			mes = " won!";
		}
		else
		{
			mes = " lost";
		}
		eventTextBlock_->SetText(FText::FromString(gameOverMessage + mes));
	}
	else
	{
		eventTextBlock_->SetText(FText::FromString(latesEvent));
	}
	

}


void UUnrealFpsGameWidget::InitTextBlocks()
{
	eventTextBlock_ = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_Event")));
}

void UUnrealFpsGameWidget::InitTimers()
{
	auto& timerManager = GetWorld()->GetTimerManager();
	timerManager.SetTimer(latesEventHandle_, this, &UUnrealFpsGameWidget::SetLatesEvent, 1.0, true, 1.0f);
}

void UUnrealFpsGameWidget::ClearTimers()
{
	auto& timerManager = GetWorld()->GetTimerManager();

	timerManager.ClearTimer(latesEventHandle_);
}

FString UUnrealFpsGameWidget::GetOwingPlayerTeamName()
{
	APlayerState* owningPlayerState = GetOwningPlayerState();

	if (owningPlayerState == nullptr)
	{
		check(!"owningPlayerState‚ªnullptr");
		return FString();
	}

	AUnrealFpsGamePlayerState* owningFpsGameState = Cast<AUnrealFpsGamePlayerState>(owningPlayerState);

	if (owningFpsGameState == nullptr)
	{
		check(!"owningFpsGameState‚ªnullptr");
		return FString();
	}
	FString owningPlayerTeam = owningFpsGameState->GetTeamString();
	if (owningPlayerTeam.Len() <= 0)
	{
		check(!"owningPlayerTeam‚Ì’†g‚ª‚È‚¢");
		return FString();
	}
	return owningPlayerTeam;
}
