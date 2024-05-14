// Fill out your copyright notice in the Description page of Project Settings.


#include "ParentShutter.h"
#include "CoreShutter.h"

// Sets default values
AParentShutter::AParentShutter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		bReplicates = true;
		SetReplicateMovement(true);
	}
	
}

// Called when the game starts or when spawned
void AParentShutter::BeginPlay()
{
	Super::BeginPlay();

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		////SpawnActor_Implementation();
		ArrayCoreShutter.SetNum(4);
		//
		ArrayCoreShutter[0] = Shutter0;
		ArrayCoreShutter[1] = Shutter1;
		ArrayCoreShutter[2] = Shutter2;
		ArrayCoreShutter[3] = Shutter3;
		for (int32 i = 0; i < 4; i++)
		{
			ArrayCoreShutter[i]->SetActorRelativeRotation({ 0.f, 90.f * i,0.f });
			ArrayCoreShutter[i]->SetPivotPos(GetActorLocation());
		}

		ArrayCoreShutter[0]->SetOpenPos({ -MovingSize,MovingSize ,0.f });
		ArrayCoreShutter[1]->SetOpenPos({ -MovingSize,-MovingSize ,0.f });
		ArrayCoreShutter[2]->SetOpenPos({ MovingSize,-MovingSize ,0.f });
		ArrayCoreShutter[3]->SetOpenPos({ MovingSize,MovingSize ,0.f });
	}

}

// Called every frame
void AParentShutter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
}

void AParentShutter::SetShutterOpen()
{
	for (int32 i = 0; i < ArrayCoreShutter.Num(); i++)
	{
		ArrayCoreShutter[i]->OpenShutter();
	}
}

void AParentShutter::SetDone()
{
	for (int32 i = 0; i < ArrayCoreShutter.Num(); i++)
	{
		ArrayCoreShutter[i]->SetDone();
	}
}
