// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFlyingSaucer.h"
#include "Misc/Paths.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InteractionUIComponent.h"
#include "FsmComponent.h"
#include "HomingRocket.h"
#include "EnemyMoonBaboon.h"
#include "ArcingProjectile.h"
#include "Floor.h"
#include "EnergyChargeEffect.h"
#include "FlyingSaucerAIController.h"
#include "BossRotatePivotActor.h"
#include "OverlapCheckActor.h"
#include "PlayerBase.h"
#include "Cody.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

// test
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

// Sets default values
AEnemyFlyingSaucer::AEnemyFlyingSaucer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		SetupComponent();
		SetupFsmComponent();
		Tags.Add(FName("Boss"));

		bReplicates = true;
		SetReplicateMovement(true);
	}
}

// Called when the game starts or when spawned
void AEnemyFlyingSaucer::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		// �׽�Ʈ ����(����Ÿ������ ���濹��)
		// LaserTargetActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerBase::StaticClass());


		// ž���� ������ ����
		EnemyMoonBaboon = GetWorld()->SpawnActor<AEnemyMoonBaboon>(EnemyMoonBaboonClass);
		EnemyMoonBaboon->SetOwner(this);
		EnemyMoonBaboon->GetMesh()->SetVisibility(false);
		FsmComp->ChangeState(EBossState::None);
	}


}

// test code, ���� ���� 
void AEnemyFlyingSaucer::BossHitTestFireRocket()
{
	// 1������ ����
	AActor* FloorActor = Cast<AActor>(GetFloor());
	AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
	AActor* TargetActor = this;
	AHomingRocket* TestHomingRocket = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
	TestHomingRocket->SetupTarget(TargetActor);
	TestHomingRocket->SetActorLocation(FVector(0.0f, 0.0f, 1000.0f));
	TestHomingRocket->SetOwner(this);
}

void AEnemyFlyingSaucer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// �޽� ������Ʈ�� Replication�ϱ� ���� ���� �߰�
	DOREPLIFETIME(AEnemyFlyingSaucer, EnemyMoonBaboon);
	DOREPLIFETIME(AEnemyFlyingSaucer, FsmComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, CodyHoldingUIComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, MayLaserDestroyUIComp);

	DOREPLIFETIME(AEnemyFlyingSaucer, SkeletalMeshComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, LaserSpawnPointMesh);
	DOREPLIFETIME(AEnemyFlyingSaucer, HomingRocketSpawnPointMesh1);
	DOREPLIFETIME(AEnemyFlyingSaucer, HomingRocketSpawnPointMesh2);
	DOREPLIFETIME(AEnemyFlyingSaucer, ArcingProjectileSpawnPointMesh);
	DOREPLIFETIME(AEnemyFlyingSaucer, AnimInstance);
	DOREPLIFETIME(AEnemyFlyingSaucer, AnimSequence);
	DOREPLIFETIME(AEnemyFlyingSaucer, OverlapCheckActor);
	DOREPLIFETIME(AEnemyFlyingSaucer, EnergyChargeEffect);
	DOREPLIFETIME(AEnemyFlyingSaucer, bIsCodyHoldingEnter);
	DOREPLIFETIME(AEnemyFlyingSaucer, PrevTargetLocation);
	DOREPLIFETIME(AEnemyFlyingSaucer, PrevTargetLocationBuffer);
	DOREPLIFETIME(AEnemyFlyingSaucer, bPrevTargetLocationValid);
	DOREPLIFETIME(AEnemyFlyingSaucer, LaserLerpRatio);
	DOREPLIFETIME(AEnemyFlyingSaucer, LaserLerpRate);
	DOREPLIFETIME(AEnemyFlyingSaucer, LaserFireCount);
	DOREPLIFETIME(AEnemyFlyingSaucer, PatternDestroyCount);
	
}
// Multicast �Լ� 
void AEnemyFlyingSaucer::Multicast_ChangeAnimationFlyingSaucer_Implementation(const FString& AnimationPath, const uint8 AnimType, bool AnimationLoop)
{
	if (nullptr == SkeletalMeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkeletalMeshComp is nullptr"));
		return;
	}

	EAnimationAssetType AnimationAssetType = static_cast<EAnimationAssetType>(AnimType);
	switch (AnimationAssetType)
	{
	case AEnemyFlyingSaucer::EAnimationAssetType::Sequence:
	{
		UAnimInstance* AnimationInstance = SkeletalMeshComp->GetAnimInstance();
		UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, *AnimationPath);
		if (nullptr == LoadedAnimationSequence)
		{
			UE_LOG(LogTemp, Warning, TEXT("Animation Sequence is nullptr"));
			return;
		}
		SkeletalMeshComp->PlayAnimation(LoadedAnimationSequence, AnimationLoop);
	}
		break;
	case AEnemyFlyingSaucer::EAnimationAssetType::Blueprint:
	{
		UAnimBlueprint* LoadedAnimBlueprint = LoadObject<UAnimBlueprint>(nullptr, *AnimationPath);
		if (nullptr != LoadedAnimBlueprint)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Change AnimBluerprint"));
			SkeletalMeshComp->SetAnimInstanceClass(LoadedAnimBlueprint->GeneratedClass);
		}
	}
		break;
	}
	
}

