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

void UUnrealFpsGameWidget::SetTeammateCount()
{
	FString owningPlayerTeam = GetOwingPlayerTeamName();
	teamNameTextBlock_->SetText(FText::FromString("Team Name: " + owningPlayerTeam));

	TArray<APlayerState*> playerStates = GetWorld()->GetGameState()->PlayerArray;

	int teammateCount = 0;

	for (const auto& playerState: playerStates)
	{
		if (playerState == nullptr)
		{
			continue;
		}
		AUnrealFpsGamePlayerState* fpsGameState = Cast<AUnrealFpsGamePlayerState>(playerState);
		if (fpsGameState == nullptr)
		{
			continue;
		}
		if (fpsGameState->CheckTeamName(owningPlayerTeam))
		{
			teammateCount++;
		}
	}

	teammateCountTextBlock_->SetText(FText::FromString("Teammate Count: " + FString::FromInt(teammateCount)));



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

void UUnrealFpsGameWidget::SetAveragePlayerLatency()
{
	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance == nullptr)
	{
		check(!"gameInstance‚ªnullptr");
		return;
	}

	UUnrealFpsGameInstance* fpsGameInstance = Cast<UUnrealFpsGameInstance>(gameInstance);

	if (fpsGameInstance == nullptr)
	{
		check(!"fpsGameInstance‚ªnullptr");
		return;
	}

	float playerLatency = LatencyRecorder::GetAveragePlayerLatency(fpsGameInstance);
	FString pingString = "Ping: " + FString::FromInt(FMath::RoundToInt(playerLatency)) + "ms";
	pingTextBlock_->SetText(FText::FromString(pingString));
}

void UUnrealFpsGameWidget::InitTextBlocks()
{
	teamNameTextBlock_ = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_TeamName")));
	teammateCountTextBlock_ = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_TeammateCount")));
	eventTextBlock_ = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_Event")));
	pingTextBlock_ = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_Ping")));
}

void UUnrealFpsGameWidget::InitTimers()
{
	auto& timerManager = GetWorld()->GetTimerManager();

	timerManager.SetTimer(teammateCountHandle_, this, &UUnrealFpsGameWidget::SetTeammateCount, 1.0f, true, 1.0f);
	timerManager.SetTimer(latesEventHandle_, this, &UUnrealFpsGameWidget::SetLatesEvent, 1.0, true, 1.0f);
	timerManager.SetTimer(averagePlayerLatencyHandle_, this, &UUnrealFpsGameWidget::SetAveragePlayerLatency, 1.0f, true, 1.0f);
}

void UUnrealFpsGameWidget::ClearTimers()
{
	auto& timerManager = GetWorld()->GetTimerManager();

	timerManager.ClearTimer(teammateCountHandle_);
	timerManager.ClearTimer(latesEventHandle_);
	timerManager.ClearTimer(averagePlayerLatencyHandle_);
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
