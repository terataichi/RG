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

	//// �v���C���[�R���g���[���[�̎擾
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
	// �K�[�h����
	if (!playersNum_.size())
	{
		return;
	}
	if (!(0 <= MyID && MyID <= playersNum_.size() - 1))
	{
		return;
	}

	// �}�X�̓���(���_�����S�Ȃ̂ł��炷)
	int X = static_cast<int>((impactPoint.X + size_.X / 2.0f) / divSize_.X);
	int Y = static_cast<int>((impactPoint.Y + size_.Y / 2.0f) / divSize_.Y);

	// �͈͊O�Q�Ƃ�����邽�߃N�����v
	X = FMath::Clamp(X, 0, static_cast<int>(divisionNumMAX_.X) - 1);
	Y = FMath::Clamp(Y, 0, static_cast<int>(divisionNumMAX_.Y) - 1);

	// �z��̔ԍ�
	playersNum_[MyID].first = Y * divisionNumMAX_.X + X;

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue,FString::FromInt(playersNum_[MyID].first));

	playersNum_[MyID].second =
	{
		X * divSize_.X + divSize_.X / 2.0f - size_.X / 2.0f,
		Y * divSize_.Y + divSize_.Y / 2.0f - size_.Y / 2.0f,
		impactPoint.Z
	};

	// �f�o�b�O�p�K�C�h�\��
	FLinearColor col = FLinearColor::Red;
	if (spaceState_[playersNum_[MyID].first].first == StageSpaceState::NotPut)
	{
		col = FLinearColor::Blue;
	}
	if (spaceState_[playersNum_[MyID].first].first == StageSpaceState::Put)
	{
		col = FLinearColor::Green;
	}

	// �f�o�b�O�`��
	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), impactPoint, 30.0f, 12, col);
	UKismetSystemLibrary::DrawDebugBox(GetWorld(), playersNum_[MyID].second, divSize_ / 2.0f, col);
}

void AStageMain::Reset(const int32& MyID)
{
	// �K�[�h����
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
	// �K�[�h����
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
	// �����ݒu����ĂȂ�������ݒu����
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
	// �K�[�h����
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

	// �����ݒu����Ă�����폜����
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