void AEnemyFlyingSaucer::Multicast_ChangeAnimationMoonBaboon_Implementation(const FString& AnimationPath, const uint8 AnimType, bool AnimationLoop)
{
	if (nullptr == EnemyMoonBaboon)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyMoonBaboon is nullptr"));
		return;
	}

	EAnimationAssetType AnimationType = static_cast<EAnimationAssetType>(AnimType);
	switch (AnimationType)
	{
	case AEnemyFlyingSaucer::EAnimationAssetType::Sequence:
	{
		UAnimInstance* AnimationInstance = SkeletalMeshComp->GetAnimInstance();
		UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, *AnimationPath);
		if (nullptr == LoadedAnimationSequence)
		{
			UE_LOG(LogTemp, Warning, TEXT("Animation Sequence is nullptr"));
			return;
		}

		EnemyMoonBaboon->GetMesh()->PlayAnimation(LoadedAnimationSequence, AnimationLoop);
	}
	break;
	case AEnemyFlyingSaucer::EAnimationAssetType::Blueprint:
	{
		UAnimBlueprint* LoadedAnimBlueprint = LoadObject<UAnimBlueprint>(nullptr, *AnimationPath);
		if (nullptr == LoadedAnimBlueprint)
		{
			UE_LOG(LogTemp, Warning, TEXT("AnimBlueprint is nullptr"));
			return;
		}

		EnemyMoonBaboon->GetMesh()->SetAnimInstanceClass(LoadedAnimBlueprint->GeneratedClass);
	}
	break;
	}
}

void AEnemyFlyingSaucer::Multicast_CheckCodyKeyPressedAndChangeState_Implementation()
{
	if (nullptr == OverlapCheckActor)
	{
		return;
	}

	APlayerBase* CurrentOverlapPlayer = OverlapCheckActor->GetCurrentOverlapPlayer();
	if (nullptr != CurrentOverlapPlayer)
	{
		// �ڵ��̰�.
		if (true == CurrentOverlapPlayer->ActorHasTag("Cody"))
		{
			PlayerCody = Cast<ACody>(CurrentOverlapPlayer);

			// �ڵ� Ŀ�� �����ΰ͵� üũ 
			bool bIsInteract = CurrentOverlapPlayer->GetIsInteract();
			CodySize Size = PlayerCody->GetCodySize();

			if (false == bIsInteract)
			{
				// UE_LOG(LogTemp, Warning, TEXT("Player Interact false"));
			}

			// ���� ���ͷ�Ʈ Ű �������� üũ
			if (true == bIsInteract && CodySize::BIG == Size)
			{
				UE_LOG(LogTemp, Warning, TEXT("Change State : CodyHolding_Enter Start"));
				FsmComp->ChangeState(EBossState::CodyHolding_Enter);
				bIsCodyHoldingEnter = true;

				return;
			}
		}
	}
}

void AEnemyFlyingSaucer::Multicast_CreateEnergyChargeEffect_Implementation()
{
	EnergyChargeEffect = nullptr;
	EnergyChargeEffect = GetWorld()->SpawnActor<AEnergyChargeEffect>(EnergyChargeEffectClass, LaserSpawnPointMesh->GetComponentLocation(), FRotator::ZeroRotator);
	EnergyChargeEffect->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("EnergyChargeEffectSpawnPointSocket"));
}

