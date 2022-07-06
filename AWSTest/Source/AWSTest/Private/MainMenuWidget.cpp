// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "TextReaderComponent.h"

#include "WebBrowser.h"
#include "WebBrowserModule.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "IWebBrowserSingleton.h"
#include "IWebBrowserCookieManager.h"
#include "../Public/UnrealFpsGameInstance.h"
#include "Json.h"
#include "JsonUtilities.h"

namespace
{
	constexpr float DEFAULT_PLAYERLATENCY = 60.0f;
}

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& objectInitializer):
	UUserWidget(objectInitializer)
{
	UTextReaderComponent* textReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	loginUrl_ = textReader->ReadFile("Urls/LoginUrl.txt");

	apiUrl_ = textReader->ReadFile("Urls/ApiUrl.txt");
	callbackUrl_ = textReader->ReadFile("Urls/CallbackUrl.txt");
	regionCode_ = textReader->ReadFile("Urls/RegionCode.txt");

	httpModule_ = &FHttpModule::Get();

	averagePlayerLatency_ = DEFAULT_PLAYERLATENCY;

	searchingForGame_ = false;
}

void UMainMenuWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();
	bIsFocusable = true;

	InitWidgets();

	FScriptDelegate matchmakingDelegate;
	matchmakingDelegate.BindUFunction(this, "OnMatchmakingButtonClicked");
	matchmakingButton_->OnClicked.Add(matchmakingDelegate);

	GetWorld()->GetTimerManager().SetTimer(averagePlayerLatencyHandle_, this, &UMainMenuWidget::SetAveragePlayerLatency, 1.0f, true, 1.0f);

	IWebBrowserSingleton* webBrowserSinglton = IWebBrowserModule::Get().GetSingleton();
	
	if (webBrowserSinglton == nullptr)
	{
		check(!"webBrowserSingltonがnullptr");
	}
	TOptional<FString> defaultContext;
	TSharedPtr<IWebBrowserCookieManager> cookieManager = webBrowserSinglton->GetCookieManager(defaultContext);
	if (!cookieManager.IsValid())
	{
		check(!"IWebBrowserCookieManagerが無効");
	}
	cookieManager->DeleteCookies();

	webBrowser_->LoadURL(loginUrl_);

	FScriptDelegate loginDelegate;
	loginDelegate.BindUFunction(this, "HandleLoginUrlChange");
	webBrowser_->OnUrlChanged.Add(loginDelegate);

	UE_LOG(LogTemp, Log, TEXT("UMainMenuWidget:NativeConstruct"));
}

std::pair<bool, TSharedPtr<FJsonObject>> UMainMenuWidget::CheckJsonError(FHttpResponsePtr response)
{
	TSharedPtr<FJsonObject> jsonObject;
	auto reader = TJsonReaderFactory<>::Create(response->GetContentAsString());

	if (!FJsonSerializer::Deserialize(reader, jsonObject) || jsonObject->HasField("error"))
	{
		check(!"json is error");
		// エラーだからからのObject返しとく
		return std::pair<bool, TSharedPtr<FJsonObject>>{true,TSharedPtr<FJsonObject>()};
	}
	return std::pair<bool, TSharedPtr<FJsonObject>>{false, jsonObject};
}

void UMainMenuWidget::InitWidgets()
{
	webBrowser_ = Cast<UWebBrowser>(GetWidgetFromName(TEXT("WebBrowser_Login")));

	matchmakingButton_ = Cast<UButton>(GetWidgetFromName(TEXT("Button_MatchMaking")));

	winsTextBlock_ = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_Wins")));

	lossesTextBlock_ = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_Losses")));

	pingTextBlock_ = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_Ping")));

	matchmakingEventTextBlock_ = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_MatchMakingEvent")));
}

