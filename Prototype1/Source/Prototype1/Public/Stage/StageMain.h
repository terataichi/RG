// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include <memory>
#include <map>
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

	// �����ŌĂ��
	UFUNCTION(BlueprintCallable, Category = "StageF")
	void Init(FVector spaceSize);

	/// <summary>
	/// �}�X����肷��
	/// </summary>
	/// <param name="impactPoint">�u�������}�X</param>
	UFUNCTION(BlueprintCallable, Category = "StageF")
	void DrawSpace(const int32& spaceNum,AActor* Obj, const float& z);

	UFUNCTION(BlueprintCallable, Category = "StageF")
	void ResetPreview();

	// �ݒu����
	// ���ƂŃA�C�e�����X�g�����炤�悤�ɂ���
	UFUNCTION(BlueprintCallable, Category = "StageF")
	bool Put(const int32& spaceNum,AActor* Obj,const float& z);
	// �P������
	UFUNCTION(BlueprintCallable, Category = "StageF")
	AActor* Take(const int32& spaceNum);

	// �P������
	UFUNCTION(BlueprintCallable, Category = "StageF")
	int32 GetSpaceNum(const FVector& impactPoint);

private:
	void MapInit();

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

	FString nowPreview_;
	AActor* previewObj_;
	std::vector<std::pair<StageSpaceState, AActor*>> spaceState_;						// �}�X�̃X�e�[�^�X�Ɣz�u�I�u�W�F�N�g

	TMap<FString, FString> previewMap_;
};
