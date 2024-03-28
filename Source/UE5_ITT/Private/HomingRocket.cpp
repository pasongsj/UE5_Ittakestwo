// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingRocket.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Cody.h"


// Sets default values
AHomingRocket::AHomingRocket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComp);

	RocketMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMeshComp->SetupAttachment(SceneComp);
}

// Called when the game starts or when spawned
void AHomingRocket::BeginPlay()
{
	Super::BeginPlay();

	// �ӽ�, ��������
	PlayerCodyRef = Cast<ACody>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	RocketMeshComp->OnComponentHit.AddDynamic(this, &AHomingRocket::OnHit);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

// Called every frame
void AHomingRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (nullptr == PlayerCodyRef)
	{
		Destroy();
		return;
	}
	
	FVector RocketLocation = GetActorLocation();
	FVector TargetLocation = PlayerCodyRef->GetActorLocation();

	FVector Dir = TargetLocation - RocketLocation;
	Dir.Normalize();

	SetActorRotation(Dir.Rotation());
	
	FVector NewRocketLocation = RocketLocation + Dir * RocketMoveSpeed * DeltaTime;
	SetActorLocation(NewRocketLocation);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		
	}
}

void AHomingRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// �ӽ� 
	Destroy();
}

