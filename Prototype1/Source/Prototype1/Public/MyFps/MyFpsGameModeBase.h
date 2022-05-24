// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyFpsGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE1_API AMyFpsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual void StartPlay()override;
};
