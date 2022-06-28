// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "UnrealFpsGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class AWSTEST_API UUnrealFpsGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UUnrealFpsGameInstance();

	virtual void Shutdown()override;

	UPROPERTY()
		FString accessToken_;

	UPROPERTY()
		FString idToken_;

	UPROPERTY()
		FString refreshToken_;

	UPROPERTY()
		FTimerHandle retrieveNewTokensHandle_;

	UFUNCTION()
		void SetCognitoTokens(const FString& newAccessToken,const FString& newIdToken, const FString& newRefreshToken);

private:
	FHttpModule* httpModule_;

	UPROPERTY()
		FString apiUrl_;

	UFUNCTION()
		void RetrieveNewTokens();

	void OnRetrieveNewTokensResponseReceived(FHttpRequestPtr request,FHttpResponsePtr response,bool bWasSuccessfull);
};
