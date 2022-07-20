// Fill out your copyright notice in the Description page of Project Settings.


#include "FpsGameMode.h"
#include "../AWSTestCharacter.h"
#include "GameLiftServerSDK.h"
#include "UObject/ConstructorHelpers.h"
#include "UnrealFpsGameHUD.h"
#include "UnrealFpsGamePlayerState.h"
#include "UnrealFpsGameGameState.h"
#include "ETeamType.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "TextReaderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Json.h"
#include "JsonUtilitys.h"


AFpsGameMode::AFpsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	if (PlayerPawnClassFinder.Class == nullptr)
	{
		check(!"PlayerPawnClassFinder.Class‚ªnullptr");
		return;
	}
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	HUDClass = AUnrealFpsGameHUD::StaticClass();
	PlayerStateClass = AUnrealFpsGamePlayerState::StaticClass();
	GameStateClass = AUnrealFpsGameGameState::StaticClass();
}

void AFpsGameMode::PreLogin(const FString& option, const FString& address, const FUniqueNetIdRepl& uniqueId, FString& errorMessage)
{
}

void AFpsGameMode::Logout(AController* exiting)
{
}

void AFpsGameMode::BeginPlay()
{
	Super::BeginPlay();
	AWSGameInit();

}

FString AFpsGameMode::InitNewPlayer(APlayerController* newPlayerController, const FUniqueNetIdRepl& uniqueId, const FString& options, const FString& portal)
{
	FString initialiizedString = Super::InitNewPlayer(newPlayerController, uniqueId, options, portal);

	/*if (newPlayerController == nullptr)
	{
		return initialiizedString;
	}

	APlayerState* playerState = newPlayerController->PlayerState;

	if (playerState == nullptr)
	{
		return initialiizedString;
	}

	AUnrealFpsGamePlayerState* fpsPlayerState = Cast<AUnrealFpsGamePlayerState>(playerState);

	if (fpsPlayerState == nullptr)
	{
		return initialiizedString;
	}
	fpsPlayerState->SetPlayerTeamType(ETeamType::GetRandomTeamType2String());*/

	return initialiizedString;
}

void AFpsGameMode::AWSGameInit()
{
#if WITH_GAMELIFT
	auto initSDKoutcome = Aws::GameLift::Server::InitSDK();

	if (!initSDKoutcome.IsSuccess())
	{
		check(!"AWSInitŽ¸”s");
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
			check(!"getTerminationTimeOutcome:Ž¸”s");
		}
		state->terminationTime_ = getTerminationTimeOutcome.GetResult();
		auto ProcessEndingOutcome = Aws::GameLift::Server::ProcessEnding();

		if (!ProcessEndingOutcome.IsSuccess())
		{
			check(!"ProcessEndingOutcome:Ž¸”s");
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
	/*if (GameState == nullptr)
	{
		check(!"GameState‚ªnullptr");
		return;
	}
	
	AUnrealFpsGameGameState* unrealFpsGameState = Cast<AUnrealFpsGameGameState>(GameState);
	if (unrealFpsGameState == nullptr)
	{
		check(!"unrealFpsGameState‚ªnullptr");
		return;
	}
	unrealFpsGameState->SetLatestEvent("GameEnded");
	unrealFpsGameState->SetWinningTeam(ETeamType::TeamType2FString(TEAMTYPE::RED));*/
}

void AFpsGameMode::CountDownUntilGameOver()
{
}

void AFpsGameMode::EndGame()
{
}

void AFpsGameMode::PickAWinningTeam()
{
}

void AFpsGameMode::HandleProcessTermination()
{
}

void AFpsGameMode::HandleGameSessionUpdate()
{
}

void AFpsGameMode::OnRecordMatchResultResponseReceived(FHttpRequestPtr Request, FHttpREsponsePtr response, bool bWasSuccessful)
{
}
