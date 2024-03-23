// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"
#include "ParentShutter.h"
#include "Pillar.h"
#include "FsmComponent.h"

// Sets default values
AFloor::AFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetupFsm();
}

// Called when the game starts or when spawned
void AFloor::BeginPlay()
{
	Super::BeginPlay();

	FsmComp->ChangeState(Fsm::Phase1_1);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

// Called every frame
void AFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFloor::SetPhase(Fsm Phase)
{	
	FsmComp->ChangeState(Phase);	
}

void AFloor::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));

	FsmComp->CreateState(Fsm::Wait,
		[this]
		{
		},

		[this](float DeltaTime)
		{
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Phase1_1,		
		[this]
		{
			ParentShutter1->SetShutterOpen(true);
		},

		[this](float DeltaTime)
		{
			PillarSecond += DeltaTime;
			if (PillarSecond >= 1.f)
			{
				Pillar1->ChangeState(APillar::EnumState::WaitMove);
				FsmComp->ChangeState(Fsm::Wait);
			}
		},

		[this]
		{
			PillarSecond = 0.f;
		}
		);

}
