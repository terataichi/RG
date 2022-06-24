// Fill out your copyright notice in the Description page of Project Settings.


#include "FpsGameMode.h"
#include "../AWSTestCharacter.h"
#include "GameLiftServerSDK.h"
#include "UObject/ConstructorHelpers.h"

AFpsGameMode::AFpsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}

void AFpsGameMode::BeginPlay()
{
	Super::BeginPlay();
	AWSGameInit();

}

void AFpsGameMode::AWSGameInit()
{
#if WITH_GAMELIFT
	auto initSDKoutcome = Aws::GameLift::Server::InitSDK();

	if (!initSDKoutcome.IsSuccess())
	{
		check(!"AWSInité∏îs");
	}
	auto OnStartGameSession = [](Aws::GameLift::Server::Model::GameSession gameSessionObj, void* params) {
		FStartGameSessionState* state = static_cast<FStartGameSessionState*>(params);
		state->status_ = Aws::GameLift::Server::ActivateGameSession().IsSuccess();
	};

	auto OnUpdateGameSession = [](Aws::GameLift::Server::Model::UpdateGameSession updateGameSessionObj, void* params)
	{
		FUpdateGameSessionState* state = static_cast<FUpdateGameSessionState*>(params);
	};

	auto OnProcessTerminate = [](void* params) {
		FProcessTerminateState* state = static_cast<FProcessTerminateState*>(params);
		auto getTerminationTimeOutcome = Aws::GameLift::Server::GetTerminationTime();
		if (!getTerminationTimeOutcome.IsSuccess())
		{
			check(!"getTerminationTimeOutcome:é∏îs");
		}
		state->terminationTime_ = getTerminationTimeOutcome.GetResult();
		auto ProcessEndingOutcome = Aws::GameLift::Server::ProcessEnding();

		if (!ProcessEndingOutcome.IsSuccess())
		{
			check(!"ProcessEndingOutcome:é∏îs");
		}
		state->status_ = true;
		FGenericPlatformMisc::RequestExit(false);
	};

	auto OnHealthCheck = [](void* params) {
		FHealthCheckState* state = static_cast<FHealthCheckState*>(params);
		state->status_ = true;

		return state->status_;
	};

	TArray<FString> CommandLineTokens = {};
	TArray<FString> CommandLineSwitches = {};
	int port = FURL::UrlConfig.DefaultPort;


	FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

	for (FString str:CommandLineTokens)
	{
		FString key = {};
		FString value = {};

		if (str.Split("=",&key,&value))
		{
			if (key.Equals("port"))
			{
				port = FCString::Atoi(*value);
			}
		}
	}

	const char* logFile = "aLogFile.txt";
	const char** logFiles = &logFile;

	auto logParams = new Aws::GameLift::Server::LogParameters(logFiles, 1);

	auto params = new Aws::GameLift::Server::ProcessParameters(
		OnStartGameSession,
		&startGameSessionState_,
		OnUpdateGameSession,
		&updateGameSessionState_,
		OnProcessTerminate,
		&processTerminateState_,
		OnHealthCheck,
		&healthCheckState_,
		port,
		*logParams
	);

	auto ProcessReadeOutcome = Aws::GameLift::Server::ProcessReady(*params);
#endif
}
