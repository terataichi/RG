// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UnrealFpsGamePlayerState.generated.h"


/**
 * 
 */
UCLASS()
class AWSTEST_API AUnrealFpsGamePlayerState : public APlayerState
{
	GENERATED_BODY()
public:

	AUnrealFpsGamePlayerState();

	void SetPlayerTeamType(const FString& teamName);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const FString& GetTeamString()const;

	bool CheckTeamName(const FString& teamName);

private:
	UPROPERTY()
		FString playerSessionId_;

	UPROPERTY()
		FString matchMakingPlayerId_;

	UPROPERTY(replicated)
		FString team_;
};
