// Fill out your copyright notice in the Description page of Project Settings.


#include "AwsGameLift/FpsGameMode.h"
#include "MyFps/MyFpsCharacter.h"

#include "GameLiftServerSDK.h"
#include "UObject/ConstructorHelpers.h"
#include "AwsGameLift/UnrealFpsGameHUD.h"
#include "AwsGameLift/UnrealFpsGamePlayerState.h"
#include "AwsGameLift/UnrealFpsGameGameState.h"
#include "AwsGameLift/ETeamType.h"
#include "AwsGameLift/TextReaderComponent.h"

#include "MyFps/MyFPSHUD.h"

#include "Runtime/Online/HTTP/Public/Http.h"
#include "Kismet/GameplayStatics.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "GameFramework/PlayerStart.h"

namespace
{
	constexpr int DEFAULT_RAMAININGGAMETIME = 240;
	const FString PLELOGIN_ERRORMES = "Unauthorized";
}

AFpsGameMode::AFpsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_MyFPSMoveItCharacter"));
	if (PlayerPawnClassFinder.Class == nullptr)
	{
		check(!"PlayerPawnClassFinder.Classがnullptr");
		return;
	}
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	HUDClass = AMyFPSHUD::StaticClass();
	PlayerStateClass = AUnrealFpsGamePlayerState::StaticClass();
	GameStateClass = AUnrealFpsGameGameState::StaticClass();

	UTextReaderComponent* textReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	apiUrl_ = textReader->ReadFile("Urls/ApiUrl.txt");

	httpModule_ = &FHttpModule::Get();

	remainingGameTime_ = DEFAULT_RAMAININGGAMETIME;
	gameSessionActivated_ = false;

}

void AFpsGameMode::PreLogin(const FString& option, const FString& address, const FUniqueNetIdRepl& uniqueId, FString& errorMessage)
{
	auto errorFunc = [](FString& errorMessage) {
		errorMessage = PLELOGIN_ERRORMES;
	};
	Super::PreLogin(option, address, uniqueId, errorMessage);
#if WITH_GAMELIFT
	if (option.Len() <= 0)
	{
		errorFunc(errorMessage);
		return;
	}

	const FString& playerSessionId = UGameplayStatics::ParseOption(option, "PlayerSessionId");
	const FString& playerId = UGameplayStatics::ParseOption(option, "playerId");
	if (playerSessionId.Len() <= 0 || playerId.Len() <= 0)
	{
		errorFunc(errorMessage);
		return;
	}

	Aws::GameLift::Server::Model::DescribePlayerSessionsRequest descrideplayerSessionRequest;
	descrideplayerSessionRequest.SetPlayerSessionId(TCHAR_TO_ANSI(*playerSessionId));
	
	auto describePlayerSessionOutcome = Aws::GameLift::Server::DescribePlayerSessions(descrideplayerSessionRequest);
	if (!describePlayerSessionOutcome.IsSuccess())
	{
		errorFunc(errorMessage);
		return;
	}
	auto describePlayerSessionResult = describePlayerSessionOutcome.GetResult();
	int count = 1;
	auto playerSessions = describePlayerSessionResult.GetPlayerSessions(count);
	if (playerSessions == nullptr)
	{
		errorFunc(errorMessage);
		return;
	}

	auto playerSession = playerSessions[0];
	FString expectedPlayerId = playerSession.GetPlayerId();
	auto playerStatus = playerSession.GetStatus();

	if (!(expectedPlayerId.Equals(playerId)) && !(playerStatus == Aws::GameLift::Server::Model::PlayerSessionStatus::RESERVED))
	{
		errorFunc(errorMessage);
		return;
	}

	auto acceptPlayerSessionOutcome = Aws::GameLift::Server::AcceptPlayerSession(TCHAR_TO_ANSI(*playerSessionId));
	if (!acceptPlayerSessionOutcome.IsSuccess())
	{
		errorFunc(errorMessage);
		return;
	}
#endif
}

void AFpsGameMode::Logout(AController* exiting)
{
#if WITH_GAMELIFT
	if (exiting == nullptr)
	{
		check(!"exitingがnullptr");
	}

	APlayerState* playerState = exiting->PlayerState;

	if (playerState == nullptr)
	{
		check(!"playerStateがnullptr");
	}
	AUnrealFpsGamePlayerState* fpsGamePlayerState = Cast<AUnrealFpsGamePlayerState>(playerState);

	const FString& playerSessionId = fpsGamePlayerState->GetPlayerSessionId();

	if (playerSessionId.Len() > 0)
	{
		Aws::GameLift::Server::RemovePlayerSession(TCHAR_TO_ANSI(*playerSessionId));
	}
#endif
	Super::Logout(exiting);
}

