// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AWSTestGameMode.generated.h"

UCLASS(minimalapi)
class AAWSTestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAWSTestGameMode();
	// ---------------------------------------------------------追加分-------------------------------------------------------
private:
	/// <summary>
	/// AWSのGameLift関連初期化まとめ
	/// コンストラクタで呼ぶ
	/// </summary>
	void GameLiftInit();
};



