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


private:
	UPROPERTY(EditAnywhere)
	float rayLength_;																	// ���C�̒���

	UPROPERTY(EditInstanceOnly)
	FVector SpaceSize_;

	UPROPERTY(EditAnywhere)
	int divisionNumMAX_;																// ������

	UPROPERTY(EditAnywhere)
	float dbgTime_;																		// �f�o�b�O�p

	FVector centerPos_;																	// ���f���̒��S���W
	FVector size_;																		// ���f���S�̂̑傫��
	FVector divSize_;																	// 1�������̑傫��


	std::vector<std::pair<StageSpaceState, AActor*>> spaceState_;						// �}�X�̃X�e�[�^�X�Ɣz�u�I�u�W�F�N�g

	// ��
	FString path_;
	TSubclassOf<AActor> subClass_;
};
