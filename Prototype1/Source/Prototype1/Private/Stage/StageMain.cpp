// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage/StageMain.h"

// Sets default values
AStageMain::AStageMain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	

}

// Called when the game starts or when spawned
void AStageMain::BeginPlay()
{
	Super::BeginPlay();
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Stage Create"));

	this->GetActorBounds(true, senterPos_, size_);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, senterPos_.ToString());
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, size_.ToString());
}

// Called every frame
void AStageMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);




}

