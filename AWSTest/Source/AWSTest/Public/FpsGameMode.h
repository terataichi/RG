// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/IHttpRequest.h"
#include "GameLiftServerSDK.h"
#include "Interfaces/IHttpRequest.h"
#include "FpsGameMode.generated.h"


class FHttpModule;
class AUnrealFpsGameGameState;

USTRUCT()
struct FStartGameSessionState
{
	GENERATED_BODY();

	UPROPERTY()
		bool status_;

	UPROPERTY()
		FString matchmakingConfigurationArn_;

	TMap<FString, Aws::GameLift::Server::Model::Player> playerIdToPlayer_;

	FStartGameSessionState() {
		status_ = false;
	}
};

USTRUCT()
struct FUpdateGameSessionState
{
	GENERATED_BODY();

	FUpdateGameSessionState(){

	}
};

USTRUCT()
struct FProcessTerminateState
{
	GENERATED_BODY();

	UPROPERTY()
		bool status_;

	long terminationTime_;

	FProcessTerminateState() {
		status_ = false;
		terminationTime_ = 0L;
	}
};

USTRUCT()
struct FHealthCheckState
{
	GENERATED_BODY();

	UPROPERTY()
		bool status_;

	FHealthCheckState() {
		status_ = false;
	}
};

UCLASS()
class AWSTEST_API AFpsGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AFpsGameMode();

	virtual void PreLogin(const FString& option, const FString& address, const FUniqueNetIdRepl& uniqueId, FString& errorMessage)override;

	virtual void Logout(AController* exiting) override;

	UPROPERTY()
		FTimerHandle countDownUntilGameOverHandle_;

	UPROPERTY()
		FTimerHandle endGameHandle_;

	UPROPERTY()
		FTimerHandle pickAWinningHandle_;

	UPROPERTY()
		FTimerHandle handleProcessTerminationHandle_;

	UPROPERTY()
		FTimerHandle handleGameSessionUpdateHandle_;

	AUnrealFpsGameGameState* GetGameState();

protected:
	virtual void BeginPlay()override;

	virtual FString InitNewPlayer(
		APlayerController* newPlayerController, const FUniqueNetIdRepl& uniqueId, const FString& options, const FString& portal
	)override;


private:
	FHttpModule* httpModule_;

	void AWSGameInit();

	UPROPERTY()
		FStartGameSessionState startGameSessionState_;
	
	UPROPERTY()
		FUpdateGameSessionState updateGameSessionState_;

	UPROPERTY()
		FProcessTerminateState processTerminateState_;

	UPROPERTY()
		FHealthCheckState healthCheckState_;

	UPROPERTY()
		FString apiUrl_;

	UPROPERTY()
		FString serverPassword_;

	UPROPERTY()
		int remainingGameTime_;

	UPROPERTY()
		bool gameSessionActivated_;

	UFUNCTION()
		void CountDownUntilGameOver();

	UFUNCTION()
		void EndGame();

	UFUNCTION()
		void PickAWinningTeam();

	UFUNCTION()
		void HandleProcessTermination();

	UFUNCTION()
		void HandleGameSessionUpdate();

	void OnRecordMatchResultResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr response, bool bWasSuccessful);
};
