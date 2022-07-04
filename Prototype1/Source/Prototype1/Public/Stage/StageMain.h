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

	/// <summary>
	/// マスを特定する
	/// </summary>
	/// <param name="impactPoint">置きたいマス</param>
	UFUNCTION(BlueprintCallable, Category = "StageF")
	void DrawSpace(const int32& spaceNum);

	// 設置命令
	// あとでアイテムリストももらうようにする
	UFUNCTION(BlueprintCallable, Category = "StageF")
	void Put(const int32& spaceNum);
	// 撤去命令
	UFUNCTION(BlueprintCallable, Category = "StageF")
	void Take(const int32& spaceNum);

	// 撤去命令
	UFUNCTION(BlueprintCallable, Category = "StageF")
	int32 GetSpaceNum(const FVector& impactPoint);

	//// プレイヤーの追加
	//// 引数に保存したい変数を渡す
	//UFUNCTION(BlueprintCallable, Category = "StageF")
	//int32 AddPlayer(UPARAM(ref) int32& myID);

private:
	// マス目を縦と横何マスか調べる
	FVector2D SpaceToXY(const int32& spaceNum);

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 1,ClampMax = 10000))
	float rayLength_;																	// レイの長さ

	UPROPERTY(EditAnywhere,Meta = (AllowPreserveRatio, ClampMin = 1, ClampMax = 10000))
	FVector SpaceSize_;																	// １マスのサイズ

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.01, ClampMax = 5))
	float dbgTime_;																		// デバッグ用

	FVector divisionNumMAX_;															// 分割数

	FVector centerPos_;																	// モデルの中心座標
	FVector size_;																		// モデル全体の大きさ
	FVector divSize_;																	// 1分割分の大きさ

	//std::vector<std::pair<int32, FVector>> playersNum_;								// プレイヤーごとにほしい情報{マスの番号、設置する場合の座標}

	std::vector<std::pair<StageSpaceState, AActor*>> spaceState_;						// マスのステータスと配置オブジェクト

	// 仮
	FString path_;
	TSubclassOf<AActor> subClass_;
};
