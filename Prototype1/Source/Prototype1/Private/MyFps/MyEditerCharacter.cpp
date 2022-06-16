// Fill out your copyright notice in  the Description  page of Project Settings.


#include "MyFps/MyEditerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyEditerCharacter::AMyEditerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Camera
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FPSCameraComponent != nullptr);
	// capsule attach
	FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	// relativelocation setup
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	// Enable the pawn to control camera rotation
	FPSCameraComponent->bUsePawnControlRotation = true;

	// [追加] アクターがレプリケートされるようにする
	bReplicates = true;
}

void AMyEditerCharacter::BeginPlay()
{
	Super::BeginPlay();

	AMyEditerCharacter::ServerBeginPlay();
}

// Called when the game starts or when spawned
void AMyEditerCharacter::ServerBeginPlay_Implementation()
{
	// 飛行モード
	this->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	APlayerController* playerCtl = UGameplayStatics::GetPlayerController(this, 0);
	UE_LOG(LogTemp, Log, TEXT("BeginPlay now"));
	if (playerCtl) {
		UE_LOG(LogTemp, Log, TEXT("have playerctl "));
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("don't have playerctl"));
	}
}

bool AMyEditerCharacter::ServerBeginPlay_Validate()
{
	return true;
}
// Called every frame
void AMyEditerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyEditerCharacter::MoveForwardAndBackward(float value)
{
	// 前後
	//int deg = 180;
	//FVector dir = FVector().RotateAngleAxis(value * deg,FPSCameraComponent->GetForwardVector());
	//SetActorLocation(GetActorLocation() + Direction);
	
	//FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	////AddMovementInput(Direction, value);
	//AddMovementInput(GetActorForwardVector(), value);

	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	value *= movescale_;
	AddMovementInput(Direction, value);
}
void AMyEditerCharacter::MoveRightAndLeft(float value)
{
	//FPSCameraComponent->GetForwardVector().RotateAngleAxis(value * deg);
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	value *= movescale_;
	//SetActorLocation(GetActorLocation() + Direction);
	AddMovementInput(Direction, value);
}
void AMyEditerCharacter::MoveUpdown(float value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Z);
	value *= movescale_;
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

	PlayerInputComponent->BindAxis("Fly", this, &AMyEditerCharacter::MoveUpdown);
}

