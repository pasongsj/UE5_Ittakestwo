// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFlyingSaucer.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyMoonBaboon.h"

// Sets default values
AEnemyFlyingSaucer::AEnemyFlyingSaucer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(SceneComp);
	
	GetArrowComponent()->SetupAttachment(SceneComp);
	GetMesh()->SetupAttachment(SceneComp);
}

// Called when the game starts or when spawned
void AEnemyFlyingSaucer::BeginPlay()
{
	Super::BeginPlay();

	EnemyMoonBaboon = GetWorld()->SpawnActor<AEnemyMoonBaboon>(EnemyMoonBaboonClass);
	EnemyMoonBaboon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("ChairSocket"));
	EnemyMoonBaboon->SetOwner(this);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

// Called every frame
void AEnemyFlyingSaucer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		int a = 0;
	}
}

// Called to bind functionality to input
void AEnemyFlyingSaucer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

