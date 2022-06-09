// Fill out your copyright notice in the Description page of Project Settings.


#include "OfflineMainmenuWidget.h"
#include "Json.h"
#include "JsonUtilities/Public/JsonUtilities.h"
#include "Kismet/GameplayStatics.h"

UOfflineMainmenuWidget::UOfflineMainmenuWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	http_ = &FHttpModule::Get();

	apiGatewayEndPoint_ = FString::Printf(TEXT("https://53lfdkkvk5.execute-api.ap-northeast-1.amazonaws.com/test"));
	logInURI_ = FString::Printf(TEXT("/login"));
	stratSessionURI_ = FString::Printf(TEXT("/startsession"));
}

void UOfflineMainmenuWidget::OnLoginClicked()
{
	LogInRequest(user_,pass_);
}

void UOfflineMainmenuWidget::LogInRequest(FString usr, FString pwd)
{
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
	jsonObject->SetStringField(TEXT("username"), FString::Printf(TEXT("%s"), *usr));
	jsonObject->SetStringField(TEXT("password"), FString::Printf(TEXT("%s"), *pwd));

	FString jsonBody;
	TSharedRef<TJsonWriter<TCHAR>> jsonWiter = TJsonWriterFactory<>::Create(&jsonBody);
	FJsonSerializer::Serialize(jsonObject.ToSharedRef(), jsonWiter);

	TSharedRef<IHttpRequest,ESPMode::ThreadSafe> loginHttpRequest = http_->CreateRequest();

	loginHttpRequest->SetVerb("POST");
	loginHttpRequest->SetURL(apiGatewayEndPoint_ + logInURI_);
	loginHttpRequest->SetHeader("Content-Type", "application/json");
	loginHttpRequest->SetContentAsString(jsonBody);
	loginHttpRequest->OnProcessRequestComplete().BindUObject(this, &UOfflineMainmenuWidget::OnLoginResponse);
	loginHttpRequest->ProcessRequest();
}

void UOfflineMainmenuWidget::OnLoginResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool bWesSuccessful)
{
	if (bWesSuccessful)
	{
		TSharedPtr<FJsonObject> jsonObject;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(response->GetContentAsString());

		if (FJsonSerializer::Deserialize(reader, jsonObject))
		{
			// server‚©‚ç‚Ìjsonfile‚©‚çID‚ðŽæ‚èo‚·
			FString IdToken = jsonObject->GetObjectField("tokens")->GetStringField("IdToken");
			UE_LOG(LogTemp, Log, TEXT("SuccessLogin"));
			StartSessionRequest(IdToken);
		}
	}
}

void UOfflineMainmenuWidget::StartSessionRequest(FString idt)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> startSessionHttpRequest = http_->CreateRequest();

	startSessionHttpRequest->SetVerb("GET");
	startSessionHttpRequest->SetURL(apiGatewayEndPoint_ + stratSessionURI_);
	startSessionHttpRequest->SetHeader("Content-Type", "application/json");
	startSessionHttpRequest->SetHeader("Authorization", idt);
	startSessionHttpRequest->OnProcessRequestComplete().BindUObject(this, &UOfflineMainmenuWidget::OnStartSessionResponse);
	startSessionHttpRequest->ProcessRequest();
}

void UOfflineMainmenuWidget::OnStartSessionResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> jsonObject;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(response->GetContentAsString());

		if (FJsonSerializer::Deserialize(reader,jsonObject))
		{
			FString ipAddress = jsonObject->GetObjectField("PlayerSession")->GetStringField("IpAddress");
			FString port = jsonObject->GetObjectField("PlayerSession")->GetStringField("Port");

			FString levelName = ipAddress + ":" + port;
			UE_LOG(LogTemp, Log, TEXT("OpenLevel"));
			UGameplayStatics::OpenLevel(GetWorld(), FName(*levelName), false);
		}
	}
}
