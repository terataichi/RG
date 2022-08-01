// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnrealFpsGameWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class PROTOTYPE1_API UUnrealFpsGameWidget : public UUserWidget
{
	GENERATED_BODY()
public:

protected:
	virtual void NativeConstruct()override;
	virtual void NativeDestruct()override;

private:
	UPROPERTY()
		UTextBlock* eventTextBlock_;

	UFUNCTION()
		void SetLatesEvent();


	void InitTextBlocks();
	void InitTimers();

	void ClearTimers();

	FString GetOwingPlayerTeamName();

	UPROPERTY()
		FTimerHandle latesEventHandle_;
};
