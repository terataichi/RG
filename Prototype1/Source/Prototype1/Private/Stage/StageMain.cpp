// Fill out your copyright notice in the Description page of Project Settings.

#include "Stage/StageMain.h"
#include "StageObject/TestObj.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"


// Sets default values
AStageMain::AStageMain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Stage Create"));

	dbgTime_ = 0.01f;

}

// Called when the game starts or when spawned
void AStageMain::BeginPlay()
{
	Super::BeginPlay();

	// ���S���W�Ƒ傫���̎擾
	this->GetActorBounds(true, centerPos_, size_);

	// �T�C�Y�͔����ɂȂ��Ă�̂œ�{�ɂ���
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
}

// Called every frame
void AStageMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStageMain::SpaceSpecific(const FVector& impactPoint,bool leftClick, bool rightClick)
{
	// �}�X�̓���(���_�����S�Ȃ̂ł��炷)
	int X = static_cast<int>((impactPoint.X + size_.X / 2.0f) / divSize_.X);
	int Y = static_cast<int>((impactPoint.Y + size_.Y / 2.0f) / divSize_.Y);

	// �͈͊O�Q�Ƃ�����邽�߃N�����v
	X = FMath::Clamp(X, 0, static_cast<int>(divisionNumMAX_.X) - 1);
	Y = FMath::Clamp(Y, 0, static_cast<int>(divisionNumMAX_.Y) - 1);

	// �z��̔ԍ�
	int num = Y * divisionNumMAX_.X + X;

	FVector point =
	{
		X * divSize_.X + divSize_.X / 2.0f - size_.X / 2.0f,
		Y * divSize_.Y + divSize_.Y / 2.0f - size_.Y / 2.0f,
		impactPoint.Z
	};

	// �f�o�b�O�p�K�C�h�\��
	FLinearColor col = FLinearColor::Red;
	if (spaceState_[num].first == StageSpaceState::NotPut)
	{
		col = FLinearColor::Blue;
	}
	if (spaceState_[num].first == StageSpaceState::Put)
	{
		col = FLinearColor::Green;
	}

	// �f�o�b�O�`��
	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), impactPoint, 30.0f, 12, col, 0.0f, 3.0f);
	UKismetSystemLibrary::DrawDebugBox(GetWorld(), point, divSize_ / 2.0f, col);

	// �N���b�N���ꂽ��X�e�[�^�X�ύX
	if (spaceState_[num].first == StageSpaceState::NotPut && leftClick)
	{
		if (subClass_ != nullptr)
		{
			spaceState_[num].first = StageSpaceState::Put;

			spaceState_[num].second = GetWorld()->SpawnActor<AActor>(subClass_);
			spaceState_[num].second->SetActorLocation(point);
		}
	}
	else if (spaceState_[num].first == StageSpaceState::Put && rightClick)
	{
		if (GetWorld()->DestroyActor(spaceState_[num].second))
		{
			spaceState_[num].first = StageSpaceState::NotPut;
		}
	}
}

