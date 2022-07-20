// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "utility"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/IHttpRequest.h"
#include "MainMenuWidget.generated.h"

class UWebBrowser;
class UButton;
class UTextBlock;
class FJsonObject;
class UUnrealFpsGameInstance;
class FHttpModule;

/**
 * 
 */
UCLASS()
class AWSTEST_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMainMenuWidget(const FObjectInitializer& objectInitializer);
	
protected:
	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;
private:
	FHttpModule* httpModule_;

	UPROPERTY()
		FString loginUrl_;

	UPROPERTY()
		FString apiUrl_;

	UPROPERTY()
		FString callbackUrl_;

	UPROPERTY()
		FString regionCode_;

	UPROPERTY()
		UWebBrowser* webBrowser_;

	UPROPERTY()
		UButton* matchmakingButton_;

	UPROPERTY()
		UTextBlock* winsTextBlock_;

	UPROPERTY()
		UTextBlock* lossesTextBlock_;

	UPROPERTY()
		UTextBlock* pingTextBlock_;

	UPROPERTY()
		UTextBlock* matchmakingEventTextBlock_;

	UPROPERTY()
		FTimerHandle averagePlayerLatencyHandle_;

	UPROPERTY()
		FTimerHandle pollMatchmakingHandle_;

	UPROPERTY()
		float averagePlayerLatency_;

	UPROPERTY()
		bool searchingForGame_;

	UFUNCTION()
		void HandleLoginUrlChange();

	UFUNCTION()
		void SetAveragePlayerLatency();

	UFUNCTION()
		void OnMatchmakingButtonClicked();

	UFUNCTION()
		void PollMatchmaking();

	/// <summary>
	/// UnrealFpsGameInstance�ւ̃|�C���^�̊l���Bnullcheck�ς�
	/// </summary>
	/// <returns>gameInstance�ւ̃|�C���^</returns>
	UUnrealFpsGameInstance* GetFpsGameInstance();

	bool SendStopMatchmakingRequest(const FString& accessToken, const FString& matchmakingTicketId);

	bool SendStartMatchmakingRequest(const FString& accessToken);

	/// <summary>
	/// ��M����response���G���[���ǂ����`�F�b�N
	/// </summary>
	/// <param name="response">��M����response</param>
	/// <returns>first:error���ǂ��� true = error,second:respone��Content��Json error�̏ꍇ���JsonObject</returns>
	std::pair<bool,TSharedPtr<FJsonObject>> CheckJsonError(FHttpResponsePtr response);

	void InitWidgets();

	void PlayerDataReques(const FString& accessToken);

	void OnExchangeCodeForTokensResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull);

	void OnGetPlayerDataResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull);

	void OnStartMatchmakingResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull);

	void OnStopMatchmakingResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull);

	void OnPollMatchmakingResponceReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull);
};
