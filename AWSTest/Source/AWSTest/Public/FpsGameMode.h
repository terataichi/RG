// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FpsGameMode.generated.h"


USTRUCT()
struct FStartGameSessionState
{
	GENERATED_BODY();

	UPROPERTY()
		bool status_;

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
		terminationTime_ = 0;
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

protected:
	virtual void BeginPlay()override;

private:
	void AWSGameInit();

	UPROPERTY()
		FStartGameSessionState startGameSessionState_;
	
	UPROPERTY()
		FUpdateGameSessionState updateGameSessionState_;

	UPROPERTY()
		FProcessTerminateState processTerminateState_;

	UPROPERTY()
		FHealthCheckState healthCheckState_;
};
