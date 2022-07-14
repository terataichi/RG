// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealFpsGameWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "UnrealFpsGamePlayerState.h"
#include "UnrealFpsGameGameState.h"
#include "UnrealFpsGameInstance.h"
#include "Kismet/GameplayStatics.h"


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
	APlayerState* owningPlayerState = GetOwningPlayerState();

	if (owningPlayerState == nullptr)
	{
		check(!"owningPlayerState‚ªnullptr");
		return;
	}

	AUnrealFpsGamePlayerState* owningFpsGameState = Cast<AUnrealFpsGamePlayerState>(owningPlayerState);

	if (owningFpsGameState == nullptr)
	{
		check(!"owningFpsGameState‚ªnullptr");
		return;
	}
	FString owningPlayerTeam = owningFpsGameState->GetTeamString();
	if (owningPlayerTeam.Len() <= 0)
	{
		check(!"owningPlayerTeam‚Ì’†g‚ª‚È‚¢");
		return;
	}
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
}

void UUnrealFpsGameWidget::SetAveragePlayerLatency()
{
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
