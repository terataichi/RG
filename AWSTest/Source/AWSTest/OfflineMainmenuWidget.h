// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Http.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OfflineMainmenuWidget.generated.h"


/**
 * 
 */
UCLASS()
class AWSTEST_API UOfflineMainmenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UOfflineMainmenuWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
		void OnLoginClicked();

	UPROPERTY(EditAnywhere)
		FString apiGatewayEndPoint_;

	UPROPERTY(EditAnywhere)
		FString logInURI_;

	UPROPERTY(EditAnywhere)
		FString stratSessionURI_;

	UPROPERTY(BluePrintReadWrite)
		FString user_;

	UPROPERTY(BluePrintReadWrite)
		FString pass_;

private:
	FHttpModule* http_;

	/// <summary>
	/// ���O�C���̃��N�G�X�g
	/// �n���ꂽstring����
	/// JsonObject���쐬����
	/// </summary>
	/// <param name="usr">username</param>
	/// <param name="pwd">password</param>
	void LogInRequest(FString usr, FString pwd);

	/// <summary>
	/// ���O�C���������擾�����セ�̉�������������
	/// </summary>
	/// <param name="Request"></param>
	/// <param name="Response"></param>
	/// <param name="bWesSuccessful"></param>
	void OnLoginResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool bWesSuccessful);

	/// <summary>
	/// �Z�b�V�����J�n
	/// </summary>
	/// <param name="idt">id�g�[�N��</param>
	void StartSessionRequest(FString idt);

	/// <summary>
	/// �J�n�Z�b�V��������̉�������������
	/// </summary>
	/// <param name="request"></param>
	/// <param name="response"></param>
	/// <param name="bWasSuccessful"></param>
	void OnStartSessionResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful);

};
