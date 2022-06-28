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

	// 中央からのサイズになってるので全体のサイズは２倍
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

	GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, TEXT("Stage Init"));
}

// Called every frame
void AStageMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStageMain::DrawSpace(const int32& spaceNum)
{
	auto num = SpaceToXY(spaceNum);

	FVector drawPos =
	{
		num.X * divSize_.X + divSize_.X / 2.0f - size_.X / 2.0f,
		num.Y * divSize_.Y + divSize_.Y / 2.0f - size_.Y / 2.0f,
		centerPos_.Z + size_.Z
	};

	// デバッグ用ガイド表示
	FLinearColor col = FLinearColor::Red;
	if (spaceState_[spaceNum].first == StageSpaceState::NotPut)
	{
		col = FLinearColor::Blue;
	}
	if (spaceState_[spaceNum].first == StageSpaceState::Put)
	{
		col = FLinearColor::Green;
	}

	// デバッグ描画
	//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), impactPoint, 30.0f, 12, col);
	UKismetSystemLibrary::DrawDebugBox(GetWorld(), drawPos + this->GetActorLocation(), divSize_ / 2.0f, col);
}

int32 AStageMain::GetSpaceNum(const FVector& impactPoint)
{
	auto location = this->GetActorLocation();
	// マスの特定(原点が中心なのでずらす)
	int X = static_cast<int>((impactPoint.X - location.X+ size_.X / 2.0f) / divSize_.X);
	int Y = static_cast<int>((impactPoint.Y - location.Y+ size_.Y / 2.0f) / divSize_.Y);

	// 範囲外参照を避けるためクランプ
	X = FMath::Clamp(X, 0, static_cast<int>(divisionNumMAX_.X) - 1);
	Y = FMath::Clamp(Y, 0, static_cast<int>(divisionNumMAX_.Y) - 1);

	// マス目の計算
	int32 num = Y * divisionNumMAX_.X + X;

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::FromInt(num));

	return  num;
}

FVector2D AStageMain::SpaceToXY(const int32& spaceNum)
{
	// 番号をマス目に変換
	int X = spaceNum % static_cast<int>(divisionNumMAX_.Y);
	int Y = spaceNum / static_cast<int>(divisionNumMAX_.Y);

	// 範囲外参照を避けるためクランプ
	X = FMath::Clamp(X, 0, static_cast<int>(divisionNumMAX_.X) - 1);
	Y = FMath::Clamp(Y, 0, static_cast<int>(divisionNumMAX_.Y) - 1);

	return FVector2D(X,Y);
}

void AStageMain::Reset(const int32& myID)
{
	// ガード処理
	//if (!playersNum_.size())
	//{
	//	return;
	//}
	//if (!(0 <= myID && myID <= playersNum_.size() - 1))
	//{
	//	return;
	//}

	//playersNum_[myID].first = -1;
}

void AStageMain::Put(const int32& spaceNum)
{
	// ガード処理
	if (spaceNum == -1)
	{
		return;
	}
	// 何も設置されてなかったら設置する
	if (spaceState_[spaceNum].first != StageSpaceState::NotPut)
	{
		return;
	}

	if (subClass_ == nullptr)
	{
		return;
	}

	// 番号をマス目に変換
	auto num = SpaceToXY(spaceNum);

	// 中心座標と大きさの取得
	this->GetActorBounds(true, centerPos_, size_);

	// 設置場所
	FVector putPos =
	{
		num.X * divSize_.X + divSize_.X / 2.0f - size_.X / 2.0f,
		num.Y * divSize_.Y + divSize_.Y / 2.0f - size_.Y / 2.0f,
		centerPos_.Z + size_.Z
	};
	spaceState_[spaceNum].first = StageSpaceState::Put;

	spaceState_[spaceNum].second = GetWorld()->SpawnActor<AActor>(subClass_);
	spaceState_[spaceNum].second->SetActorLocation(putPos);

	UE_LOG(LogTemp, Display, TEXT("Put"));
}

void AStageMain::Take(const int32& spaceNum)
{
	// ガード処理
	if (spaceNum == -1)
	{
		return;
	}
	// 置かれているか確認
	if (spaceState_[spaceNum].first != StageSpaceState::Put)
	{
		return;
	}

	// 何か設置されていたら削除する
	if (GetWorld()->DestroyActor(spaceState_[spaceNum].second))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("take"));
		spaceState_[spaceNum].first = StageSpaceState::NotPut;
	}

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("take"));
	
}

//int32 AStageMain::AddPlayer(UPARAM(ref) int32& myID)
//{
//	playersNum_.push_back({ -1, {} });
//
//	myID = playersNum_.size() - 1;
//	int32 id = myID;
//
//	UE_LOG(LogTemp, Display, TEXT("AddPlayer : %d"), id);
//	return id;
//}

