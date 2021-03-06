// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFps/MyFPSProjectile.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyFPSProjectile::AMyFPSProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// original
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}
	if (!CollisionComponent)
	{
		// Use a sphere as a simple collision representation.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// Set the sphere's collision radius.
		CollisionComponent->InitSphereRadius(1.0f);
		// Set the root component to be the collision component.
		RootComponent = CollisionComponent;
	}

	// Movement
	if (!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 10000.0f;
		ProjectileMovementComponent->MaxSpeed = 10000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	// Mesh
	if (!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/MyPerson/Sphere.Sphere'"));
		if (Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}
	}

	// Material 
	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("'/Game/MyPerson/SphereMaterial.SphereMaterial'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);

	// Delete the projectile after 3 seconds.
	InitialLifeSpan = 3.0f;

	// Colision
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

	// Event called when component hits something.
	CollisionComponent->OnComponentHit.AddDynamic(this, &AMyFPSProjectile::OnHit);

	// [????] ?A?N?^?[?????v???P?[?g????????????????
	bReplicates = true;
}

// Called when the game starts or when spawned
void AMyFPSProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyFPSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyFPSProjectile::FireInDirection(const FVector& ShootDirection)
{
	// ?v???C???[?R???g???[???[????
	APlayerController* playercontroller = UGameplayStatics::GetPlayerController(this, 0);
	
	// ???S???W??3D??????
	
	//playercontroller->DeprojectScreenPositionToWorld(GEngine->GetView)

	// ??????????????
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AMyFPSProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		//OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
	}
	FVector hitlocation = HitComponent->GetComponentLocation();
	this->SetActorLocation(hitlocation);
	// Destroy();
}