AUnrealFpsGameGameState* AFpsGameMode::GetGameState()
{
	if (GameState == nullptr)
	{
		return nullptr;
	}
	AUnrealFpsGameGameState* fpsGameState = Cast<AUnrealFpsGameGameState>(GameState);
	if (fpsGameState == nullptr)
	{
		return nullptr;
	}
	return fpsGameState;
}

void AFpsGameMode::BeginPlay()
{
	Super::BeginPlay();
	AWSGameInit();

}

FString AFpsGameMode::InitNewPlayer(APlayerController* newPlayerController, const FUniqueNetIdRepl& uniqueId, const FString& options, const FString& portal)
{
	FString initialiizedString = Super::InitNewPlayer(newPlayerController, uniqueId, options, portal);

	// server未接続時テスト用コード
	//if (newPlayerController == nullptr)
	//{
	//	return initialiizedString;
	//}

	//APlayerState* playerState = newPlayerController->PlayerState;

	//if (playerState == nullptr)
	//{
	//	return initialiizedString;
	//}

	//AUnrealFpsGamePlayerState* fpsPlayerState = Cast<AUnrealFpsGamePlayerState>(playerState);

	//if (fpsPlayerState == nullptr)
	//{
	//	return initialiizedString;
	//}
	//fpsPlayerState->SetPlayerTeamType(ETeamType::GetRandomTeamType2String());

#if WITH_GAMELIFT
	const FString& playerSessionId = UGameplayStatics::ParseOption(options, "PlayerSessionId");
	const FString& playerId = UGameplayStatics::ParseOption(options, "PlayerId");

	if (newPlayerController == nullptr)
	{
		check(!"newPlayerControllerがnullptr");
		return initialiizedString;
	}
	APlayerState* playerState = newPlayerController->PlayerState;

	if (playerState == nullptr)
	{
		check(!"playerStateがnullptr");
		return initialiizedString;
	}

	AUnrealFpsGamePlayerState* fpsGamePlayerState = Cast<AUnrealFpsGamePlayerState>(playerState);

	if (fpsGamePlayerState == nullptr)
	{
		check(!"fpsGamePlayerStateがnullptr");
		return initialiizedString;
	}

	fpsGamePlayerState->SetPlayerSessionId(playerSessionId);
	fpsGamePlayerState->SetMatchmakingPlayerId(playerId);

	if (startGameSessionState_.playerIdToPlayer_.Num() <= 0)
	{
		return initialiizedString;
	}

	auto playerObj = startGameSessionState_.playerIdToPlayer_.Find(playerId);

	FString team = playerObj->GetTeam();
	fpsGamePlayerState->SetPlayerTeamType(team);
#endif
	return initialiizedString;
}

AActor* AFpsGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// チーム情報が欲しいのでプレイヤーステートをもらう
	AUnrealFpsGamePlayerState* state = Player->GetPlayerState<AUnrealFpsGamePlayerState>();
	if (state == nullptr)
	{
		check(!"ChoosePlayerStart_Implementation");
		return nullptr;
	}

	auto str = state->GetTeamString();
	FName ConvertedFString = FName(*str);

	TSubclassOf<APlayerStart> FindClass;
	FindClass = APlayerStart::StaticClass();

	// ワールド内のチームタグと同じプレイヤースタートを探す
	TArray<AActor*> FindPlayerStart;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), FindClass, ConvertedFString, FindPlayerStart);

	// サイズチェック
	if (FindPlayerStart.Num() != 1)
	{
		//check(!"ChoosePlayerStart_Implementation : Size 0");
		return nullptr;
	}

	// １つしか置いてないはずなのでないと思うが一応回してあれば返す
	for (auto playerStart : FindPlayerStart)
	{
		if (playerStart != nullptr)
		{
			return playerStart;
		}
	}

	return nullptr;
}

