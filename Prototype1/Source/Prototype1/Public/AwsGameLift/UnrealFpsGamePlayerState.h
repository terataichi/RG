// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UnrealFpsGamePlayerState.generated.h"


/**
 * 
 */
UCLASS()
class PROTOTYPE1_API AUnrealFpsGamePlayerState : public APlayerState
{
	GENERATED_BODY()
public:

	AUnrealFpsGamePlayerState();

	void SetPlayerTeamType(const FString& teamName);

	void SetPlayerSessionId(const FString& playerSessionId);

	void SetMatchmakingPlayerId(const FString& matchmakingPlayerId);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const FString& GetTeamString()const;

	const FString& GetPlayerSessionId()const;

	bool CheckTeamName(const FString& teamName);

private:
	UPROPERTY()
		FString playerSessionId_;

	UPROPERTY()
		FString matchmakingPlayerId_;

	UPROPERTY(replicated)
		FString team_;
};
