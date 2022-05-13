// Fill out your copyright notice in the Description page of Project Settings.

#include "Stage/StageMain.h"
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

	spaceState_.resize(divSize_.X * divSize_.Y);

	int i = 0;
	for (auto& state : spaceState_)
	{
		if (i % 3 == 0)
		{
			state = StageSpaceState::None;
		}
		else if (i % 3 == 1)
		{
			state = StageSpaceState::NotPut;
		}		
		else if (i % 3 == 2)
		{
			state = StageSpaceState::Put;
		}
		i++;
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
	if (result.Actor.Get() == this)
	{
		// マスの特定(原点が中心なのでずらす)
		int X = static_cast<int>((result.ImpactPoint.X + size_.X / 2.0f) / divSize_.X);
		int Y = static_cast<int>((result.ImpactPoint.Y + size_.Y / 2.0f) / divSize_.Y);

		// 範囲外参照を避けるためクランプ
		X = FMath::Clamp(X, 0, divisionNumMAX_ - 1);
		Y = FMath::Clamp(Y, 0, divisionNumMAX_ - 1);

		//GEngine->AddOnScreenDebugMessage(-1, dbgTime_, FColor::Red, FVector2D(X, Y).ToString());
		//GEngine->AddOnScreenDebugMessage(-1, dbgTime_, FColor::Red, result.ImpactPoint.ToString());
		//GEngine->AddOnScreenDebugMessage(-1, dbgTime_, FColor::Red, divSize_.ToString());

		FVector point = 
		{
			X * divSize_.X + divSize_.X / 2.0f - size_.X / 2.0f,
			Y * divSize_.Y + divSize_.Y / 2.0f - size_.Y / 2.0f,
			size_.Z
		};

		int num = X * Y;

		FLinearColor col = FLinearColor::Red;

		if (spaceState_[num] == StageSpaceState::NotPut)
		{
			col = FLinearColor::Blue;
		}			
		if (spaceState_[num] == StageSpaceState::Put)
		{
			col = FLinearColor::Yellow;
		}

		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), point, 100.0f, 12, col, 0.0f, 3.0f);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), result.ImpactPoint, 30.0f, 12, col, 0.0f, 3.0f);
		UKismetSystemLibrary::DrawDebugBox(GetWorld(), point, divSize_ / 2.0f, col);
	}

	
}

