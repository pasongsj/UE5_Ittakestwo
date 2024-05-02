// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingRocket.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "EnemyFlyingSaucer.h"
#include "ExplosionEffect.h"
#include "Floor.h"
#include "FsmComponent.h"
#include "Cody.h"
#include "Net/UnrealNetwork.h"
#include "PlayerBase.h"

// Sets default values
AHomingRocket::AHomingRocket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		bReplicates = true;
		SetReplicateMovement(true);

		SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
		SetRootComponent(SceneComp);

		RocketMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
		RocketMeshComp->SetupAttachment(SceneComp);

		FireEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireEffectComponent"));
		FireEffectComp->SetupAttachment(SceneComp);

		SetupFsmComponent();
	}
}

void AHomingRocket::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// �޽� ������Ʈ�� Replication�ϱ� ���� ���� �߰�
	DOREPLIFETIME(AHomingRocket, FireEffectComp);
}

const int32 AHomingRocket::GetCurrentState() const
{
	if (nullptr == RocketFsmComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("RocketFsmComponent is nullptr"));
	}

	return RocketFsmComponent->GetCurrentState();
}

// Called when the game starts or when spawned
void AHomingRocket::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		RocketFsmComponent->ChangeState(ERocketState::PlayerChase);
		SetupOverlapEvent();
	}
}

void AHomingRocket::Multicast_SpawnDestroyEffect_Implementation()
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
			// �������̺�Ʈ���� ��Ȱ��üũ, ��Ȱ���� ��� Destroy �� ����.
			if (false == bIsActive)
			{
				RocketFsmComponent->ChangeState(ERocketState::DestroyWait);
				return;
			}

			// Ÿ�پ��Ͱ� nullptr �̶�� return;
			// ���� ������..? 
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

			// �÷��̾� ���� ���� 
			TickPlayerChaseLogic(DT);
		},

		[this]
		{
			// chase ���¿��� �ٸ� ���·� �Ѿ ��� �θ���� ����ī��Ʈ ���� -- 
			AEnemyFlyingSaucer* ParentActor = Cast<AEnemyFlyingSaucer>(GetOwner());
			if (nullptr != ParentActor)
			{
				ParentActor->DisCountHomingRocketFireCount();
			}
		});

	RocketFsmComponent->CreateState(ERocketState::PlayerEquipWait,
		[this]
		{
			RocketMeshComp->SetSimulatePhysics(true);
			RocketMeshComp->SetEnableGravity(true);
			Multicast_ActivateFireEffectComponent(false);
			
			// UE_LOG(LogTemp, Warning, TEXT("Start Player EquipWait"));
		},

		[this](float DT)
		{
			// �������� �� �÷��̾� Ű ���ȴ��� Ȯ���Ͽ� ���º���
			if (true == bIsPlayerOverlap && nullptr != OverlapActor)
			{
				APlayerBase* PlayerBase = Cast<APlayerBase>(OverlapActor);
				bool KeyCheck = PlayerBase->GetIsInteract();
				if (true == KeyCheck)
				{
					UE_LOG(LogTemp, Warning, TEXT("Change State PlayerEquip"));
					RocketFsmComponent->ChangeState(ERocketState::PlayerEquip);
					return;
				}
			}
		},

		[this]
		{
			
		});

	RocketFsmComponent->CreateState(ERocketState::PlayerEquip,
		[this]
		{
			UE_LOG(LogTemp, Warning, TEXT("Start PlayerEquip"));

			if (nullptr != OverlapActor)
			{
				bIsPlayerEquip = true;
				UE_LOG(LogTemp, Warning, TEXT("Attach To Actor"));

				RocketMeshComp->SetSimulatePhysics(false);
				RocketMeshComp->SetEnableGravity(false);
				RocketMeshComp->AttachToComponent(SceneComp, FAttachmentTransformRules::KeepRelativeTransform);
				
				USkeletalMeshComponent* ActorMesh = OverlapActor->GetMesh();
				if (nullptr != ActorMesh)
				{
					SetActorRelativeLocation(FVector::ZeroVector);
					RocketMeshComp->SetRelativeLocation(FVector::ZeroVector);
					/*SetActorRelativeRotation(FRotator::ZeroRotator);
					RocketMeshComp->SetRelativeRotation(FRotator::ZeroRotator);*/

					AttachToComponent(OverlapActor->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("RocketSocket"));
					this->SetOwner(OverlapActor);

					APlayerBase* PlayerBase = Cast<APlayerBase>(OverlapActor);
					PlayerBase->TestFunction();
				}
				
				UE_LOG(LogTemp, Warning, TEXT("Attach Clear"));
			}
		},

		[this](float DT)
		{
			AActor* ParentActor = GetAttachParentActor();
			if (nullptr == ParentActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("Parent Actor nullptr"));
			}
			else
			{
				bool Check = ParentActor->ActorHasTag(TEXT("Player"));
				if (true == Check)
				{
					UE_LOG(LogTemp, Warning, TEXT("Player Equip"));
				}
			}
		},

		[this]
		{
		});

	// �������� �� ������ �������� �ʰ� ����Ʈ�� ������ ���·� 10���Ŀ� ����
	RocketFsmComponent->CreateState(ERocketState::DestroyWait,
		[this]
		{
			Multicast_SpawnDestroyEffect();
			this->SetActorHiddenInGame(true);
		},

		[this](float DT)
		{
			UE_LOG(LogTemp, Warning, TEXT("Rocket Destroy Wait Tick"));

			// �������ӽð��� 10�ʰ� �Ѿ��ٸ� Destroy ���·� ����
			if (10.0f <= RocketFsmComponent->GetStateLiveTime())
			{
				RocketFsmComponent->ChangeState(ERocketState::Destroy);
			
				return;
			}
		},

		[this]
		{
		});

	RocketFsmComponent->CreateState(ERocketState::Destroy,
		[this]
		{
			UE_LOG(LogTemp, Warning, TEXT("Rocket Destroy State Start"));
		},

		[this](float DT)
		{
		},

		[this]
		{
		});

	// ��Ʈ���� ��� ���� ����� 

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

void AHomingRocket::Multicast_ActivateFireEffectComponent_Implementation(const bool bIsActivate)
{
	FireEffectComp->SetActive(bIsActivate, true);
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

void AHomingRocket::TickPlayerChaseLogic(float DeltaTime)
{
	RocketLifeTime -= DeltaTime;
	FVector RocketLocation = GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	FVector Dir = TargetLocation - RocketLocation;
	Dir.Normalize();

	SetActorRotation(Dir.Rotation());

	FVector NewRocketLocation = RocketLocation + Dir * RocketMoveSpeed * DeltaTime;
	SetActorLocation(NewRocketLocation);


	if (false == bIsSetLifeTime && true == IsMaxFloorDistance())
	{
		SetRocektLifeTime(3.0f);
		bIsSetLifeTime = true;
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
			APlayerBase* PlayerBase = Cast<APlayerBase>(OtherActor);
			OverlapActor = PlayerBase;
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
	if (nullptr != OtherActor)
	{
		if (true == OtherActor->ActorHasTag("Player"))
		{
			bIsPlayerOverlap = false;
		}
	}
}

void AHomingRocket::DestroyRocket()
{
	Destroy();
}

