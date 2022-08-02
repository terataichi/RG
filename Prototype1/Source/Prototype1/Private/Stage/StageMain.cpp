// Fill out your copyright notice in the Description page of Project Settings.

#include "Stage/StageMain.h"
#include "StageObject/TestObj.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


// Sets default values
AStageMain::AStageMain() :
	rayLength_(10000.0f), SpaceSize_(100.0f), dbgTime_(0.01f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Stage Create"));
}

// Called when the game starts or when spawned
void AStageMain::BeginPlay()
{
	Super::BeginPlay();

	//Init({ 100.0f,100.0f,1.0f });
}

// Called every frame
void AStageMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStageMain::Init(FVector spaceSize)
{
	SpaceSize_ = spaceSize;

	if ((SpaceSize_.X == 0.0f) || 
		(SpaceSize_.Y == 0.0f) ||
		(SpaceSize_.Z == 0.0f))
	{
		return;
	}

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

	GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, TEXT("Stage Init"));

	MapInit();
}

void AStageMain::MapInit()
{
	//"/Game/Stage/BP/StageObj/MyTestObj.MyTestObj_C"
	previewMap_.Reset();
	FString str = "/Game/Stage/BP/StageObj/StageObjCopy/";
	previewMap_.Add("RockBP_C", str + "RockBPP.RockBPP_C");
	previewMap_.Add("NewPortalBP_C", str + "NewPortalBPP.NewPortalBPP_C");
	previewMap_.Add("accelerationBP_C", str + "accelerationBPP.accelerationBPP_C");
	previewMap_.Add("JumpPadBP_C", str + "JumpPadBPP.JumpPadBPP_C");
	previewMap_.Add("StairBP_C", str + "StairBPP.StairBPP_C");
	previewMap_.Add("StepBP_C", str + "StepBPP.StepBPP_C");
	previewMap_.Add("WallBP_C", str + "WallBPP.WallBPP_C");
	previewMap_.Add("HealSpotBP_C", str + "HealSpotBPP.HealSpotBPP_C");
}

void AStageMain::DrawSpace(const int32& spaceNum, AActor* Obj, const float& z)
{
	if (spaceState_[spaceNum].first == StageSpaceState::Put)
	{
		return;
	}
	if (Obj == nullptr)
	{
		return;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, Obj->GetClass()->GetName());

	// 違うアクターを表示する時に切り替える
	if ((previewObj_ == nullptr) || (previewMap_[Obj->GetClass()->GetName()] != nowPreview_))
	{
		if (previewObj_ != nullptr)
		{
			GetWorld()->DestroyActor(previewObj_);
		} 
		
		if (!previewMap_.Find(Obj->GetClass()->GetName()))
		{
			return;
		}
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("findclear"));

		// アクターの生成
		TSubclassOf<class AActor> sc = TSoftClassPtr<AActor>(FSoftObjectPath(*previewMap_[Obj->GetClass()->GetName()])).LoadSynchronous();
		if (sc == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("nullptr"));
			return;
		}

		nowPreview_ = previewMap_[Obj->GetClass()->GetName()];
		previewObj_ = GetWorld()->SpawnActor(sc);
		previewObj_->SetActorLocation(Obj->GetActorLocation());
		previewObj_->SetActorRotation(Obj->GetActorRotation());
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("CreatePreview"));
	}
	
	auto num = SpaceToXY(spaceNum);

	FVector drawPos =
	{
		num.X * divSize_.X + divSize_.X / 2.0f - size_.X / 2.0f,
		num.Y * divSize_.Y + divSize_.Y / 2.0f - size_.Y / 2.0f,
		z
	};

	// デバッグ用ガイド表示
	//FLinearColor col = FLinearColor::Red;
	//if (spaceState_[spaceNum].first == StageSpaceState::NotPut)
	//{
	//	col = FLinearColor::Blue;
	//}
	//if (spaceState_[spaceNum].first == StageSpaceState::Put)
	//{
	//	col = FLinearColor::Green;
	//}

	// デバッグ描画
	//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), impactPoint, 30.0f, 12, col);
	previewObj_->SetActorLocation(drawPos + this->GetActorLocation());
	previewObj_->SetActorRotation(Obj->GetActorRotation());

	//UKismetSystemLibrary::DrawDebugBox(GetWorld(), drawPos + this->GetActorLocation(), divSize_ / 2.0f, col);
}

void AStageMain::ResetPreview()
{
	if (previewObj_)
	{
		previewObj_->SetActorLocation({ 50000,50000,50000 });
	}
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

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::FromInt(num));

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

bool AStageMain::Put(const int32& spaceNum, AActor* Obj,const float& z)
{
	// ガード処理
	if (spaceNum == -1)
	{
		return false;
	}
	// 何も設置されてなかったら設置する
	if (spaceState_[spaceNum].first != StageSpaceState::NotPut)
	{
		return false;
	}

	if (Obj == nullptr)
	{
		return false;
	}

	// 番号をマス目に変換
	auto num = SpaceToXY(spaceNum);

	auto location = this->GetActorLocation();

	// 設置場所
	FVector putPos =
	{
		location.X + (num.X * divSize_.X + divSize_.X / 2.0f - size_.X / 2.0f),
		location.Y + (num.Y * divSize_.Y + divSize_.Y / 2.0f - size_.Y / 2.0f),
		z
	};
	spaceState_[spaceNum].first = StageSpaceState::Put;

	//auto Component = Obj->GetComponentByClass(USceneComponent::StaticClass());
	//USceneComponent* MyComponent = Cast<USceneComponent>(Component);
	//MyComponent->GetDefaultSceneRootVariableName();
	//MyComponent->SetHiddenInGame(false);

	//Obj->SetHidden(false);
	//Obj->SetActorHiddenInGame(false);
	Obj->SetActorLocation(putPos);
	//Obj->SetActorEnableCollision(true);
	spaceState_[spaceNum].second = Obj;

	UE_LOG(LogTemp, Display, TEXT("Put"));
	return true;
}

AActor* AStageMain::Take(const int32& spaceNum)
{
	// ガード処理
	if (spaceNum != -1 && (spaceState_[spaceNum].first == StageSpaceState::Put))
	{
		// 何か設置されていたら削除する
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("take"));
		spaceState_[spaceNum].first = StageSpaceState::NotPut;

		auto obj = spaceState_[spaceNum].second;
		spaceState_[spaceNum].second = nullptr;
		return obj;
	}

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("nottake"));
	return nullptr;
}

