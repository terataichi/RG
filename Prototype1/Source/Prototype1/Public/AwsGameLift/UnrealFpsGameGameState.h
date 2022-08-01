// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "UnrealFpsGameGameState.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE1_API AUnrealFpsGameGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AUnrealFpsGameGameState();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetLatestEvent(const FString& latestEvent);
	void SetWinningTeam(const FString& winningTeam);

	const FString& GetLatesEvent()const;
	const FString& GetWinningTeam()const;


	UFUNCTION(BlueprintCallable)
		void SetWinTeam(const FString& winningTeam);

private:
	UPROPERTY(replicated)
		FString latestEvent_;

	UPROPERTY(replicated)
		FString winningTeam_;
};