void UMainMenuWidget::HandleLoginUrlChange()
{
	UE_LOG(LogTemp, Log, TEXT("UMainMenuWidget:HandleLoginUrlChange"));
	FString browserUrl = webBrowser_->GetUrl();
	FString url;
	FString queryParameters;

	if(!browserUrl.Split("?",&url,&queryParameters) || !url.Equals(callbackUrl_))
	{
		return;
	}
	FString parameterName;
	FString parameterValue;

	if (!queryParameters.Split("=",&parameterName,&parameterValue) || !parameterName.Equals("code"))
	{
		return;
	}

	parameterValue = parameterValue.Replace(*FString("#"), *FString(""));

	TSharedPtr<FJsonObject> requestObj = MakeShareable(new FJsonObject);
	requestObj->SetStringField(parameterName, parameterValue);

	FString requestBody;
	auto writer = TJsonWriterFactory<>::Create(&requestBody);

	if (!FJsonSerializer::Serialize(requestObj.ToSharedRef(), writer))
	{
		return;
	}
	auto exChangeCodeForTokensRequest = httpModule_->CreateRequest();
	exChangeCodeForTokensRequest->OnProcessRequestComplete().BindUObject(this, &UMainMenuWidget::OnExchangeCodeForTokensResponseReceived);
	exChangeCodeForTokensRequest->SetURL(apiUrl_ + "/exchangecodefortokens");
	exChangeCodeForTokensRequest->SetVerb("POST");
	exChangeCodeForTokensRequest->SetHeader("Content-Type", "application/json");
	exChangeCodeForTokensRequest->SetContentAsString(requestBody);
	exChangeCodeForTokensRequest->ProcessRequest();
	UE_LOG(LogTemp, Log, TEXT("UMainMenuWidget:HandleLoginUrlChange end"));
}

void UMainMenuWidget::SetAveragePlayerLatency()
{
	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance == nullptr)
	{
		check(!"gameInstanceがnullptr");
		return;
	}

	UUnrealFpsGameInstance* fpsGameInstance = Cast<UUnrealFpsGameInstance>(gameInstance);

	if (fpsGameInstance == nullptr)
	{
		check(!"fpsGameInstanceがnullptr");
		return;
	}
	float totalPlayerLatency = 0.0f;
	const auto& playerLatencyList = fpsGameInstance->GetPlayerLatencies();
	if (playerLatencyList.Num() <= 0)
	{
		// playerLatencyListSizeが0なら計算も何もないのでreturn
		return;
	}
	for (float playerLatency : playerLatencyList)
	{
		totalPlayerLatency += playerLatency;
	}

	if (totalPlayerLatency < 0.0f)
	{
		check(!"totalPlayerLatencyが0");
		return;
	}

	averagePlayerLatency_ = totalPlayerLatency / playerLatencyList.Num();

	FString pingString = "Ping: " + FString::FromInt(FMath::RoundToInt(averagePlayerLatency_)) + "ms";
	pingTextBlock_->SetText(FText::FromString(pingString));
}

void UMainMenuWidget::OnMatchmakingButtonClicked()
{
	matchmakingButton_->SetIsEnabled(false);

	UGameInstance* gameInstance = GetGameInstance();

	if (gameInstance == nullptr)
	{
		check(!"gameInstanceがnullptr");
		return;
	}

	UUnrealFpsGameInstance* fpsGameInstance = Cast<UUnrealFpsGameInstance>(gameInstance);

	if (fpsGameInstance == nullptr)
	{
		check(!"fpsGameInstanceがnullptr");
		return;
	}

	const FString& accessToken = fpsGameInstance->GetAccessToken();
	const FString& matchmakingTicketID = fpsGameInstance->GetMatchmakingTicketID();

	if (searchingForGame_)
	{
		searchingForGame_ = false;
		if (SendStopMatchmakingRequest(accessToken, matchmakingTicketID))
		{
			return;
		}
		// 送信に失敗したときは待機状態に戻す
		UTextBlock* buttonTextBlock = Cast<UTextBlock>(matchmakingButton_->GetChildAt(0));
		buttonTextBlock->SetText(FText::FromString("Join Game"));
		matchmakingEventTextBlock_->SetText(FText::FromString(""));

		matchmakingButton_->SetIsEnabled(true);
	}
	else
	{
		searchingForGame_ = true;
		if (SendStartMatchmakingRequest(accessToken))
		{
			return;
		}

		UTextBlock* buttonTextBlock = Cast<UTextBlock>(matchmakingButton_->GetChildAt(0));
		buttonTextBlock->SetText(FText::FromString("Cancel Matchmaking"));
		matchmakingEventTextBlock_->SetText(FText::FromString("Currently looking for a match"));

		matchmakingButton_->SetIsEnabled(true);
	}
}

