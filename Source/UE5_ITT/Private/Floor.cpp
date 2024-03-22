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
	FsmComp->InitFsm(Fsm::None);
	FsmComp->ChangeState(Fsm::Phase1_1);

	UFsmComponent::StateFunction State;

	State.Start.BindLambda([this]
		{
			ParentShutter1->SetShutterOpen(true);
		});
	State.Update.BindLambda([this](float DeltaTime)
		{
			PillarSecond += DeltaTime;
			if (PillarSecond>=1.f)
			{
				Pillar1->ChangeState(APillar::EnumState::WaitMove);
				FsmComp->ChangeState(Fsm::Wait);
			}
		});
	State.End.BindLambda([this]
		{
			PillarSecond = 0.f;
		});

	FsmComp->CreateState(Fsm::Phase1_1, State);
}
