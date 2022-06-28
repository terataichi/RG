// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "TextReaderComponent.h"
#include "WebBrowser.h"
#include "WebBrowserModule.h"
#include "IWebBrowserSingleton.h"
#include "IWebBrowserCookieManager.h"

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& objectInitializer):
	UUserWidget(objectInitializer)
{
	UTextReaderComponent* textReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	loginUrl_ = textReader->ReadFile("Urls/LoginUrl.txt");
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
}

void UMainMenuWidget::HandleLoginUrlChange()
{
}

void UMainMenuWidget::OnExchangeCodeForTokensResponseReceived(FHttpRequestPtr request, FHttpRequestPtr response, bool bWasSuccessfull)
{
}
