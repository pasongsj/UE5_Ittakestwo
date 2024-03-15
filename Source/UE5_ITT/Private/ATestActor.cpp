// Fill out your copyright notice in the Description page of Project Settings.


#include "ATestActor.h"

// Sets default values
AATestActor::AATestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AATestActor::BeginPlay()
{
	Super::BeginPlay();

	StartPosFloat = GetActorLocation();

	MovingUnit = 0.0f;
	Test_Dir = StartPosFloat - DestPosFloat;
	//SetActorLocation(StartPosFloat);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

// Called every frame
void AATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		if (MovingUnit > 1.0f * 2)
		{
			FVector tmp = StartPosFloat;
			StartPosFloat = DestPosFloat;
			DestPosFloat = tmp;
			MovingUnit = 0.0f;
		}
		// �̵��ϵ��� �ϴ� ����
		MovingUnit += DeltaTime;
		FVector NextPos = FMath::Lerp(StartPosFloat, DestPosFloat, MovingUnit / 2);
		SetActorLocation(NextPos);
	}
}