UUnrealFpsGameInstance* UMainMenuWidget::GetFpsGameInstance()
{
	UGameInstance* gameInstance = GetGameInstance();

	if (gameInstance == nullptr)
	{
		check(!"gameInstanceがnullptr");
		return nullptr;
	}

	UUnrealFpsGameInstance* fpsGameInstance = Cast<UUnrealFpsGameInstance>(gameInstance);

	if (fpsGameInstance == nullptr)
	{
		check(!"UUnrealFpsGameInstanceへのキャスト失敗");
		return nullptr;
	}
	return fpsGameInstance;
}

bool UMainMenuWidget::SendStopMatchmakingRequest(const FString& accessToken, const FString& matchmakingTicketId)
{
	if (accessToken.Len() < 0 && matchmakingTicketId.Len() < 0)
	{
		check(!"tokenが存在しない");
		return false;
	}

	TSharedPtr<FJsonObject> requestObject = MakeShareable(new FJsonObject);
	requestObject->SetStringField("ticketId", matchmakingTicketId);

	FString requestBody;

	auto writer = TJsonWriterFactory<>::Create(&requestBody);

	if (!FJsonSerializer::Serialize(requestObject.ToSharedRef(), writer))
	{
		check(!"Serialize失敗");
		return false;
	}

	auto stopMatchmakingRequest = httpModule_->CreateRequest();
	stopMatchmakingRequest->OnProcessRequestComplete().BindUObject(this, &UMainMenuWidget::OnStopMatchmakingResponseReceived);
	stopMatchmakingRequest->SetURL(apiUrl_ + "/stopmatchmaking");
	stopMatchmakingRequest->SetVerb("POST");
	stopMatchmakingRequest->SetHeader("Content-Type", "application/json");
	stopMatchmakingRequest->SetHeader("Authorization", accessToken);
	stopMatchmakingRequest->SetContentAsString(requestBody);
	stopMatchmakingRequest->ProcessRequest();

	return true;
}

bool UMainMenuWidget::SendStartMatchmakingRequest(const FString& accessToken)
{
	if (accessToken.Len() < 0)
	{
		check(!"tokenが存在しない");
		return false;
	}

	TSharedPtr<FJsonObject> latencyMapObject = MakeShareable(new FJsonObject);

	latencyMapObject->SetNumberField(regionCode_, averagePlayerLatency_);

	TSharedPtr<FJsonObject> requestObject = MakeShareable(new FJsonObject);
	requestObject->SetObjectField("latencyMap", latencyMapObject);

	FString requestBody;

	auto writer = TJsonWriterFactory<>::Create(&requestBody);

	if (!FJsonSerializer::Serialize(requestObject.ToSharedRef(), writer))
	{
		check(!"Serialize失敗");
		return false;
	}

	auto startMatchmakingRequest = httpModule_->CreateRequest();
	startMatchmakingRequest->OnProcessRequestComplete().BindUObject(this, &UMainMenuWidget::OnStartMatchmakingResponseReceived);
	startMatchmakingRequest->SetURL(apiUrl_ + "/startmatchmaking");
	startMatchmakingRequest->SetVerb("POST");
	startMatchmakingRequest->SetHeader("Content-Type", "application/json");
	startMatchmakingRequest->SetHeader("Authorization", accessToken);
	startMatchmakingRequest->SetContentAsString(requestBody);
	startMatchmakingRequest->ProcessRequest();

	return true;
}



void UMainMenuWidget::PlayerDataReques(const FString& accessToken)
{
	auto getPlayerDataRequest = httpModule_->CreateRequest();
	getPlayerDataRequest->OnProcessRequestComplete().BindUObject(this, &UMainMenuWidget::OnGetPlayerDataResponseReceived);
	getPlayerDataRequest->SetURL(apiUrl_ + "/getplayerdata");
	getPlayerDataRequest->SetVerb("GET");
	getPlayerDataRequest->SetHeader("Authorization", accessToken);
	getPlayerDataRequest->ProcessRequest();
}

void UMainMenuWidget::OnExchangeCodeForTokensResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull)
{
	if (!bWasSuccessfull)
	{
		check(!"OnExchangeCodeForTokensResponseReceived is not bWasSuccessfull");
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("UMainMenuWidget:OnExchangeCodeForTokensResponseReceived"));

	auto checkJson = CheckJsonError(response);

	if (checkJson.first)
	{
		check(!"error");
		return;
	}
	TSharedPtr<FJsonObject> jsonObject = checkJson.second;

	if (!jsonObject->HasField("access_token") || !jsonObject->HasField("id_token") || !jsonObject->HasField("refresh_token"))
	{
		check(!"OnExchangeCodeForTokensResponseReceivedでの受信エラー");
		return;
	}

	UUnrealFpsGameInstance* fpsGameInstance = GetFpsGameInstance();

	FString accessToken = jsonObject->GetStringField("access_token");
	FString idToken = jsonObject->GetStringField("id_token");
	FString refreshtoken = jsonObject->GetStringField("refresh_token");

	fpsGameInstance->SetCognitoTokens(accessToken, idToken, refreshtoken);

	PlayerDataReques(accessToken);

	UE_LOG(LogTemp, Log, TEXT("UMainMenuWidget:OnExchangeCodeForTokensResponseReceived end"));
}

