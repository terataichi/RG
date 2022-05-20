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

	divisionNumMAX_ = size_ / SpaceSize_;

	// 分割した大きさの割り出し
	divSize_ = size_ / divisionNumMAX_;

	spaceState_.resize(divisionNumMAX_.X * divisionNumMAX_.Y);
	for (auto& state : spaceState_)
	{
		state.first = StageSpaceState::NotPut;
	}

	path_ = "/Game/Stage/BP/StageObj/MyTestObj.MyTestObj_C";							// /Content 以下のパスが /Game 以下のパスに置き換わり、コンテントブラウザーで名前が test なら test.test_C を指定する。
	subClass_ = TSoftClassPtr<AActor>(FSoftObjectPath(*path_)).LoadSynchronous();		// 上記で設定したパスに該当するクラスを取得
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
	X = FMath::Clamp(X, 0, static_cast<int>(divisionNumMAX_.X) - 1);
	Y = FMath::Clamp(Y, 0, static_cast<int>(divisionNumMAX_.Y) - 1);

	// 配列の番号
	int num = Y * divisionNumMAX_.X + X;

	FVector point = 
	{
		X * divSize_.X + divSize_.X / 2.0f - size_.X / 2.0f,
		Y * divSize_.Y + divSize_.Y / 2.0f - size_.Y / 2.0f,
		result.ImpactPoint.Z
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

	// クリックされたらステータス変更

	if (spaceState_[num].first == StageSpaceState::NotPut &&
		playerCtl->WasInputKeyJustPressed(EKeys::LeftMouseButton))
	{
		if (subClass_ != nullptr)
		{
			spaceState_[num].first = StageSpaceState::Put;
		
			spaceState_[num].second = GetWorld()->SpawnActor<AActor>(subClass_);
			spaceState_[num].second->SetActorLocation(point);

			//spaceState_[num].second->SetActorScale3D(this->GetActorScale3D());
			//spaceState_[num].second->SetActorEnableCollision(false);
		}
	}
	else if (spaceState_[num].first == StageSpaceState::Put && 
		playerCtl->WasInputKeyJustPressed(EKeys::RightMouseButton))
	{
		if (GetWorld()->DestroyActor(spaceState_[num].second))
		{
			spaceState_[num].first = StageSpaceState::NotPut;
		}
	}
}

