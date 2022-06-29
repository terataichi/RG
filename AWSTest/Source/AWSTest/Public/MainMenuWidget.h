// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "MainMenuWidget.generated.h"

class UWebBrowser;

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

	UFUNCTION()
		void HandleLoginUrlChange();

	void OnExchangeCodeForTokensResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull);
};
