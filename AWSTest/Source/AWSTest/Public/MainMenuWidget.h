// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "utility"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "MainMenuWidget.generated.h"

class UWebBrowser;
class UButton;
class UTextBlock;
class FJsonObject;

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
private:
	FHttpModule* httpModule_;

	UPROPERTY()
		FString loginUrl_;

	UPROPERTY()
		FString apiUrl_;

	UPROPERTY()
		FString callbackUrl_;

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

	UFUNCTION()
		void HandleLoginUrlChange();

	/// <summary>
	/// 受信したresponseがエラーかどうかチェック
	/// </summary>
	/// <param name="response">受信したresponse</param>
	/// <returns>first:errorかどうか true = error,second:responeのContentのJson errorの場合空のJsonObject</returns>
	std::pair<bool,TSharedPtr<FJsonObject>> CheckJsonError(FHttpResponsePtr response);

	void InitWidgets();

	void PlayerDataReques(const FString& accessToken);

	void OnExchangeCodeForTokensResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull);

	void OnGetPlayerDataResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull);
};
