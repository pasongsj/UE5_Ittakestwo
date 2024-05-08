// Fill out your copyright notice in the Description page of Project Settings.


#include "Laser.h"
#include "FsmComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ALaser::ALaser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		bReplicates = true;
		SetReplicateMovement(true);
		LaserMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMesh"));
		RootComponent = LaserMesh;

		SetupFsm();
	}


}

// Called when the game starts or when spawned
void ALaser::BeginPlay()
{
	Super::BeginPlay();

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::LaserOn);

		bPhaseEnd = true;

		DefaultPos = LaserMesh->GetRelativeLocation();
		AttackPos = DefaultPos;
		AttackPos.Z += AttackMoveSize;
	}
}

void ALaser::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));
	FsmComp->CreateState(Fsm::Wait,
		[this]
		{

		},

		[this](float DT)
		{
			if (true == bAttackStart)
			{
				FsmComp->ChangeState(Fsm::MoveUp);
			}
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::MoveUp,
		[this]
		{
		},

		[this](float DT)
		{
			MovingRatio += DT;

			if (MovingRatio >= 1.f)
			{
				MovingRatio = 1.f;
				FsmComp->ChangeState(Fsm::LaserOn);
			}

			SetActorRelativeLocation(FMath::Lerp(DefaultPos, AttackPos, MovingRatio));
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::LaserOn,
		[this]
		{
			SetActiveLaser(true);
		},

		[this](float DT)
		{
			//������ ���� �����ϴ� �ڵ�
			LaserSizeRatio += DT/ LaserIncreaseTime;
			if (LaserIncreaseTime <= FsmComp->GetStateLiveTime())
			{
				LaserSizeRatio = 1.f;
				SetLaserSize(FMath::Lerp(1, LaserMaxSize, LaserSizeRatio));
				FsmComp->ChangeState(Fsm::Attack);
				return;
			}

			SetLaserSize(FMath::Lerp(1, LaserMaxSize, LaserSizeRatio));
		},

		[this]
		{
			LaserSizeRatio = 0.f;
		});

	FsmComp->CreateState(Fsm::Attack,
		[this]
		{

		},

		[this](float DT)
		{
			if (false == bAttackStart)
			{
				FsmComp->ChangeState(Fsm::LaserOff);
			}


			AddActorLocalRotation({ 0.f,RotateSpeed * DT,0.f });
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::LaserOff,
		[this]
		{
			SetActiveLaser(false);
			UE_LOG(LogTemp, Display, TEXT("OffLaser"));
		},

		[this](float DT)
		{
			//������ ���� �����ϴ� �ڵ�
			FsmComp->ChangeState(Fsm::MoveDown);
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::MoveDown,
		[this]
		{

		},

		[this](float DT)
		{
			MovingRatio -= DT;

			if (MovingRatio <= 0.f)
			{
				MovingRatio = 0.f;
				FsmComp->ChangeState(Fsm::Wait);
			}

			SetActorRelativeLocation(FMath::Lerp(DefaultPos, AttackPos, MovingRatio));
		},

		[this]
		{
		});

}

// Called every frame
void ALaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