void AEnemyFlyingSaucer::Multicast_SetFocusTarget_Implementation()
{
	AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
	if (nullptr != AIController)
	{
		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
		if (nullptr != BlackboardComp)
		{
			AIController->SetFocus(Cast<AActor>(LaserTargetActor));
		}
	}
}

void AEnemyFlyingSaucer::Multicast_AttachToMoonBaboonActorAndFloor_Implementation()
{
	EnemyMoonBaboon->SetActorRelativeLocation(FVector::ZeroVector);
	EnemyMoonBaboon->GetMesh()->SetVisibility(true);
	EnemyMoonBaboon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("ChairSocket"));
	AttachToActor(FloorObject, FAttachmentTransformRules::KeepWorldTransform);
}

void AEnemyFlyingSaucer::Multicast_SetFocusHoldingCody_Implementation()
{
	AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
	if (nullptr != AIController)
	{
		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
		if (nullptr != BlackboardComp)
		{
			AIController->SetFocus(Cast<AActor>(PlayerCody));
		}
	}
}

void AEnemyFlyingSaucer::Multicast_UnPossess_Implementation()
{
	AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
	if (nullptr != AIController)
	{
		AIController->UnPossess();
	}
}


void AEnemyFlyingSaucer::SetCodyHoldingEnter_CodyLocation()
{
	if (true == bIsCodyHoldingLerpEnd)
	{
		return;
	}


	float DeltaTime = GetWorld()->GetDeltaSeconds();
	CodyLerpRatio += DeltaTime / 4.0f;
	if (CodyLerpRatio >= 1.0f)
	{
		CodyLerpRatio = 1.0f;
		bIsCodyHoldingLerpEnd = true;
	}

	FVector StartLocation = PlayerCody->GetActorLocation();
	FVector EndLocation = FVector(CodyLerpEndLocation.X, CodyLerpEndLocation.Y, StartLocation.Z);

	FVector TargetLocation = FMath::Lerp(StartLocation, EndLocation, CodyLerpRatio);
	PlayerCody->SetActorLocation(TargetLocation);
}



// Called every frame
void AEnemyFlyingSaucer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		UpdateLerpRatioForLaserBeam(DeltaTime);
	}
}

