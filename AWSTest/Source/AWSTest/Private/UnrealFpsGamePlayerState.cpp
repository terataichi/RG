// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealFpsGamePlayerState.h"
#include "Net/UnrealNetwork.h"


AUnrealFpsGamePlayerState::AUnrealFpsGamePlayerState()
{

}

void AUnrealFpsGamePlayerState::SetPlayerTeamType(const FString& teamName)
{
	team_ = teamName;
}

void AUnrealFpsGamePlayerState::SetPlayerSessionId(const FString& playerSessionId)
{
	playerSessionId_ = playerSessionId;
}

void AUnrealFpsGamePlayerState::SetMatchmakingPlayerId(const FString& matchmakingPlayerId)
{
	matchmakingPlayerId_ = matchmakingPlayerId;
}

void AUnrealFpsGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUnrealFpsGamePlayerState, team_);
}

const FString& AUnrealFpsGamePlayerState::GetTeamString() const
{
	return team_;
}

const FString& AUnrealFpsGamePlayerState::GetPlayerSessionId() const
{
	return playerSessionId_;
}

bool AUnrealFpsGamePlayerState::CheckTeamName(const FString& teamName)
{
	return team_.Equals(teamName);
}


