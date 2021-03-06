// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyEditerCharacter.generated.h"

UCLASS()
class PROTOTYPE1_API AMyEditerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyEditerCharacter();

protected:
	// Called when the game starts or when spawned
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginPlay();

	virtual void BeginPlay() override;
public:
	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float movescale_;

	// FPScamera
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FPSCameraComponent;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Handles input for moving foward and backward
	void MoveForwardAndBackward(float value);

	void MoveRightAndLeft(float value);

	void MoveUpdown(float value);
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
