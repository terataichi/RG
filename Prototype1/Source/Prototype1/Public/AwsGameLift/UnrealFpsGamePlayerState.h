// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UnrealFpsGamePlayerState.generated.h"


UENUM(BlueprintType)
enum class PLAYER_STATE : uint8
{
	NON,
	PREPARATION,
	BATTLE
};

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

	const FString& GetPlayerSessionId()const;

	UFUNCTION(BlueprintCallable)
	const FString& GetTeamString()const;

	UFUNCTION(BlueprintCallable)
		const PLAYER_STATE& GetPlayerState()const;

	UFUNCTION(BlueprintCallable)
		void SetPlayerState(const PLAYER_STATE& playerstate);

	bool CheckTeamName(const FString& teamName);

private:
	UPROPERTY()
		FString playerSessionId_;

	UPROPERTY()
		FString matchmakingPlayerId_;

	UPROPERTY(replicated)
		FString team_;

	UPROPERTY()
		PLAYER_STATE playerState_;
};
