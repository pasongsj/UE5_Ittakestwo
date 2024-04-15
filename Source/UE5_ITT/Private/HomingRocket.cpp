// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingRocket.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "EnemyFlyingSaucer.h"
#include "ExplosionEffect.h"
#include "Floor.h"
#include "FsmComponent.h"
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

	FireEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireEffectComponent"));
	FireEffectComp->SetupAttachment(SceneComp);

	SetupFsmComponent();
}

// Called when the game starts or when spawned
void AHomingRocket::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr != RocketFsmComponent)
	{
		// ���� ���� ������ �׽�Ʈ�ڵ�
		// RocketFsmComponent->ChangeState(ERocketState::PlayerEquip);
		RocketFsmComponent->ChangeState(ERocketState::PlayerChase);
	}

	SetupOverlapEvent();

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void AHomingRocket::SetupOverlapEvent()
{
	if (nullptr != RocketMeshComp)
	{
		RocketMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AHomingRocket::OnOverlapBegin);
		RocketMeshComp->OnComponentEndOverlap.AddDynamic(this, &AHomingRocket::OnOverlapEnd);
	}
}

void AHomingRocket::SetupFsmComponent()
{
	RocketFsmComponent = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComponent"));
	RocketFsmComponent->CreateState(ERocketState::PlayerChase,
		[this]
		{

		},

		[this](float DT)
		{
			if (false == bIsActive)
			{
				RocketFsmComponent->ChangeState(ERocketState::Destroy);
				return;
			}


			if (nullptr == TargetActor)
			{
				return;
			}

			// Ÿ�پ��Ͱ� nullptr�̰ų�, ���� ������ ������ ��� �����ߴٸ� -> ChangeState
			if (0.0f >= RocketLifeTime)
			{
 				if (nullptr != RocketFsmComponent)
				{
					RocketFsmComponent->ChangeState(ERocketState::PlayerEquipWait);
					return;
				}
			}

			RocketLifeTime -= DT;
			FVector RocketLocation = GetActorLocation();
			FVector TargetLocation = TargetActor->GetActorLocation();

			FVector Dir = TargetLocation - RocketLocation;
			Dir.Normalize();

			SetActorRotation(Dir.Rotation());

			FVector NewRocketLocation = RocketLocation + Dir * RocketMoveSpeed * DT;
			SetActorLocation(NewRocketLocation);

			if (false == bIsSetLifeTime && true == IsMaxFloorDistance())
			{
				SetRocektLifeTime(3.0f);
				bIsSetLifeTime = true;
			}
		},

		[this]
		{
		});

	RocketFsmComponent->CreateState(ERocketState::PlayerEquipWait,
		[this]
		{
			RocketMeshComp->SetSimulatePhysics(true);
			RocketMeshComp->SetEnableGravity(true);
			FireEffectComp->SetActive(false);

			AEnemyFlyingSaucer* ParentActor = Cast<AEnemyFlyingSaucer>(GetOwner());
			if (nullptr != ParentActor)
			{
				ParentActor->DisCountHomingRocketFireCount();
			}
		},

		[this](float DT)
		{
			if (true == bIsPlayerOverlap)
			{
				RocketFsmComponent->ChangeState(ERocketState::PlayerEquip);
				return;
			}
		},

		[this]
		{
			
		});

	RocketFsmComponent->CreateState(ERocketState::PlayerEquip,
		[this]
		{
			bIsPlayerEquip = true;
		},

		[this](float DT)
		{
			// test �ڵ�
			FVector RocketLocation = GetActorLocation();
			FVector TargetLocation = GetOwner()->GetActorLocation();
			TargetLocation.Z += 1200.0f;


			FVector Dir = TargetLocation - RocketLocation;
			Dir.Normalize();

			SetActorRotation(Dir.Rotation());

			FVector NewRocketLocation = RocketLocation + Dir * RocketMoveSpeed * DT;
			SetActorLocation(NewRocketLocation);
		},

		[this]
		{
		});

	RocketFsmComponent->CreateState(ERocketState::Destroy,
		[this]
		{
			DestroyRocket();
		},

		[this](float DT)
		{
			
		},

		[this]
		{
		});


}

// Called every frame
void AHomingRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		

	}
}

bool AHomingRocket::IsMaxFloorDistance()
{
	AEnemyFlyingSaucer* OnwerActor = Cast<AEnemyFlyingSaucer>(GetOwner());
	if (nullptr != OnwerActor)
	{
		FVector FloorLocation = OnwerActor->GetFloor()->GetActorLocation();
		float LocationZ = GetActorLocation().Z - FloorLocation.Z;
		if (MaxFloorDistance >= LocationZ)
		{
			return true;
		}
	}

	return false;
}

void AHomingRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}

void AHomingRocket::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	int32 CurrentState = RocketFsmComponent->GetCurrentState();

	ERocketState CurrentStateEnum = static_cast<ERocketState>(CurrentState);

	switch (CurrentStateEnum)
	{
	case ERocketState::PlayerChase:
	{
		if (true == OtherActor->ActorHasTag(TEXT("Player")))
		{
			if (true == bIsActive)
			{
				bIsActive = false;
			}

		}
	}
	break;
	case ERocketState::PlayerEquipWait:
	{
		if (true == OtherActor->ActorHasTag(TEXT("Player")))
		{
			bIsPlayerOverlap = true;
		}
	}
	break;
	case ERocketState::PlayerEquip:
	{
		if (true == OtherActor->ActorHasTag(TEXT("Boss")))
		{
			// ���⼭�� �Ұ��� �����ϴ°ɷ�
			// DestroyRocket();
		}
	}
	break;
	default:
		break;
	}
}

void AHomingRocket::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AHomingRocket::DestroyRocket()
{
	FVector SettingLocation = GetActorLocation();
	AExplosionEffect* Effect = GetWorld()->SpawnActor<AExplosionEffect>(ExplosionEffectClass, SettingLocation, FRotator::ZeroRotator);
	AEnemyFlyingSaucer* ParentActor = Cast<AEnemyFlyingSaucer>(GetOwner());
	if (Effect != nullptr)
	{
		if (nullptr != ParentActor)
		{
			AActor* FloorActor = Cast<AActor>(ParentActor->GetFloor());
			Effect->AttachToActor(FloorActor, FAttachmentTransformRules::KeepWorldTransform);
			ParentActor->DisCountHomingRocketFireCount();
		}
	}

	Destroy();
}