void AFpsGameMode::AWSGameInit()
{
#if WITH_GAMELIFT
	auto initSDKoutcome = Aws::GameLift::Server::InitSDK();

	if (!initSDKoutcome.IsSuccess())
	{
		check(!"AWSInit失敗");
	}
	auto OnStartGameSession = [](Aws::GameLift::Server::Model::GameSession gameSessionObj, void* params) {
		FStartGameSessionState* state = static_cast<FStartGameSessionState*>(params);
		state->status_ = Aws::GameLift::Server::ActivateGameSession().IsSuccess();
		FString matchmakerData = gameSessionObj.GetMatchmakerData();
		TSharedPtr<FJsonObject> jsonObject;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(matchmakerData);

		if (!FJsonSerializer::Deserialize(reader,jsonObject))
		{
			return;
		}
		state->matchmakingConfigurationArn_ = jsonObject->GetStringField("matchmakingConfigurationArn");

		TArray<TSharedPtr<FJsonValue>> teams = jsonObject->GetArrayField("teams");
		for (TSharedPtr<FJsonValue> team : teams)
		{
			TSharedPtr<FJsonObject> teamObj = team->AsObject();
			FString teamName = teamObj->GetStringField("name");

			TArray<TSharedPtr<FJsonValue>> players = teamObj->GetArrayField("players");
			for (TSharedPtr<FJsonValue> player: players)
			{
				TSharedPtr<FJsonObject> playerObj = player->AsObject();
				FString playerId = playerObj->GetStringField("playerId");

				TSharedPtr<FJsonObject> attributes = playerObj->GetObjectField("attributes");
				TSharedPtr<FJsonObject> skill = attributes->GetObjectField("skill");
				FString skillValue = skill->GetStringField("valueAttribute");
				auto skillattributeValue = new Aws::GameLift::Server::Model::AttributeValue(FCString::Atod(*skillValue));

				Aws::GameLift::Server::Model::Player awsPlayerObj;
				awsPlayerObj.SetPlayerId(TCHAR_TO_ANSI(*playerId));
				awsPlayerObj.SetTeam(TCHAR_TO_ANSI(*teamName));
				awsPlayerObj.AddPlayerAttribute("skill", *skillattributeValue);

				state->playerIdToPlayer_.Add(playerId, awsPlayerObj);
			}
		}
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
			check(!"getTerminationTimeOutcome:失敗");
		}
		state->terminationTime_ = getTerminationTimeOutcome.GetResult();

		state->status_ = true;
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
			else if (key.Equals("password"))
			{
				serverPassword_ = value;
			}
			else
			{
				// 何もしない
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
	GetWorldTimerManager().SetTimer(handleGameSessionUpdateHandle_, this, &AFpsGameMode::HandleGameSessionUpdate, 1.0f, true, 5.0f);
	GetWorldTimerManager().SetTimer(handleProcessTerminationHandle_, this, &AFpsGameMode::HandleProcessTermination, 1.0f, true, 5.0f);

	// チーム確認用コード
	/*if (GameState == nullptr)
	{
		check(!"GameStateがnullptr");
		return;
	}
	
	AUnrealFpsGameGameState* unrealFpsGameState = Cast<AUnrealFpsGameGameState>(GameState);
	if (unrealFpsGameState == nullptr)
	{
		check(!"unrealFpsGameStateがnullptr");
		return;
	}
	unrealFpsGameState->SetLatestEvent("GameEnded");
	unrealFpsGameState->SetWinningTeam(ETeamType::TeamType2FString(TEAMTYPE::RED));*/
}

void AFpsGameMode::CountDownUntilGameOver()
{
	AUnrealFpsGameGameState* fpsGameState = GetGameState();
	fpsGameState->SetLatestEvent(FString::FromInt(remainingGameTime_) + " seconds until the game is over");

	if (remainingGameTime_ <= 0)
	{
		GetWorldTimerManager().ClearTimer(countDownUntilGameOverHandle_);
		return;
	}
	remainingGameTime_--;
}

void AFpsGameMode::EndGame()
{
	GetWorldTimerManager().ClearTimer(countDownUntilGameOverHandle_);
	GetWorldTimerManager().ClearTimer(endGameHandle_);
	GetWorldTimerManager().ClearTimer(pickAWinningHandle_);
	GetWorldTimerManager().ClearTimer(handleProcessTerminationHandle_);
	GetWorldTimerManager().ClearTimer(handleGameSessionUpdateHandle_);

#if WITH_GAMELIFT
	Aws::GameLift::Server::ProcessEnding();
	FGenericPlatformMisc::RequestExit(false);
#endif
}

void AFpsGameMode::PickAWinningTeam()
{
	auto errorFunc = [this]() {
		GetWorldTimerManager().SetTimer(endGameHandle_, this, &AFpsGameMode::EndGame, 1.0f, false, 5.0f);
	};

	GetWorldTimerManager().ClearTimer(countDownUntilGameOverHandle_);
#if WITH_GAMELIFT
	AUnrealFpsGameGameState* fpsGameState = GetGameState();
	if (fpsGameState == nullptr)
	{
		errorFunc();
		return;
	}
	fpsGameState->SetLatestEvent("GameEnded");
	if (FMath::RandRange(0, 1) == 0)
	{
		fpsGameState->SetWinningTeam(ETeamType::TeamType2FString(TEAMTYPE::RED));
	}
	else
	{
		fpsGameState->SetWinningTeam(ETeamType::TeamType2FString(TEAMTYPE::BLUE));
	}

	TSharedPtr<FJsonObject> requestObj = MakeShareable(new FJsonObject);
	requestObj->SetStringField("winningTeam", fpsGameState->GetWinningTeam());

	auto getGameSessionIdOutcome = Aws::GameLift::Server::GetGameSessionId();
	if (!getGameSessionIdOutcome.IsSuccess())
	{
		errorFunc();
		return;
	}
	requestObj->SetStringField("gameSessionId", getGameSessionIdOutcome.GetResult());

	FString requestBody;
	TSharedRef<TJsonWriter<>> witer = TJsonWriterFactory<>::Create(&requestBody);
	if (!FJsonSerializer::Serialize(requestObj.ToSharedRef(),witer))
	{
		errorFunc();
		return;
	}
	auto requestMatchResultRequest = httpModule_->CreateRequest();
	requestMatchResultRequest->OnProcessRequestComplete().BindUObject(this, &AFpsGameMode::OnRecordMatchResultResponseReceived);
	requestMatchResultRequest->SetURL(apiUrl_ + "/recordmatchresult");
	requestMatchResultRequest->SetVerb("POST");
	requestMatchResultRequest->SetHeader("Authorization", serverPassword_);
	requestMatchResultRequest->SetHeader("Content-Type", "application/json");
	requestMatchResultRequest->SetContentAsString(requestBody);
	requestMatchResultRequest->ProcessRequest();
#endif
}

void AFpsGameMode::HandleProcessTermination()
{
	if (!processTerminateState_.status_)
	{
		return;
	}
	GetWorldTimerManager().ClearTimer(countDownUntilGameOverHandle_);
	GetWorldTimerManager().ClearTimer(handleProcessTerminationHandle_);
	GetWorldTimerManager().ClearTimer(handleGameSessionUpdateHandle_);

	GetWorldTimerManager().SetTimer(endGameHandle_, this, &AFpsGameMode::EndGame, 1.0f, false, 10.0f);

	FString processInterruptionMessage;

	if (processTerminateState_.terminationTime_ <= 0L)
	{
		processInterruptionMessage = "Server process could shut down at any time";
	}
	else
	{
		long timeLift = static_cast<long>(processTerminateState_.terminationTime_ - FDateTime::Now().ToUnixTimestamp());
		processInterruptionMessage = FString::Printf(TEXT("Server process scheduled to terminate in %ld seconds"), timeLift);
	}

	AUnrealFpsGameGameState* fpsGameState = GetGameState();
	if (fpsGameState == nullptr)
	{
		return;
	}
	fpsGameState->SetLatestEvent(processInterruptionMessage);
}

void AFpsGameMode::HandleGameSessionUpdate()
{
	if (gameSessionActivated_)
	{
		return;
	}
	if (!startGameSessionState_.status_)
	{
		return;
	}
	gameSessionActivated_ = true;

	GetWorldTimerManager().SetTimer(pickAWinningHandle_, this, &AFpsGameMode::PickAWinningTeam, 1.0f, false, static_cast<float>(remainingGameTime_));
	GetWorldTimerManager().SetTimer(countDownUntilGameOverHandle_, this, &AFpsGameMode::CountDownUntilGameOver, 1.0f, true, 0.0f);
}

void AFpsGameMode::OnRecordMatchResultResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr response, bool bWasSuccessful)
{
	GetWorldTimerManager().SetTimer(endGameHandle_, this, &AFpsGameMode::EndGame, 1.0f, false, 5.0f);
}
