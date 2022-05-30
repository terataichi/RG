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
	// ---------------------------------------------------------�ǉ���-------------------------------------------------------
private:
	/// <summary>
	/// AWS��GameLift�֘A�������܂Ƃ�
	/// �R���X�g���N�^�ŌĂ�
	/// </summary>
	void GameLiftInit();
};