void UMainMenuWidget::OnGetPlayerDataResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull)
{
	if (!bWasSuccessfull)
	{
		check(!"OnGetPlayerDataResponseReceived is not bWasSuccessfull");
		return;
	}
	auto checkJson = CheckJsonError(response);

	if (checkJson.first)
	{
		check(!"error");
		return;
	}

	TSharedPtr<FJsonObject> jsonObject = checkJson.second;

	if (!jsonObject->HasField("playerData"))
	{
		check(!"p;ayerDataではない");
		return;
	}

	TSharedPtr<FJsonObject> playerData = jsonObject->GetObjectField("playerData");
	TSharedPtr<FJsonObject> winsData = playerData->GetObjectField("Wins");
	TSharedPtr<FJsonObject> lossesData = playerData->GetObjectField("Losses");

	FString wins = winsData->GetStringField("N");
	FString losses = lossesData->GetStringField("N");

	UE_LOG(LogTemp, Warning, TEXT("%s"), *wins);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *losses);

	winsTextBlock_->SetText(FText::FromString("Wins: " + wins));
	lossesTextBlock_->SetText(FText::FromString("Losses: " + losses));

	// Widgetの表示の変更
	webBrowser_->SetVisibility(ESlateVisibility::Hidden);
	matchmakingButton_->SetVisibility(ESlateVisibility::Visible);
	winsTextBlock_->SetVisibility(ESlateVisibility::Visible);
	lossesTextBlock_->SetVisibility(ESlateVisibility::Visible);
	pingTextBlock_->SetVisibility(ESlateVisibility::Visible);
	matchmakingEventTextBlock_->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenuWidget::OnStartMatchmakingResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull)
{
	matchmakingButton_->SetIsEnabled(true);
	if (!bWasSuccessfull)
	{
		check(!"OnStartMatchmakingResponseReceived is not bWasSuccessfull");
		return;
	}
	auto checkJson = CheckJsonError(response);

	if (checkJson.first)
	{
		check(!"error");
		return;
	}

	TSharedPtr<FJsonObject> jsonObject = checkJson.second;

	if (!jsonObject->HasField("ticketId"))
	{
		check(!"ticketIdが存在しない");
		return;
	}

	FString matchmakingTicketId = jsonObject->GetStringField("ticketId");

	UUnrealFpsGameInstance* fpsGameInstance = GetFpsGameInstance();

	fpsGameInstance->SetMatchmakingTicketID(matchmakingTicketId);

	searchingForGame_ = true;

	UTextBlock* buttonTextBlock = Cast<UTextBlock>(matchmakingButton_->GetChildAt(0));
	buttonTextBlock->SetText(FText::FromString("Cancel Matchmaking"));
	matchmakingEventTextBlock_->SetText(FText::FromString("Currently looking for a match"));

}

void UMainMenuWidget::OnStopMatchmakingResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull)
{

	UTextBlock* buttonTextBlock = Cast<UTextBlock>(matchmakingButton_->GetChildAt(0));
	buttonTextBlock->SetText(FText::FromString("Join Game"));
	matchmakingEventTextBlock_->SetText(FText::FromString(""));

	matchmakingButton_->SetIsEnabled(true);
	if (!bWasSuccessfull)
	{
		check(!"OnStartMatchmakingResponseReceived is not bWasSuccessfull");
		return;
	}
	auto checkJson = CheckJsonError(response);

	if (checkJson.first)
	{
		check(!"error");
		return;
	}

	TSharedPtr<FJsonObject> jsonObject = checkJson.second;

	if (!jsonObject->HasField("success"))
	{
		check(!"not success");
		return;
	}

	UUnrealFpsGameInstance* fpsGameInstance = GetFpsGameInstance();

	fpsGameInstance->SetMatchmakingTicketID("");
}
