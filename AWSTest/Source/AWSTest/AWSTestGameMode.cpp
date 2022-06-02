// Copyright Epic Games, Inc. All Rights Reserved.

#include "AWSTestGameMode.h"
#include "AWSTest.h"
#include "Engine.h"
#include "EngineGlobals.h"
#include "AWSTestHUD.h"
#include "AWSTestCharacter.h"
#include "GameLiftServerSDK.h"
#include "UObject/ConstructorHelpers.h"

AAWSTestGameMode::AAWSTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AAWSTestHUD::StaticClass();
	GameLiftInit();
}

void AAWSTestGameMode::GameLiftInit()
{
	// ServerTargetかつGameLiftが有効な場合のみ有効
#if WITH_GAMELIFT
	
	// 最初にGameLiftModuleを取得する
	FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

	// GameLiftとのローカル接続を確立し、通信を可能状態にする
	gameLiftSdkModule->InitSDK();

	// 新しいゲームセッションのアクティベーション要求に応答する
	// ゲームサーバーにゲームプロパティを含むゲームセッションオブジェクト及びサーバー起動要求を送信する
	// ゲームサーバーがプレイヤーの接続を受け入れる準備が出来たら GameLiftAPI.Session を呼び出す
	
	auto onGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		gameLiftSdkModule->ActivateGameSession();
	};

	FProcessParameters* params = new FProcessParameters();
	params->OnStartGameSession.BindLambda(onGameSession);

	// OnProcessTerminateコールバック。GameLiftはこのインスタンスをシャットダウンする前に呼び出す

	// 単にGameLiftをシャットダウンすることを伝えているだけ
	auto terminateGameSession = [=]()
	{
		gameLiftSdkModule->ProcessEnding();
	};
	params->OnTerminate.BindLambda(terminateGameSession);

	// HealthCheckコールバック。GameLiftは約60秒ごとにこのコールバックを呼び出す。
	// デフォルトでは自動的にtrueを返す。ゲームはオプションで以下のチェックを行うことができるらしい
	// 依存関係のチェックを行い、その情報に基づいてステータスを報告することができる。60秒以内に応答がない場合falseが記録される
	// なお今回は常にtrue
	auto healthCheck = []()
	{
		return true;
	};
	params->OnHealthCheck.BindLambda(healthCheck);

	// GameLiftへのport番号の通知
	params->port = 7777;

	// GameLiftに対して、ゲームセッションが終了したときにアップロードすべきファイル群の指示
	TArray<FString> logfiles;
	logfiles.Add(TEXT("aLogFile.txt"));
	params->logParameters = logfiles;

	// ProcessReadyを呼び出してGameLiftにこのゲームサーバーが準備完了なことを伝える
	gameLiftSdkModule->ProcessReady(*params);
#endif
}
