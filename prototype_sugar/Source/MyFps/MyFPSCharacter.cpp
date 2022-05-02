// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFPSCharacter.h"

// Sets default values
AMyFPSCharacter::AMyFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("I'm using MyFPSCharacter"));
	
}

// Called every frame
void AMyFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up "move"
	PlayerInputComponent->BindAxis("MoveForwardAndBackward", this, &AMyFPSCharacter::MoveForwardAndBackward);
	PlayerInputComponent->BindAxis("MoveRightAndLeft", this, &AMyFPSCharacter::MoveRightAndLeft);

	// set up "look"
	PlayerInputComponent->BindAxis("Turn", this, &AMyFPSCharacter::AMyFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyFPSCharacter::AMyFPSCharacter::AddControllerPitchInput);
}

void AMyFPSCharacter::MoveForwardAndBackward(float value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, value);
}

void AMyFPSCharacter::MoveRightAndLeft(float value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, value);
}

