// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFps/MyFPSCharacter.h"

// Sets default values
AMyFPSCharacter::AMyFPSCharacter()
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


	// Arms
	// Create a first person mesh component for the owning player.
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FPSMesh != nullptr);

	// Only the owning player sees this mesh.
	FPSMesh->SetOnlyOwnerSee(true);

	// Attach the FPS mesh to the FPS camera.
	FPSMesh->SetupAttachment(FPSCameraComponent);

	// Disable some environmental shadows to preserve the illusion of having a single mesh.
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(true);
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
	PlayerInputComponent->BindAxis("Turn", this, &AMyFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyFPSCharacter::AddControllerPitchInput);

	// set up "fire"
	//PlayerInputComponent->BindAxis("FireKeep", this, &AMyFPSCharacter::FireKeep);

	// set up "action"
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this,&AMyFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("LookUp", IE_Released ,this,&AMyFPSCharacter::StopJump);

	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyFPSCharacter::ServerFire);
}

void AMyFPSCharacter::MoveForwardAndBackward(float value)
{
	FVector direction = GetActorForwardVector();
	direction.Normalize();
	AddMovementInput(direction, value);
}

void AMyFPSCharacter::MoveRightAndLeft(float value)
{
	FVector direction = GetActorRightVector();
	direction.Normalize();
	AddMovementInput(direction , value);
}

void AMyFPSCharacter::StartJump()
{
	bPressedJump = true;
}

void AMyFPSCharacter::StopJump()
{
	bPressedJump = false;
}

void AMyFPSCharacter::FireKeep()
{
}

void AMyFPSCharacter::Fire()
{
	// Attempt to fire a projectile.
	if (ProjectileClass)
	{
		// Get the camera transform.
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 0.0f,0.0f);

		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		// Skew the aim to be slightly upwards.
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch += 0.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AMyFPSProjectile* Projectile = World->SpawnActor<AMyFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}	
	}
}
void AMyFPSCharacter::ServerFire_Implementation()
{
	// この関数はサーバープロセス上でのみ実行される
	Fire();
}

bool AMyFPSCharacter::ServerFire_Validate()
{
	return true;
}