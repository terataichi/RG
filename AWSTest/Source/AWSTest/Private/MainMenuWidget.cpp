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

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& objectInitializer):
	UUserWidget(objectInitializer)
{
	UTextReaderComponent* textReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	loginUrl_ = textReader->ReadFile("Urls/LoginUrl.txt");

	apiUrl_ = textReader->ReadFile("Urls/ApiUrl.txt");
	callbackUrl_ = textReader->ReadFile("Urls/CallbackUrl.txt");

	httpModule_ = &FHttpModule::Get();
}

void UMainMenuWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();
	bIsFocusable = true;

	IWebBrowserSingleton* webBrowserSinglton = IWebBrowserModule::Get().GetSingleton();

	InitWidgets();
	
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

	UGameInstance* gameInstance = GetGameInstance();

	if (gameInstance == nullptr)
	{
		check(!"gameInstanceがnullptr");
		return;
	}

	UUnrealFpsGameInstance* fpsGameInstance = Cast<UUnrealFpsGameInstance>(gameInstance);

	if (fpsGameInstance == nullptr)
	{
		check(!"UUnrealFpsGameInstanceへのキャスト失敗");
		return;
	}

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

	TSharedPtr<FJsonObject> playerData = jsonObject->GetObjectField("playerData");
	TSharedPtr<FJsonObject> winsData = jsonObject->GetObjectField("Wins");
	TSharedPtr<FJsonObject> lossesData = jsonObject->GetObjectField("Losses");

	FString player = playerData->GetStringField("S");
	FString wins = winsData->GetStringField("N");
	FString losses = lossesData->GetStringField("N");

	UE_LOG(LogTemp, Warning, TEXT("%s"), *wins);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *losses);

	winsTextBlock_->SetText(FText::FromString("Wins: " + wins));
	lossesTextBlock_->SetText(FText::FromString("Losses: " + losses));

	// Widgetの表示の変更
	webBrowser_->SetVisibility(ESlateVisibility::Hidden);
	winsTextBlock_->SetVisibility(ESlateVisibility::Visible);
	lossesTextBlock_->SetVisibility(ESlateVisibility::Visible);
	pingTextBlock_->SetVisibility(ESlateVisibility::Visible);
	matchmakingEventTextBlock_->SetVisibility(ESlateVisibility::Visible);
}
