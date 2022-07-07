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

	// ���S���W�Ƒ傫���̎擾
	this->GetActorBounds(true, centerPos_, size_);

	// ��������̃T�C�Y�ɂȂ��Ă�̂őS�̂̃T�C�Y�͂Q�{
	size_ *= 2.0f;

	divisionNumMAX_ = size_ / SpaceSize_;

	// ���������傫���̊���o��
	divSize_ = size_ / divisionNumMAX_;

	spaceState_.resize(divisionNumMAX_.X * divisionNumMAX_.Y);
	for (auto& state : spaceState_)
	{
		state.first = StageSpaceState::NotPut;
	}

	path_ = "/Game/Stage/BP/StageObj/MyTestObj.MyTestObj_C";							// /Content �ȉ��̃p�X�� /Game �ȉ��̃p�X�ɒu�������A�R���e���g�u���E�U�[�Ŗ��O�� test �Ȃ� test.test_C ���w�肷��B
	subClass_ = TSoftClassPtr<AActor>(FSoftObjectPath(*path_)).LoadSynchronous();		// ��L�Őݒ肵���p�X�ɊY������N���X���擾

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

	// �f�o�b�O�p�K�C�h�\��
	FLinearColor col = FLinearColor::Red;
	if (spaceState_[spaceNum].first == StageSpaceState::NotPut)
	{
		col = FLinearColor::Blue;
	}
	if (spaceState_[spaceNum].first == StageSpaceState::Put)
	{
		col = FLinearColor::Green;
	}

	// �f�o�b�O�`��
	//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), impactPoint, 30.0f, 12, col);
	UKismetSystemLibrary::DrawDebugBox(GetWorld(), drawPos + this->GetActorLocation(), divSize_ / 2.0f, col);
}

int32 AStageMain::GetSpaceNum(const FVector& impactPoint)
{
	auto location = this->GetActorLocation();
	// �}�X�̓���(���_�����S�Ȃ̂ł��炷)
	int X = static_cast<int>((impactPoint.X - location.X+ size_.X / 2.0f) / divSize_.X);
	int Y = static_cast<int>((impactPoint.Y - location.Y+ size_.Y / 2.0f) / divSize_.Y);

	// �͈͊O�Q�Ƃ�����邽�߃N�����v
	X = FMath::Clamp(X, 0, static_cast<int>(divisionNumMAX_.X) - 1);
	Y = FMath::Clamp(Y, 0, static_cast<int>(divisionNumMAX_.Y) - 1);

	// �}�X�ڂ̌v�Z
	int32 num = Y * divisionNumMAX_.X + X;

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::FromInt(num));

	return  num;
}

FVector2D AStageMain::SpaceToXY(const int32& spaceNum)
{
	// �ԍ����}�X�ڂɕϊ�
	int X = spaceNum % static_cast<int>(divisionNumMAX_.Y);
	int Y = spaceNum / static_cast<int>(divisionNumMAX_.Y);

	// �͈͊O�Q�Ƃ�����邽�߃N�����v
	X = FMath::Clamp(X, 0, static_cast<int>(divisionNumMAX_.X) - 1);
	Y = FMath::Clamp(Y, 0, static_cast<int>(divisionNumMAX_.Y) - 1);

	return FVector2D(X,Y);
}

bool AStageMain::Put(const int32& spaceNum, const AActor* Obj)
{
	// �K�[�h����
	if (spaceNum == -1)
	{
		return false;
	}
	// �����ݒu����ĂȂ�������ݒu����
	if (spaceState_[spaceNum].first != StageSpaceState::NotPut)
	{
		return false;
	}

	if (subClass_ == nullptr)
	{
		return false;
	}

	// �ԍ����}�X�ڂɕϊ�
	auto num = SpaceToXY(spaceNum);

	auto location = this->GetActorLocation();

	// �ݒu�ꏊ
	FVector putPos =
	{
		location.X + (num.X * divSize_.X + divSize_.X / 2.0f - size_.X / 2.0f),
		location.Y + (num.Y * divSize_.Y + divSize_.Y / 2.0f - size_.Y / 2.0f),
		centerPos_.Z + size_.Z
	};
	spaceState_[spaceNum].first = StageSpaceState::Put;

	spaceState_[spaceNum].second = GetWorld()->SpawnActor<AActor>(subClass_);
	spaceState_[spaceNum].second->SetActorLocation(putPos);

	UE_LOG(LogTemp, Display, TEXT("Put"));
	return true;
}

AActor* AStageMain::Take(const int32& spaceNum)
{
	// �K�[�h����
	if (spaceNum != -1 && spaceState_[spaceNum].first != StageSpaceState::Put)
	{
		// �����ݒu����Ă�����폜����
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("take"));
		spaceState_[spaceNum].first = StageSpaceState::NotPut;
		return spaceState_[spaceNum].second;
	}
	return nullptr;
}

