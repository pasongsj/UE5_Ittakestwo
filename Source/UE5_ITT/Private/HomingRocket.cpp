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
		RocketFsmComponent->ChangeState(ERocketState::PlayerChase);
	}
	SetupOverlapEvent();

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
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
			if (nullptr == TargetActor)
			{
				return;
			}

			// 타겟액터가 nullptr이거나, 현재 로켓의 수명을 모두 소진했다면 -> ChangeState
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
			
		},

		[this]
		{
		});


}

// Called every frame
void AHomingRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		
	}
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
		FVector SettingLocation = GetActorLocation();
		AExplosionEffect* Effect = GetWorld()->SpawnActor<AExplosionEffect>(ExplosionEffectClass, SettingLocation, FRotator::ZeroRotator);
		AEnemyFlyingSaucer* ParentActor = Cast<AEnemyFlyingSaucer>(GetOwner());
		if (Effect != nullptr)
		{
			if (nullptr != ParentActor)
			{
				AActor* FloorActor = Cast<AActor>(ParentActor->GetFloor());
				Effect->AttachToActor(FloorActor, FAttachmentTransformRules::KeepWorldTransform);
			}
		}

		ParentActor->DisCountHomingRocketFireCount();
		Destroy();
	}
	break;
	case ERocketState::PlayerEquipWait:
	{
		// 내가 플레이어 장착 대기 상태일때 오버랩 이벤트가 발생하면 
		// 플레이어가 e키를 눌렀는지 체크해 
		if (nullptr != OtherActor)
		{
			APlayerBase* PlayerRef = Cast<APlayerBase>(OtherActor);
			if (nullptr != PlayerRef)
			{
				bIsPlayerOverlap = true;
			}
		}


	}
	break;
	case ERocketState::PlayerEquip:
	{

	}
	break;
	default:
		break;
	}
}

void AHomingRocket::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

