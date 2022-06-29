// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "TextReaderComponent.h"
#include "WebBrowser.h"
#include "WebBrowserModule.h"
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
	Super::NativeConstruct();
	bIsFocusable = true;

	IWebBrowserSingleton* webBrowserSinglton = IWebBrowserModule::Get().GetSingleton();

	webBrowser_ = static_cast<UWebBrowser*>(GetWidgetFromName(TEXT("WebBrowser_Login")));

	if (webBrowserSinglton == nullptr)
	{
		check(!"webBrowserSinglton‚ªnullptr");
	}
	TOptional<FString> defaultContext;
	TSharedPtr<IWebBrowserCookieManager> cookieManager = webBrowserSinglton->GetCookieManager(defaultContext);
	if (!cookieManager.IsValid())
	{
		check(!"IWebBrowserCookieManager‚ª–³Œø");
	}
	cookieManager->DeleteCookies();

	webBrowser_->LoadURL(loginUrl_);

	FScriptDelegate loginDelegate;
	loginDelegate.BindUFunction(this, "HandleLoginUrlChange");
	webBrowser_->OnUrlChanged.Add(loginDelegate);
}

void UMainMenuWidget::HandleLoginUrlChange()
{
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
}

void UMainMenuWidget::OnExchangeCodeForTokensResponseReceived(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessfull)
{
	if (!bWasSuccessfull)
	{
		return;
	}
	TSharedPtr<FJsonObject> jsonObject;
	auto reader = TJsonReaderFactory<>::Create(response->GetContentAsString());

	if (FJsonSerializer::Deserialize(reader,jsonObject) || jsonObject->HasField("error"))
	{
		return;
	}

	UGameInstance* gameInstance = GetGameInstance();

	if (gameInstance == nullptr)
	{
		check(!"gameInstance‚ªnullptr");
		return;
	}

	UUnrealFpsGameInstance* fpsGameInstance = Cast<UUnrealFpsGameInstance>(gameInstance);

	if (fpsGameInstance == nullptr)
	{
		check(!"UUnrealFpsGameInstance‚Ö‚ÌƒLƒƒƒXƒgŽ¸”s");
		return;
	}

	fpsGameInstance->SetCognitoTokens(
		jsonObject->GetStringField("access_token"),jsonObject->GetStringField("id_token"), jsonObject->GetStringField("refresh_token")
	);
}
