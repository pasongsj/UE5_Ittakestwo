// Fill out your copyright notice in the Description page of Project Settings.


#include "UIRoseArm.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AUIRoseArm::AUIRoseArm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	RootComponent = SkeletalMeshComp;

	
}

// Called when the game starts or when spawned
void AUIRoseArm::BeginPlay()
{
	Super::BeginPlay();
	

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	//D: / project / UE5_Ittakestwo / Content / UI / StartLevel / RoseArm / RoseArm_SkeletalMesh.uasset
}

// Called every frame
void AUIRoseArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

