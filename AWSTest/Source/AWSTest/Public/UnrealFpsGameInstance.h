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

	virtual void Init() override;

	const TDoubleLinkedList<float>& GetPlayerLatencies()const;

	const FString& GetAccessToken()const;

	const FString& GetIdToken()const;

	const FString& GetRefreshToken()const;

	const FString& GetMatchmakingTicketID()const;

	void SetMatchmakingTicketID(const FString& ticketId);

	UFUNCTION()
		void SetCognitoTokens(const FString& newAccessToken,const FString& newIdToken, const FString& newRefreshToken);

private:
	FHttpModule* httpModule_;

	TDoubleLinkedList<float> playerLatencies_;

	UPROPERTY()
		FString accessToken_;

	UPROPERTY()
		FString idToken_;

	UPROPERTY()
		FString refreshToken_;

	UPROPERTY()
		FString matchmakingTicketId_;

	UPROPERTY()
		FTimerHandle retrieveNewTokensHandle_;

	UPROPERTY()
		FTimerHandle responeTimeHandle_;

	UPROPERTY()
		FString apiUrl_;

	UPROPERTY()
		FString regionCode_;

	UFUNCTION()
		void RetrieveNewTokens();

	UFUNCTION()
		void GetRsponseTime();

	void OnRetrieveNewTokensResponseReceived(FHttpRequestPtr request,FHttpResponsePtr response,bool bWasSuccessfull);
	void OnGetResponseTimeResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull);
};
