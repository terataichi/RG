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

void AUnrealFpsGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUnrealFpsGamePlayerState, team_);
}

const FString& AUnrealFpsGamePlayerState::GetTeamString() const
{
	return team_;
}

bool AUnrealFpsGamePlayerState::CheckTeamName(const FString& teamName)
{
	return team_.Equals(teamName);
}

