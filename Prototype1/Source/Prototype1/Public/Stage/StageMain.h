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
	/// �}�X����肷��
	/// </summary>
	/// <param name="impactPoint">�u�������}�X</param>
	UFUNCTION(BlueprintCallable, Category = "StageF")
	void DrawSpace(const int32& spaceNum);

	// �ݒu����
	// ���ƂŃA�C�e�����X�g�����炤�悤�ɂ���
	UFUNCTION(BlueprintCallable, Category = "StageF")
	void Put(const int32& spaceNum);
	// �P������
	UFUNCTION(BlueprintCallable, Category = "StageF")
	void Take(const int32& spaceNum);

	// �P������
	UFUNCTION(BlueprintCallable, Category = "StageF")
	int32 GetSpaceNum(const FVector& impactPoint);

	//// �v���C���[�̒ǉ�
	//// �����ɕۑ��������ϐ���n��
	//UFUNCTION(BlueprintCallable, Category = "StageF")
	//int32 AddPlayer(UPARAM(ref) int32& myID);

private:
	// �}�X�ڂ��c�Ɖ����}�X�����ׂ�
	FVector2D SpaceToXY(const int32& spaceNum);

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 1,ClampMax = 10000))
	float rayLength_;																	// ���C�̒���

	UPROPERTY(EditAnywhere,Meta = (AllowPreserveRatio, ClampMin = 1, ClampMax = 10000))
	FVector SpaceSize_;																	// �P�}�X�̃T�C�Y

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.01, ClampMax = 5))
	float dbgTime_;																		// �f�o�b�O�p

	FVector divisionNumMAX_;															// ������

	FVector centerPos_;																	// ���f���̒��S���W
	FVector size_;																		// ���f���S�̂̑傫��
	FVector divSize_;																	// 1�������̑傫��

	//std::vector<std::pair<int32, FVector>> playersNum_;								// �v���C���[���Ƃɂق������{�}�X�̔ԍ��A�ݒu����ꍇ�̍��W}

	std::vector<std::pair<StageSpaceState, AActor*>> spaceState_;						// �}�X�̃X�e�[�^�X�Ɣz�u�I�u�W�F�N�g

	// ��
	FString path_;
	TSubclassOf<AActor> subClass_;
};
