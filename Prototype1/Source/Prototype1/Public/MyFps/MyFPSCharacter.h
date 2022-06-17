// Fill out your copyright  notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "MyFPSProjectile.h"
#include "MyFPSCharacter.generated.h"

UCLASS()
class PROTOTYPE1_API AMyFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Projectile class to spawn.
	UPROPERTY(EditAnywhere, Category = Projectile)
	TSubclassOf<class AMyFPSProjectile> ProjectileClass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handles input for moving foward and backward
	UFUNCTION()
	void MoveForwardAndBackward(float value);

	UFUNCTION()
	void MoveRightAndLeft(float value);

	UFUNCTION()
	void StartJump();

	UFUNCTION()
	void StopJump();

	UFUNCTION()
	void FireKeep();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	// ïœêî
	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool HaveFlag;

	// FPScamera
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FPSCameraComponent;

	// First-person mesh(arms)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	USkeletalMeshComponent* FPSMesh;

	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	// ïêäÌ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TArray<AActor*> Weaponvec_;

	// ïêäÌ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	AActor* myweapon_;

	UFUNCTION(BlueprintCallable, Category = MyFunction)
	void Fire();
private:
};