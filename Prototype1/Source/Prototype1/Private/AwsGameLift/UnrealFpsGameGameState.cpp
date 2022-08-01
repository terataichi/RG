// Fill out your copyright notice in the Description page of Project Settings.


#include "AwsGameLift/UnrealFpsGameGameState.h"
#include "Net/UnrealNetwork.h"


AUnrealFpsGameGameState::AUnrealFpsGameGameState()
{

}

void AUnrealFpsGameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUnrealFpsGameGameState, latestEvent_);
	DOREPLIFETIME(AUnrealFpsGameGameState, winningTeam_);
}

void AUnrealFpsGameGameState::SetLatestEvent(const FString& latestEvent)
{
	latestEvent_ = latestEvent;
}

void AUnrealFpsGameGameState::SetWinningTeam(const FString& winningTeam)
{
	winningTeam_ = winningTeam;
}

const FString& AUnrealFpsGameGameState::GetLatesEvent() const
{
	return latestEvent_;
}

const FString& AUnrealFpsGameGameState::GetWinningTeam() const
{
	return winningTeam_;
}

void AUnrealFpsGameGameState::SetWinTeam(const FString& winningTeam)
{
	SetLatestEvent(TEXT("GameEnded"));
	SetWinningTeam(winningTeam);
}