// Called to bind functionality to input
void AEnemyFlyingSaucer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyFlyingSaucer::SetupRotateCenterPivotActor()
{
	// ���⼭ ���� ����ġ�� ���� �����ؾ��ϰ� �����ؾ��� �� �� ����. üũ 
	RotateCenterPivotActor = GetWorld()->SpawnActor<ABossRotatePivotActor>(RotateCenterPivotActorClass, FVector::ZeroVector,FRotator::ZeroRotator);
	if (nullptr != RotateCenterPivotActor)
	{
		RotateCenterPivotActor->SetActorEnableCollision(false);
		UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(RotateCenterPivotActor->GetRootComponent());
		if (nullptr != PrimitiveComp)
		{
			PrimitiveComp->SetEnableGravity(false);
		}
		AttachToActor(RotateCenterPivotActor, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void AEnemyFlyingSaucer::DetachRotateCenterPivotActor()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	if (nullptr != RotateCenterPivotActor)
	{
		RotateCenterPivotActor->Destroy();
	}
}

void AEnemyFlyingSaucer::RotationCenterPivotActor(float DeltaTime)
{
	if (nullptr != RotateCenterPivotActor)
	{
		RotateCenterPivotActor->AddActorLocalRotation({ 0.f, 1.f * DeltaTime * RotateCenterPivotActorMoveSpeed , 0.0f });
	}
}

void AEnemyFlyingSaucer::FireHomingRocket()
{
	if (0 < HomingRocketFireCount)
	{
		return;
	}

	HomingRocketFireCount += 2;

	// 1������ ����
	AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
	AActor* TargetActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerCody")));
	AHomingRocket* HomingRocket1 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
	HomingRocket1->SetupTarget(TargetActor);
	HomingRocket1->SetActorLocation(HomingRocketSpawnPointMesh1->GetComponentLocation());
	HomingRocket1->SetOwner(this);

	// 2������ ����
	TargetActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerMay")));
	AHomingRocket* HomingRocket2 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
	HomingRocket2->SetupTarget(TargetActor);
	HomingRocket2->SetActorLocation(HomingRocketSpawnPointMesh2->GetComponentLocation());
	HomingRocket2->SetOwner(this);
}

void AEnemyFlyingSaucer::FireArcingProjectile()
{
	if (nullptr == FloorObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("FloorObject nullptr"));
	}

	AArcingProjectile* Projectile = GetWorld()->SpawnActor<AArcingProjectile>(ArcingProjectileClass, ArcingProjectileSpawnPointMesh->GetComponentLocation(), FRotator::ZeroRotator);
	AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
	if (nullptr != Projectile && nullptr != AIController)
	{
		FVector TargetLocation = FVector::ZeroVector;
		if (0 == CurrentArcingProjectileTargetIndex)
		{
			AActor* TargetActor = PlayerActors[0];
			TargetLocation = TargetActor->GetActorLocation();
			++CurrentArcingProjectileTargetIndex;
		}
		else
		{
			AActor* TargetActor = PlayerActors[1];
			TargetLocation = TargetActor->GetActorLocation();
			CurrentArcingProjectileTargetIndex = 0;
		}

		Projectile->SetupTargetLocation(TargetLocation);
	}
	
	Projectile->SetupProjectileMovementComponent();
	Projectile->SetOwner(this);
	if (Projectile != nullptr)
	{
		Projectile->AttachToActor(FloorObject, FAttachmentTransformRules::KeepWorldTransform);
	}
}






void AEnemyFlyingSaucer::SetupComponent()
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	SetRootComponent(CapsuleComp);
	
	SkeletalMeshComp = GetMesh();
	SkeletalMeshComp->SetupAttachment(CapsuleComp);

	LaserSpawnPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserSpawnPointMesh"));
	LaserSpawnPointMesh->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("LaserSpawnPointSocket"));

	HomingRocketSpawnPointMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HomingRocketSpawnPointMesh1"));
	HomingRocketSpawnPointMesh1->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("HomingRocketSpawnPointSocket_01"));

	HomingRocketSpawnPointMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HomingRocketSpawnPointMesh2"));
	HomingRocketSpawnPointMesh2->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("HomingRocketSpawnPointSocket_02"));

	ArcingProjectileSpawnPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArcingProjectileSpawnPointMesh"));
	ArcingProjectileSpawnPointMesh->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("ArcingProjectileSpawnPointSocket"));

	CodyHoldingUIComp = CreateDefaultSubobject<UInteractionUIComponent>(TEXT("CodyHoldingUIComponent"));
	CodyHoldingUIComp->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("CodyUISocket"));
	CodyHoldingUIComp->SetVisibility(false, true);

	MayLaserDestroyUIComp = CreateDefaultSubobject<UInteractionUIComponent>(TEXT("MayLaserDestroyUIComponent"));
	MayLaserDestroyUIComp->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("MayUISocket"));
	MayLaserDestroyUIComp->SetVisibility(false, true);
}

void AEnemyFlyingSaucer::DrawDebugMesh()
{
	FVector LaserSpawnPointMeshLocation = LaserSpawnPointMesh->GetComponentLocation();

	float SphereRadius = 50.0f;
	int32 Segments = 12;
	float LifeTime = 0.1f;
	float Thickness = 2.0f;

	DrawDebugSphere(
		GetWorld(),
		LaserSpawnPointMeshLocation,
		SphereRadius,
		Segments,
		FColor::Red,
		false,
		LifeTime,
		0,
		Thickness
		);

	FVector HomingRocketSpawnPointMesh1Locaiton = HomingRocketSpawnPointMesh1->GetComponentLocation();
	
	DrawDebugSphere(
		GetWorld(),
		HomingRocketSpawnPointMesh1Locaiton,
		SphereRadius,
		Segments,
		FColor::Blue,
		false,
		LifeTime,
		0,
		Thickness
	);

	FVector HomingRocketSpawnPointMesh2Locaiton = HomingRocketSpawnPointMesh2->GetComponentLocation();

	DrawDebugSphere(
		GetWorld(),
		HomingRocketSpawnPointMesh2Locaiton,
		SphereRadius,
		Segments,
		FColor::Blue,
		false,
		LifeTime,
		0,
		Thickness
	);

	FVector ArcingProjectileSpawnPointMeshLocaiton = ArcingProjectileSpawnPointMesh->GetComponentLocation();

	DrawDebugSphere(
		GetWorld(),
		ArcingProjectileSpawnPointMeshLocaiton,
		SphereRadius,
		Segments,
		FColor::Green,
		false,
		LifeTime,
		0,
		Thickness
	);
}

