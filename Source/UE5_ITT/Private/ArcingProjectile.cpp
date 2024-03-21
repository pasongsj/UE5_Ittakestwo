// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcingProjectile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// test
#include "Cody.h"


// Sets default values
AArcingProjectile::AArcingProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComp);

	ProjectileMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMeshComp->SetupAttachment(SceneComp);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComp->SetUpdatedComponent(RootComponent);
	ProjectileMovementComp->InitialSpeed = 400.0f; // Initial speed (will be set based on suggested velocity)
	ProjectileMovementComp->ProjectileGravityScale = 1.0f; // Gravity scale (if you want to override global gravity)
	ProjectileMovementComp->bShouldBounce = false; // Set to true if you want the projectile to bounce
}

// Called when the game starts or when spawned
void AArcingProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetupProjectileMovementComponent();
	ProjectileMeshComp->OnComponentHit.AddDynamic(this, &AArcingProjectile::OnHit);
}

// Called every frame
void AArcingProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (nullptr != ProjectileMovementComp)
	{
		FVector NewLocation = GetActorLocation() + ProjectileMovementComp->Velocity * DeltaTime;
		SetActorLocation(NewLocation);
	}
}

void AArcingProjectile::SetupProjectileMovementComponent()
{
	if (nullptr != ProjectileMovementComp)
	{
		TargetLocation = Cast<ACody>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))->GetActorLocation();
		FVector StartLocation = GetActorLocation(); // ���� ��ġ
		float ArcValue = 0.25f; // ArcParam (0.0-1.0)
		FVector OutVelocity = FVector::ZeroVector;
		if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutVelocity, StartLocation, TargetLocation, GetWorld()->GetGravityZ(), ArcValue))
		{
			ProjectileMovementComp->Velocity = OutVelocity;
		}
	}
}

void AArcingProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	int a = 0; 
}

