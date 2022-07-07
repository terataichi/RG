// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealFpsGameInstance.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "TextReaderComponent.h"

namespace
{
	constexpr int MAX_PLAYERLATENCIE_NUM = 4;
	constexpr float TIME_COEFFICIENT = 1000.0f;
}

UUnrealFpsGameInstance::UUnrealFpsGameInstance()
{
	UTextReaderComponent* textReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	apiUrl_ = textReader->ReadFile("Urls/ApiUrl.txt");
	regionCode_ = textReader->ReadFile("Urls/RegionCode.txt");

	httpModule_ = &FHttpModule::Get();
}


void UUnrealFpsGameInstance::Shutdown()
{

	GetWorld()->GetTimerManager().ClearTimer(retrieveNewTokensHandle_);
	GetWorld()->GetTimerManager().ClearTimer(responeTimeHandle_);

	// accessToken‚Ì–³Œø‰»ˆË—Š
	if (accessToken_.Len() > 0)
	{
		// matchmakingI—¹ˆ—
		if (matchmakingTicketId_.Len() > 0)
		{
			EndMatchmaking();
		}
		auto invalidateTokenRequest = httpModule_->CreateRequest();
		invalidateTokenRequest->SetURL(apiUrl_ + "/invalidatetokens");
		invalidateTokenRequest->SetVerb("Get");
		invalidateTokenRequest->SetHeader("Content-Type", "application/json");
		invalidateTokenRequest->SetHeader("Authorization", accessToken_);
		invalidateTokenRequest->ProcessRequest();
	}
	Super::Shutdown();
}

void UUnrealFpsGameInstance::Init()
{
	Super::Init();

	GetWorld()->GetTimerManager().SetTimer(responeTimeHandle_, this, &UUnrealFpsGameInstance::GetRsponseTime, 1.0f, true, 1.0f);
}

const TDoubleLinkedList<float>& UUnrealFpsGameInstance::GetPlayerLatencies()const
{
	return playerLatencies_;
}

const FString& UUnrealFpsGameInstance::GetAccessToken() const
{
	return accessToken_;
}

const FString& UUnrealFpsGameInstance::GetIdToken() const
{
	return idToken_;
}

const FString& UUnrealFpsGameInstance::GetRefreshToken() const
{
	return refreshToken_;
}

const FString& UUnrealFpsGameInstance::GetMatchmakingTicketID() const
{
	return matchmakingTicketId_;
}

void UUnrealFpsGameInstance::SetMatchmakingTicketID(const FString& ticketId)
{
	matchmakingTicketId_ = ticketId;
}

void UUnrealFpsGameInstance::SetCognitoTokens(const FString& newAccessToken, const FString& newIdToken, const FString& newRefreshToken)
{
	accessToken_ = newAccessToken;
	idToken_ = newIdToken;
	refreshToken_ = newRefreshToken;

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

void UUnrealFpsGameInstance::GetRsponseTime()
{
	auto responseTimeRequest = httpModule_->CreateRequest();
	responseTimeRequest->OnProcessRequestComplete().BindUObject(this, &UUnrealFpsGameInstance::OnGetResponseTimeResponseReceived);
	responseTimeRequest->SetURL("https://gamelift." + regionCode_ + ".amazonaws.com");
	responseTimeRequest->SetVerb("GET");
	responseTimeRequest->ProcessRequest();
}

void UUnrealFpsGameInstance::EndMatchmaking()
{
	TSharedPtr<FJsonObject> requestObject = MakeShareable(new FJsonObject);
	requestObject->SetStringField("ticketId", matchmakingTicketId_);

	FString requestBody;

	auto writer = TJsonWriterFactory<>::Create(&requestBody);

	if (!FJsonSerializer::Serialize(requestObject.ToSharedRef(), writer))
	{
		check(!"SerializeŽ¸”s");
		return;
	}
	auto stopMatchmakingRequest = httpModule_->CreateRequest();
	stopMatchmakingRequest->SetURL(apiUrl_ + "/stopmatchmaking");
	stopMatchmakingRequest->SetVerb("POST");
	stopMatchmakingRequest->SetHeader("Content-Type", "application/json");
	stopMatchmakingRequest->SetHeader("Authorization", accessToken_);
	stopMatchmakingRequest->SetContentAsString(requestBody);
	stopMatchmakingRequest->ProcessRequest();
}

void UUnrealFpsGameInstance::OnRetrieveNewTokensResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull)
{
	if (!bWasSuccessfull)
	{
		check(!"OnRetrieveNewTokensResponseReceived is not bWasSuccessfull");
		return;
	}
	TSharedPtr<FJsonObject> jsonObject;
	auto reader = TJsonReaderFactory<>::Create(response->GetContentAsString());
	if (!FJsonSerializer::Deserialize(reader,jsonObject) || jsonObject->HasField("error") || !jsonObject->HasField("accessToken") || !jsonObject->HasField("idToken"))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRetrieveNewTokensResponseReceived"));
		GetWorld()->GetTimerManager().SetTimer(retrieveNewTokensHandle_, this, &UUnrealFpsGameInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
		return;
	}
	SetCognitoTokens(jsonObject->GetStringField("accessToken"), jsonObject->GetStringField("idToken"), refreshToken_);
}

void UUnrealFpsGameInstance::OnGetResponseTimeResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull)
{
	if (!bWasSuccessfull)
	{
		check(!"OnGetResponseTimeResponseReceived is not bWasSuccessfull");
		return;
	}

	if (playerLatencies_.Num() >= MAX_PLAYERLATENCIE_NUM)
	{
		playerLatencies_.RemoveNode(playerLatencies_.GetHead());
	}

	float responseTime = request->GetElapsedTime() * TIME_COEFFICIENT;

	//UE_LOG(LogTemp, Warning, TEXT("response time in milliseconds:  %s"), *FString::SanitizeFloat(responseTime));

	playerLatencies_.AddTail(responseTime);
}