void AEnemyFlyingSaucer::SetupFsmComponent()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComponent"));
	FsmComp->CreateState(EBossState::None,
		[this]
		{
		},

		[this](float DT)
		{
			// ���� Ŭ�� ���� ���� ������ ���� ������Ʈ ���� ������ �߰� 
			if (ServerDelayTime <= FsmComp->GetStateLiveTime())
			{

				// ���� 1������ ���� �� �׽�Ʈ �ڵ�
				FsmComp->ChangeState(EBossState::Phase1_BreakThePattern);


				// �����ڵ� 
				// FsmComp->ChangeState(EBossState::Phase1_Progress_LaserBeam_1);
				return;
			}
		},

		[this]
		{
			Multicast_AttachToMoonBaboonActorAndFloor();
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerBase::StaticClass(), PlayerActors);
			if (0 == PlayerActors.Num())
			{
				UE_LOG(LogTemp, Warning, TEXT("Player Actors Size : 0"));
			}
		});

	FsmComp->CreateState(EBossState::Phase1_Progress_LaserBeam_1,
		[this]
		{
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);


			//UE_LOG(LogTemp, Warning, TEXT("Change State : Phase1_Progress_LaserBeam_1"));
		},

		[this](float DT)
		{
			if (1 == PatternDestroyCount)
			{
				
				FsmComp->ChangeState(EBossState::Phase1_Progress_LaserBeam_1_Destroy);
				return;
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase1_Progress_LaserBeam_1_Destroy,
		[this]
		{
			// UE_LOG(LogTemp, Warning, TEXT("Change State : Phase1_Progress_LaserBeam_1_Destroy"));
			SetDamage(CoreExplodeDamage);
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Laser_HitPod_Anim"), 1, false);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Laser_HitPod_Anim"), 1, false);
		},

		[this](float DT)
		{
			USkeletalMeshComponent* MoonBaboonMesh = EnemyMoonBaboon->GetMesh();
			if (false == SkeletalMeshComp->IsPlaying() && false == MoonBaboonMesh->IsPlaying())
			{
				FsmComp->ChangeState(EBossState::Phase1_Progress_ArcingProjectile_1);
				return;
			}
		},

		[this]
		{

		});




	FsmComp->CreateState(EBossState::Phase1_Progress_ArcingProjectile_1,
		[this]
		{
			

			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},

		[this](float DT)
		{
			int32 CurrentFloorPhaseToInt = static_cast<int32>(AFloor::Fsm::Phase1_2);
			if (CurrentFloorPhaseToInt == GetFloorCurrentState())
			{
				FsmComp->ChangeState(EBossState::Phase1_Progress_LaserBeam_2);
				return;
			}

			ArcingProjectileFireTime -= DT;
			if (ArcingProjectileFireTime <= 0.0f)
			{
				FireArcingProjectile();
				ArcingProjectileFireTime = ArcingProjectileMaxFireTime;
			}
		},

		[this]
		{
			ArcingProjectileFireTime = ArcingProjectileMaxFireTime;
		});

	FsmComp->CreateState(EBossState::Phase1_Progress_LaserBeam_2,
		[this]
		{
			

			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},

		[this](float DT)
		{
			if (2 == PatternDestroyCount)
			{
				// UE_LOG(LogTemp, Warning, TEXT("Change State : Phase1_Progress_LaserBeam_2_Destroy"));
				FsmComp->ChangeState(EBossState::Phase1_Progress_LaserBeam_2_Destroy);
				return;
			}
		},

		[this]
		{
			
		});


	FsmComp->CreateState(EBossState::Phase1_Progress_LaserBeam_2_Destroy,
		[this]
		{
			SetDamage(CoreExplodeDamage);
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Laser_HitPod_Anim"), 1, false);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Laser_HitPod_Anim"), 1, false);
		},

		[this](float DT)
		{
			USkeletalMeshComponent* MoonBaboonMesh = EnemyMoonBaboon->GetMesh();
			if (false == SkeletalMeshComp->IsPlaying() && false == MoonBaboonMesh->IsPlaying())
			{
				FsmComp->ChangeState(EBossState::Phase1_Progress_ArcingProjectile_2);
				return;
			}
		},

		[this]
		{

		});


	FsmComp->CreateState(EBossState::Phase1_Progress_ArcingProjectile_2,
		[this]
		{


			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},

		[this](float DT)
		{
			int32 CurrentFloorPhaseToInt = static_cast<int32>(AFloor::Fsm::Phase1_3);
			if (CurrentFloorPhaseToInt == GetFloorCurrentState())
			{
				// UE_LOG(LogTemp, Warning, TEXT("Change State : Phase1_Progress_LaserBeam_3"));

				FsmComp->ChangeState(EBossState::Phase1_Progress_LaserBeam_3);
				return;
			}

			ArcingProjectileFireTime -= DT;
			if (ArcingProjectileFireTime <= 0.0f)
			{
				FireArcingProjectile();
				ArcingProjectileFireTime = ArcingProjectileMaxFireTime;
			}
		},

		[this]
		{
			ArcingProjectileFireTime = ArcingProjectileMaxFireTime;
		});


	FsmComp->CreateState(EBossState::Phase1_Progress_LaserBeam_3,
		[this]
		{
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},

		[this](float DT)
		{
			if (3 == PatternDestroyCount)
			{
				UE_LOG(LogTemp, Warning, TEXT("Change State : Phase1_BreakThePattern"));
				FsmComp->ChangeState(EBossState::Phase1_BreakThePattern);
				return;
			}
		},

		[this]
		{

		});




	FsmComp->CreateState(EBossState::Phase1_BreakThePattern,
		[this]
		{
			// �߶��ִϸ��̼� ���
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_PowerCoresDestroyed_FlyingSaucer_Anim"), 1, false);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Programming_Anim"), 1, true);


			// UI Component Activate
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUFunction(this, TEXT("Multicast_SetActivateUIComponent"), CodyHoldingUIComp, true, true);
			GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 4.5f, false);
			
			// �����ڵ� 
			// GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyFlyingSaucer::ActivateCodyHoldingUIComponent, 4.5f, false);

			// OverlapActor Spawn : ���濹��
			FTimerHandle TimerHandle2;
			GetWorldTimerManager().SetTimer(TimerHandle2, this, &AEnemyFlyingSaucer::SpawnOverlapCheckActor, 4.5f, false);
		},

		[this](float DT)
		{
			// �Լ� ���ο��� �ڵ� Ű üũ �� �ڵ�Ȧ�����·� ����, �ڵ���ġ �������������. 
			// �÷��̾� Get�Լ� ���������ʿ�, �ӽ÷� 2���Ŀ� state �������� ����
			Multicast_CheckCodyKeyPressedAndChangeState();
			/*if (2.0f <= FsmComp->GetStateLiveTime())
			{
				UE_LOG(LogTemp, Warning, TEXT("Change State : Cody Holding"));
				FsmComp->ChangeState(EBossState::CodyHolding);
				return;
			}*/
		},

		[this]
		{

		});

	// �ڵ� �κη����ִ� ���� ��¦ ��� ����
	FsmComp->CreateState(EBossState::CodyHolding_Enter,
		[this]
		{
			//UE_LOG(LogTemp, Warning, TEXT("CodyHolding_Enter Start"));
			// Multicast_SetFocusHoldingCody();
			// �ڵ� ���������� �Ĵٺ�����
			// PlayerCody->GetController();

			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Enter_Anim"), 1, false);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_CodyHolding_Enter_Anim"), 1, false);
			UE_LOG(LogTemp, Warning, TEXT("Set Cody Lerp Timer"));
		},

		[this](float DT)
		{
			// �Ѵ� �ִϸ��̼� ���������, Low, �ִϸ��̼� �ݺ�������·� ����
			USkeletalMeshComponent* MoonBaboonMesh = EnemyMoonBaboon->GetMesh();
			if (false == SkeletalMeshComp->IsPlaying() && false == MoonBaboonMesh->IsPlaying() && true == bIsCodyHoldingLerpEnd)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Change State : CodyHolding_Low"));
				FsmComp->ChangeState(EBossState::CodyHolding_Low);
				return;
			}

			SetCodyHoldingEnter_CodyLocation();
		},

		[this]
		{
			bIsCodyHoldingEnter = false;
			bIsCodyHoldingLerpEnd = false;

			Multicast_UnPossess();
		});

	// �ڵ� ��� �ִ� ���°� 
	FsmComp->CreateState(EBossState::CodyHolding_Low,
		[this]
		{
			// ufo ���� ���� �ִϸ��̼� ���
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Low_Anim"), 1, true);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_CodyHolding_Anim"), 1, true);
			//UE_LOG(LogTemp, Warning, TEXT("CodyHolding_Low Start"));
		},

		[this](float DT)
		{
			// ������Դٴ°� �̹� �������÷��̾ �ִٴ� �Ű�. ���� �˻� ���ʿ� ���� Ű�� �ѹ� �� �����ٸ� Ű�Է� ���·� ��ȯ 
			// �ٵ� ���� �÷��̾� ��ġ������ ���ѻ��´ϱ� nullptr �˻� �ѹ� ���ְ� �����Ұ���~~ 
			if (nullptr == OverlapCheckActor->GetCurrentOverlapPlayer())
			{
				return;
			}

			bool IsPressed = OverlapCheckActor->GetCurrentOverlapPlayer()->GetIsInteract();
			if (true == IsPressed)
			{
				FsmComp->ChangeState(EBossState::CodyHolding_ChangeOfAngle);
				return;
			}
		},

		[this]
		{
			
		});


	FsmComp->CreateState(EBossState::CodyHolding_ChangeOfAngle,
		[this]
		{
			// ufo ���� ���� �ִϸ��̼� ���
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/BluePrints/ABP_EnemyFlyingSaucer_CodyHoldingRotation"), 2, false);
			StateCompleteTime = 2.5f;
		},

		[this](float DT)
		{
			if (StateCompleteTime <= FsmComp->GetStateLiveTime())
			{
				//UE_LOG(LogTemp, Warning, TEXT("Change State : InputKey"));
				FsmComp->ChangeState(EBossState::CodyHolding_InputKey);
				StateCompleteTime = 0.0f;
				return;
			}
		},

		[this]
		{
			
		});

	// ������Դٴ°� Cody üũ�� �����ٴ� �ű� �ѵ�.. ��.. 
	FsmComp->CreateState(EBossState::CodyHolding_InputKey,
		[this]
		{
			// ufo ���� ���� �ִϸ��̼� ���
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Anim"), 1, true);
			bIsKeyInput = true;
			KeyInputTime = KeyInputMaxTime;

			// ����������� ���� UI On
			Multicast_SetActivateUIComponent(MayLaserDestroyUIComp, true, true);
		},

		[this](float DT)
		{
			// ���⼭ ��Ƽĳ��Ʈ�� Űüũ �ʿ�.
			if (KeyInputTime <= 0.0f)
			{
				//UE_LOG(LogTemp, Warning, TEXT("KeyMashing Time : 0.0f , Change State : CodyHolding_ChangeOfAngle_Reverse"));
				FsmComp->ChangeState(EBossState::CodyHolding_ChangeOfAngle_Reverse);
				return;
			}

			APlayerBase* OverlapPlayer = OverlapCheckActor->GetCurrentOverlapPlayer();
			if (nullptr != OverlapPlayer)
			{
				if (OverlapPlayer->ActorHasTag(TEXT("Cody")))
				{
					bool CheckInput = OverlapPlayer->GetIsInteract();
					if (true == CheckInput)
					{
						//UE_LOG(LogTemp, Warning, TEXT("Key Input : true"));
						KeyInputTime = KeyInputAdditionalTime;
					}
				}
			}

			KeyInputTime -= DT;
		},

		[this]
		{
			bIsKeyInput = false;
			KeyInputTime = KeyInputMaxTime;

			Multicast_SetActivateUIComponent(MayLaserDestroyUIComp, false, true);
		});

	FsmComp->CreateState(EBossState::CodyHolding_ChangeOfAngle_Reverse,
		[this]
		{
			// �������� �ٽ� �������� �ִϸ��̼� ����
			//UE_LOG(LogTemp, Warning, TEXT("CodyHolding_ChangeOfAngle_Reverse Start"));
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/BluePrints/ABP_EnemyFlyingSaucer_CodyHoldingRotationReverse"), 2, false);
			StateCompleteTime = 2.0f;
		},

		[this](float DT)
		{
			if (StateCompleteTime <= FsmComp->GetStateLiveTime())
			{
				//UE_LOG(LogTemp, Warning, TEXT("CodyHoldingProgress_NotKeyMashingEnd Start"));
				FsmComp->ChangeState(EBossState::CodyHolding_Low);
				StateCompleteTime = 0.0f;
				return;
			}
		},

		[this]
		{
			
		});
}

