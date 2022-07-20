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
class AWSTEST_API UUnrealFpsGameWidget : public UUserWidget
{
	GENERATED_BODY()
public:

protected:
	virtual void NativeConstruct()override;
	virtual void NativeDestruct()override;

private:
	UPROPERTY()
		UTextBlock* teamNameTextBlock_;

	UPROPERTY()
		UTextBlock* teammateCountTextBlock_;

	UPROPERTY()
		UTextBlock* eventTextBlock_;

	UPROPERTY()
		UTextBlock* pingTextBlock_;

	UFUNCTION()
		void SetTeammateCount();

	UFUNCTION()
		void SetLatesEvent();

	UFUNCTION()
		void SetAveragePlayerLatency();


	void InitTextBlocks();
	void InitTimers();

	void ClearTimers();

	FString GetOwingPlayerTeamName();

	UPROPERTY()
		FTimerHandle teammateCountHandle_;

	UPROPERTY()
		FTimerHandle latesEventHandle_;

	UPROPERTY()
		FTimerHandle averagePlayerLatencyHandle_;
};
