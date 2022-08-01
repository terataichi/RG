// Fill out your copyright notice in the Description page of Project Settings.


#include "AwsGameLift/UnrealFpsGamePlayerState.h"
#include "Net/UnrealNetwork.h"


AUnrealFpsGamePlayerState::AUnrealFpsGamePlayerState()
{
	playerState_ = PLAYER_STATE::NON;
	team_ = "NON";
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

const PLAYER_STATE& AUnrealFpsGamePlayerState::GetPlayerState() const
{
	return playerState_;
}

const FString& AUnrealFpsGamePlayerState::GetPlayerSessionId() const
{
	return playerSessionId_;
}

void AUnrealFpsGamePlayerState::SetPlayerState(const PLAYER_STATE& playerstate)
{
	playerState_ = playerstate;
}

bool AUnrealFpsGamePlayerState::CheckTeamName(const FString& teamName)
{
	return team_.Equals(teamName);
}