void AEnemyFlyingSaucer::Multicast_SetActivateUIComponent_Implementation(UInteractionUIComponent* UIComponent, bool ParentUIActivate, bool ChildUIActivate)
{
	if (nullptr != UIComponent)
	{
		UIComponent->SetVisibility(ParentUIActivate, ChildUIActivate);
	}
}

void AEnemyFlyingSaucer::SpawnOverlapCheckActor()
{
	OverlapCheckActor = GetWorld()->SpawnActor<AOverlapCheckActor>(OverlapCheckActorClass, GetActorLocation(), GetActorRotation());
	OverlapCheckActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("OverlapCheckActorSocket"));
	OverlapCheckActor->SetOverlapActorNameTag(TEXT("Player"));
}

void AEnemyFlyingSaucer::UpdateLerpRatioForLaserBeam(float DeltaTime)
{
	LaserLerpRatio += DeltaTime * LaserLerpRate;
	if (1.0f <= LaserLerpRatio)
	{
		SavePreviousTargetLocation();
		LaserLerpRatio -= 1.0f;
		if (0.1f <= LaserLerpRatio)
		{
			LaserLerpRatio = 0.0f;
		}
	}
}

void AEnemyFlyingSaucer::SavePreviousTargetLocation()
{
	// ���⼭ ���� Ÿ���� �����Ŀ� ���� ���õ� �� �ֵ���.
	if (nullptr != LaserTargetActor)
	{
		FVector CurrentTargetLocation = LaserTargetActor->GetActorLocation();

		// ���� Ÿ�� ��ġ�� ��ȿ�ϴٸ� 
		if (true == bPrevTargetLocationValid)
		{
			// Ÿ�� ��ġ�� ����Ǿ��ִ� ����Ÿ����ġ�� �����ϰ� false ó��
			PrevTargetLocation = PrevTargetLocationBuffer;
			bPrevTargetLocationValid = false;
		}

		else
		{
			// ��ȿ���� �ʴٸ� Ÿ�� ��ġ�� ���� ��ġ�� ����
			PrevTargetLocation = CurrentTargetLocation;
		}

		// Ÿ����ġ�� ����
		PrevTargetLocationBuffer = CurrentTargetLocation;
		bPrevTargetLocationValid = true;
	}
}

