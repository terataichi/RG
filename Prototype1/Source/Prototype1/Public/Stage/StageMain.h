// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include <memory>
#include <utility>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StageSpaceState.h"
#include "StageMain.generated.h"

UCLASS()
class PROTOTYPE1_API AStageMain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStageMain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY(EditAnywhere)
	float rayLength_;																	// レイの長さ

	UPROPERTY(EditInstanceOnly)
	FVector SpaceSize_;

	UPROPERTY(EditAnywhere)
	int divisionNumMAX_;																// 分割数

	UPROPERTY(EditAnywhere)
	float dbgTime_;																		// デバッグ用

	FVector centerPos_;																	// モデルの中心座標
	FVector size_;																		// モデル全体の大きさ
	FVector divSize_;																	// 1分割分の大きさ


	std::vector<std::pair<StageSpaceState, AActor*>> spaceState_;						// マスのステータスと配置オブジェクト

	// 仮
	FString path_;
	TSubclassOf<AActor> subClass_;
};
