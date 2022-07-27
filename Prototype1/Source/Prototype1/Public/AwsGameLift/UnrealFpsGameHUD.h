// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UnrealFpsGameHUD.generated.h"

class UUSerWidget;

/**
 * 
 */
UCLASS()
class PROTOTYPE1_API AUnrealFpsGameHUD : public AHUD
{
	GENERATED_BODY()
public:
	AUnrealFpsGameHUD();

protected:
	virtual void BeginPlay()override;

private:
	UPROPERTY()
		TSubclassOf<UUserWidget> gameWidgetClass_;

};