void AEnemyFlyingSaucer::SetupLaserTargetActor()
{
	// ����, �ڵ�, ���� ������ ���� ����
	switch (PatternDestroyCount)
	{
	case 0:
		for (AActor* Actor : PlayerActors)
		{
			if (nullptr != Actor && Actor->ActorHasTag(TEXT("May")))
			{
				LaserTargetActor = Actor;
				//UE_LOG(LogTemp, Warning, TEXT("Target Actor Setting : May"));
			}
		}
	
		break;
	case 1:
		for (AActor* Actor : PlayerActors)
		{
			if (nullptr != Actor && Actor->ActorHasTag(TEXT("Cody")))
			{
				LaserTargetActor = Actor;
				//UE_LOG(LogTemp, Warning, TEXT("Target Actor Setting : Cody"));
			}
		}
		break;
	case 2:
		for (AActor* Actor : PlayerActors)
		{
			if (nullptr != Actor && Actor->ActorHasTag(TEXT("May")))
			{
				LaserTargetActor = Actor;
				//UE_LOG(LogTemp, Warning, TEXT("Target Actor Setting : May"));
			}
		}
		break;
	}
}



int32 AEnemyFlyingSaucer::GetFloorCurrentState()
{
	if (nullptr == FloorObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Floor Object is nullptr"));
	}

	return FloorObject->GetCurrentPhase();
}
