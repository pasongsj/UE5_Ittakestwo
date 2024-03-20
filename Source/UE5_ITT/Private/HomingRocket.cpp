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
}

// Called every frame
void AHomingRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (nullptr == PlayerCodyRef)
	{
		// �÷��̾ nullptr �̶�� �޸𸮿��� ����
		Destroy();
		return;
	}

	// �װ� �ƴ϶�� ���� ���� 
	FVector RocketLocation = GetActorLocation();
	FVector TargetLocation = PlayerCodyRef->GetActorLocation();

	FVector Dir = TargetLocation - RocketLocation;
	Dir.Normalize();

	SetActorRotation(Dir.Rotation());
	
	FVector NewRocketLocation = RocketLocation + Dir * RocketMoveSpeed * DeltaTime;
	SetActorLocation(NewRocketLocation);
}

