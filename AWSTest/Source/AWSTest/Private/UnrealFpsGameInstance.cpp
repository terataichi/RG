// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealFpsGameInstance.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "TextReaderComponent.h"

UUnrealFpsGameInstance::UUnrealFpsGameInstance()
{
	UTextReaderComponent* textReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	apiUrl_ = textReader->ReadFile("Urls/ApiUrl.txt");

	httpModule_ = &FHttpModule::Get();
}


void UUnrealFpsGameInstance::Shutdown()
{
	UGameInstance::Shutdown();

	// accessToken‚Ì–³Œø‰»ˆË—Š
	if (accessToken_.Len() > 0)
	{
		auto invalidateTokenRequest = httpModule_->CreateRequest();
		invalidateTokenRequest->SetURL(apiUrl_ + "/invalidatetokens");
		invalidateTokenRequest->SetVerb("Get");
		invalidateTokenRequest->SetHeader("Content-Type", "application/json");
		invalidateTokenRequest->SetHeader("Authorization", accessToken_);
		invalidateTokenRequest->ProcessRequest();
	}
}

void UUnrealFpsGameInstance::SetCognitoTokens(const FString& newAccessToken, const FString& newIdToken, const FString& newRefreshToken)
{
	accessToken_ = newAccessToken;
	idToken_ = newIdToken;
	refreshToken_ = newRefreshToken;

	//UE_LOG(LogTemp, Warning, TEXT("access token: %s"), *accessToken_);
	//UE_LOG(LogTemp, Warning, TEXT("refresh token: %s"), *refreshToken_);

	GetWorld()->GetTimerManager().SetTimer(retrieveNewTokensHandle_, this, &UUnrealFpsGameInstance::RetrieveNewTokens, 1.0f, false, 3300.0f);


}

void UUnrealFpsGameInstance::RetrieveNewTokens()
{
	if (accessToken_.Len() > 0 && refreshToken_.Len() > 0)
	{
		TSharedPtr<FJsonObject> requestObj = MakeShareable(new FJsonObject);

		requestObj->SetStringField("refreshToken", refreshToken_);

		FString requestBody;
		auto writer = TJsonWriterFactory<>::Create(&requestBody);

		if (FJsonSerializer::Serialize(requestObj.ToSharedRef(), writer))
		{
			auto retriveNewTokenRequest = httpModule_->CreateRequest();
			retriveNewTokenRequest->OnProcessRequestComplete().BindUObject(this, &UUnrealFpsGameInstance::OnRetrieveNewTokensResponseReceived);
			retriveNewTokenRequest ->SetURL(apiUrl_ + "/retrievenewtokens");
			retriveNewTokenRequest->SetVerb("POST");
			retriveNewTokenRequest->SetHeader("Content-Type", "application/json");
			retriveNewTokenRequest->SetHeader("Authorization", accessToken_);
			retriveNewTokenRequest->SetContentAsString(requestBody);
			retriveNewTokenRequest->ProcessRequest();
		}
		else {
			GetWorld()->GetTimerManager().SetTimer(retrieveNewTokensHandle_, this, &UUnrealFpsGameInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
		}
	}
}

void UUnrealFpsGameInstance::OnRetrieveNewTokensResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull)
{
	if (!bWasSuccessfull)
	{
		return;
	}
	TSharedPtr<FJsonObject> jsonObject;
	auto reader = TJsonReaderFactory<>::Create(response->GetContentAsString());
	if (!FJsonSerializer::Deserialize(reader,jsonObject) || jsonObject->HasField("error"))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRetrieveNewTokensResponseReceived"));
		GetWorld()->GetTimerManager().SetTimer(retrieveNewTokensHandle_, this, &UUnrealFpsGameInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
		return;
	}
	SetCognitoTokens(jsonObject->GetStringField("accessToken"), jsonObject->GetStringField("idToken"), refreshToken_);
}
