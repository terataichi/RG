// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFps/MyEditerCharacter.h"

// Sets default values
AMyEditerCharacter::AMyEditerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Camera
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FPSCameraComponent != nullptr);
	// capsule attach
	FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	// relativelocation setup
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	// Enable the pawn to control camera rotation
	FPSCameraComponent->bUsePawnControlRotation = true;
	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void AMyEditerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ”òsƒ‚[ƒh
	this->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

// Called every frame
void AMyEditerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyEditerCharacter::MoveForwardAndBackward(float value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, value);
}

void AMyEditerCharacter::MoveRightAndLeft(float value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, value);
}

// Called to bind functionality to input
void AMyEditerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up "move"
	PlayerInputComponent->BindAxis("MoveForwardAndBackward", this, &AMyEditerCharacter::MoveForwardAndBackward);
	PlayerInputComponent->BindAxis("MoveRightAndLeft", this, &AMyEditerCharacter::MoveRightAndLeft);

	// set up "look"
	PlayerInputComponent->BindAxis("Turn", this, &AMyEditerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyEditerCharacter::AddControllerPitchInput);
}

