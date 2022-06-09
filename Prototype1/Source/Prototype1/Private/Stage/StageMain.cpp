// Fill out your copyright notice in the Description page of Project Settings.

#include "Stage/StageMain.h"
#include "StageObject/TestObj.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


// Sets default values
AStageMain::AStageMain() :
	rayLength_(10000.0f), dbgTime_(0.01f), SpaceSize_(100.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Stage Create"));
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

	//// プレイヤーコントローラーの取得
	//APlayerController* playerCtl = UGameplayStatics::GetPlayerController(this, 0);

	//if (!playerCtl)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("notPCTL"));
	//	return;
	//}

	//left_ = playerCtl->WasInputKeyJustPressed(EKeys::LeftMouseButton);
	//right_ = playerCtl->WasInputKeyJustPressed(EKeys::RightMouseButton);
}

void AStageMain::SpaceSpecific(const int32& MyID, const FVector& impactPoint)
{
	// ガード処理
	if (!playersNum_.size())
	{
		return;
	}
	if (!(0 <= MyID && MyID <= playersNum_.size() - 1))
	{
		return;
	}

	// マスの特定(原点が中心なのでずらす)
	int X = static_cast<int>((impactPoint.X + size_.X / 2.0f) / divSize_.X);
	int Y = static_cast<int>((impactPoint.Y + size_.Y / 2.0f) / divSize_.Y);

	// 範囲外参照を避けるためクランプ
	X = FMath::Clamp(X, 0, static_cast<int>(divisionNumMAX_.X) - 1);
	Y = FMath::Clamp(Y, 0, static_cast<int>(divisionNumMAX_.Y) - 1);

	// 配列の番号
	playersNum_[MyID].first = Y * divisionNumMAX_.X + X;

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue,FString::FromInt(playersNum_[MyID].first));

	playersNum_[MyID].second =
	{
		X * divSize_.X + divSize_.X / 2.0f - size_.X / 2.0f,
		Y * divSize_.Y + divSize_.Y / 2.0f - size_.Y / 2.0f,
		impactPoint.Z
	};

	// デバッグ用ガイド表示
	FLinearColor col = FLinearColor::Red;
	if (spaceState_[playersNum_[MyID].first].first == StageSpaceState::NotPut)
	{
		col = FLinearColor::Blue;
	}
	if (spaceState_[playersNum_[MyID].first].first == StageSpaceState::Put)
	{
		col = FLinearColor::Green;
	}

	// デバッグ描画
	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), impactPoint, 30.0f, 12, col);
	UKismetSystemLibrary::DrawDebugBox(GetWorld(), playersNum_[MyID].second, divSize_ / 2.0f, col);
}

void AStageMain::Reset(const int32& MyID)
{
	// ガード処理
	if (!playersNum_.size())
	{
		return;
	}
	if (!(0 <= MyID && MyID <= playersNum_.size() - 1))
	{
		return;
	}

	playersNum_[MyID].first = -1;
}

void AStageMain::Put(const int32& MyID)
{
	// ガード処理
	if (!playersNum_.size())
	{
		UE_LOG(LogTemp, Error, TEXT("NotPlayer"));
		return;
	}
	if (!(0 <= MyID && MyID <= playersNum_.size() - 1))
	{
		UE_LOG(LogTemp, Error, TEXT("IDError"));
		return;
	}
	if (playersNum_[MyID].first == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("Out"));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Put"));
	// 何も設置されてなかったら設置する
	if (spaceState_[playersNum_[MyID].first].first == StageSpaceState::NotPut)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("leftPut"));
		if (subClass_ != nullptr)
		{
			spaceState_[playersNum_[MyID].first].first = StageSpaceState::Put;

			spaceState_[playersNum_[MyID].first].second = GetWorld()->SpawnActor<AActor>(subClass_);
			spaceState_[playersNum_[MyID].first].second->SetActorLocation(playersNum_[MyID].second);
		}
	}
}

void AStageMain::Take(const int32& MyID)
{
	// ガード処理
	if (!playersNum_.size())
	{
		return;
	}
	if (!(0 <= MyID && MyID <= playersNum_.size() - 1))
	{
		return;
	}
	if (playersNum_[MyID].first == -1)
	{
		return;
	}

	// 何か設置されていたら削除する
	if (spaceState_[playersNum_[MyID].first].first == StageSpaceState::Put)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("take"));
		if (GetWorld()->DestroyActor(spaceState_[playersNum_[MyID].first].second))
		{
			spaceState_[playersNum_[MyID].first].first = StageSpaceState::NotPut;
		}
	}
}

void AStageMain::AddPlayer(UPARAM(ref) int32& MyID)
{
	playersNum_.push_back({ -1, {} });
	MyID = playersNum_.size() - 1;
	UE_LOG(LogTemp, Display, TEXT("AddPlayer : %d"), MyID);
}

