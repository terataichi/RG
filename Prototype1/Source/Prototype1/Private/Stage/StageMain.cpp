// Fill out your copyright notice in the Description page of Project Settings.

#include "Stage/StageMain.h"
#include "StageObject/TestObj.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


// Sets default values
AStageMain::AStageMain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Stage Create"));

	dbgTime_ = 0.01f;

}

// Called when the game starts or when spawned
void AStageMain::BeginPlay()
{
	Super::BeginPlay();

	// 中心座標と大きさの取得
	this->GetActorBounds(true, centerPos_, size_);

	// サイズは半分になってるので二倍にする
	size_ *= 2.0f;

	// 分割した大きさの割り出し
	divSize_ = size_ / static_cast<float>(divisionNumMAX_);

	spaceState_.resize(divisionNumMAX_ * divisionNumMAX_);
	for (auto& state : spaceState_)
	{
		state.first = StageSpaceState::NotPut;
	}

}

// Called every frame
void AStageMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// プレイヤーコントローラーの取得
	APlayerController* playerCtl = UGameplayStatics::GetPlayerController(this, 0);

	// マウスポインターの３Ｄ位置と向きの取得
	FVector start, dir;
	playerCtl->DeprojectMousePositionToWorld(start, dir);

	// レイを飛ばす
	FHitResult result;
	GetWorld()->LineTraceSingleByChannel(result, start, start + dir * rayLength_, ECollisionChannel::ECC_Visibility);

	// 自分のモデルに当たってるか
	if (result.Actor.Get() != this)
	{
		return;
	}

	// マスの特定(原点が中心なのでずらす)
	int X = static_cast<int>((result.ImpactPoint.X + size_.X / 2.0f) / divSize_.X);
	int Y = static_cast<int>((result.ImpactPoint.Y + size_.Y / 2.0f) / divSize_.Y);

	// 範囲外参照を避けるためクランプ
	X = FMath::Clamp(X, 0, divisionNumMAX_ - 1);
	Y = FMath::Clamp(Y, 0, divisionNumMAX_ - 1);

	// 配列の番号
	int num = X * Y;

	FVector point = 
	{
		X * divSize_.X + divSize_.X / 2.0f - size_.X / 2.0f,
		Y * divSize_.Y + divSize_.Y / 2.0f - size_.Y / 2.0f,
		size_.Z
	};


	// デバッグ用ガイド表示
	FLinearColor col = FLinearColor::Red;
	if (spaceState_[num].first == StageSpaceState::NotPut)
	{
		col = FLinearColor::Blue;
	}			
	if (spaceState_[num].first == StageSpaceState::Put)
	{
		col = FLinearColor::Green;
	}

	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), result.ImpactPoint, 30.0f, 12, col, 0.0f, 3.0f);
	UKismetSystemLibrary::DrawDebugBox(GetWorld(), point, divSize_ / 2.0f, col);

	//if (sc != nullptr)
	//{
	//	AActor* a = GetWorld()->SpawnActor<AActor>(sc); // スポーン処理
	//	a->SetActorLocation(FVector(-600, 200, 200)); // 確認しやすいように座標を設定
	//}

	// クリックされたらステータス変更
	if (spaceState_[num].first != StageSpaceState::None)
	{
		if (playerCtl->WasInputKeyJustPressed(EKeys::LeftMouseButton))
		{
			FString path = "/Game/Stage/BP/StageObj/MyTestObj.MyTestObj_C"; // /Content 以下のパスが /Game 以下のパスに置き換わり、コンテントブラウザーで名前が test なら test.test_C を指定する。
			TSubclassOf<class AActor> sc = TSoftClassPtr<AActor>(FSoftObjectPath(*path)).LoadSynchronous(); // 上記で設定したパスに該当するクラスを取得
			if (sc != nullptr)
			{
				spaceState_[num].first = StageSpaceState::Put;
				//spaceState_[num].second = GetWorld()->SpawnActor<ATestObj>();
				spaceState_[num].second = GetWorld()->SpawnActor<AActor>(sc);
				spaceState_[num].second->SetActorLocation(point);

				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, point.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, spaceState_[num].second->GetActorLocation().ToString());
			}
		}
		if (playerCtl->WasInputKeyJustPressed(EKeys::RightMouseButton))
		{
			spaceState_[num].first = StageSpaceState::NotPut;
			delete spaceState_[num].second;
		}
	}


}

