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
#include "OverlapCheckActor.h"
#include "PlayerBase.h"
#include "Cody.h"
#include "May.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GroundPoundEffect.h"

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

void AEnemyFlyingSaucer::AddPatternDestoryCount()
{
	++PatternDestroyCount;
	UE_LOG(LogTemp, Warning, TEXT("Pattern Destroy Count : %d"), PatternDestroyCount);

	UBlackboardComponent* BlackboardComp = Cast<AFlyingSaucerAIController>(GetController())->GetBlackboardComponent();
	BlackboardComp->SetValueAsInt(TEXT("PatternDestroyCount"), PatternDestroyCount);
}

const int32 AEnemyFlyingSaucer::GetCurrentState()
{
	if (nullptr == FsmComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("FsmComponent is Nullptr"));
		return -1;
	}

	return FsmComp->GetCurrentState();
}

// Called when the game starts or when spawned
void AEnemyFlyingSaucer::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		// ž���� ������ ����
		EnemyMoonBaboon = GetWorld()->SpawnActor<AEnemyMoonBaboon>(EnemyMoonBaboonClass);
		EnemyMoonBaboon->SetOwner(this);
		EnemyMoonBaboon->GetMesh()->SetVisibility(false);
		FsmComp->ChangeState(EBossState::None);

		SetupOverlapEvent();
		
	}
}

void AEnemyFlyingSaucer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// �޽� ������Ʈ�� Replication�ϱ� ���� ���� �߰�
	DOREPLIFETIME(AEnemyFlyingSaucer, EnemyMoonBaboon);
	DOREPLIFETIME(AEnemyFlyingSaucer, FsmComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, CodyHoldingUIComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, MayLaserDestroyUIComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, RotatingComp);
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
	DOREPLIFETIME(AEnemyFlyingSaucer, ArcingProjectileFireTime);
	DOREPLIFETIME(AEnemyFlyingSaucer, ArcingProjectileMaxFireTime);
	DOREPLIFETIME(AEnemyFlyingSaucer, PlayerCody);
	DOREPLIFETIME(AEnemyFlyingSaucer, PlayerMay);
	DOREPLIFETIME(AEnemyFlyingSaucer, PrevAnimBoneLocation);
	DOREPLIFETIME(AEnemyFlyingSaucer, MaxHp);
	DOREPLIFETIME(AEnemyFlyingSaucer, CurrentHp);
	DOREPLIFETIME(AEnemyFlyingSaucer, bIsEject);
}

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

void AEnemyFlyingSaucer::Multicast_CheckCodyKeyPressedAndChangeState_Implementation(const bool bIsInput)
{
	if (nullptr != PlayerCody)
	{
		CodySize Size = PlayerCody->GetCodySize();

		// ���� ���ͷ�Ʈ Ű �������� üũ
		if (true == bIsInput && CodySize::BIG == Size)
		{
			UE_LOG(LogTemp, Warning, TEXT("Change State : CodyHolding_Enter Start"));
			FsmComp->ChangeState(EBossState::CodyHolding_Enter);
			bIsCodyHoldingEnter = true;

			return;
		}
	}
}

void AEnemyFlyingSaucer::Multicast_SetActivateUIComponent_Implementation(UInteractionUIComponent* UIComponent, bool ParentUIActivate, bool ChildUIActivate)
{
	if (nullptr != UIComponent)
	{
		UIComponent->SetVisibility(ParentUIActivate, ChildUIActivate);
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
	UE_LOG(LogTemp, Warning, TEXT("Boss Look at Cody Start"));
	FVector TargetLocation = GetActorLocation();
	TargetLocation.Z = 0.0f;
	FVector StartLocation = PlayerCody->GetActorLocation();
	StartLocation.Z = 0.0f;

	FRotator TargetRotation = (TargetLocation - StartLocation).Rotation();
	PlayerCody->SetActorRotation(TargetRotation);
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
	CodyLerpRatio += DeltaTime;
	if (CodyLerpRatio >= 1.0f)
	{
		CodyLerpRatio = 1.0f;
		bIsCodyHoldingLerpEnd = true;

		FVector TargetLocation = GetActorLocation();
		TargetLocation.Z = 0.0f;
		FVector StartLocation = PlayerCody->GetActorLocation();
		StartLocation.Z = 0.0f;

		FRotator TargetRotation = (TargetLocation - StartLocation).Rotation();
		TargetRotation.Yaw -= 14.0f;
		PlayerCody->SetActorRotation(TargetRotation);
	}

	FVector StartLocation = PlayerCody->GetActorLocation();
	FVector EndLocation = FVector(CodyLerpEndLocation.X, CodyLerpEndLocation.Y, StartLocation.Z);

	FVector TargetLocation = FMath::Lerp(StartLocation, EndLocation, CodyLerpRatio);
	PlayerCody->SetActorLocation(TargetLocation);
}

void AEnemyFlyingSaucer::Multicast_HideLaserBaseBone_Implementation()
{
	// ������ �� ������ off
	int32 BoneIndex = SkeletalMeshComp->GetBoneIndex(TEXT("LaserBase"));
	if (INDEX_NONE != BoneIndex)
	{
		SkeletalMeshComp->HideBone(BoneIndex, EPhysBodyOp::PBO_Term);
		UE_LOG(LogTemp, Warning, TEXT("Bone Hide"));
	}
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

void AEnemyFlyingSaucer::FireHomingRocket()
{
	// �÷��̾� �������¶�� �߻� ���ϴ� ���� �߰� �ʿ� 


	// ���� 1
	if (nullptr == HomingRocketActor_1)
	{
		APlayerBase* TargetActor = Cast<APlayerBase>(PlayerActors[0]);
		HomingRocketActor_1 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
		HomingRocketActor_1->SetupTarget(TargetActor);
		HomingRocketActor_1->SetActorLocation(HomingRocketSpawnPointMesh1->GetComponentLocation());
		HomingRocketActor_1->SetOwner(this);
	}

	else if (nullptr != HomingRocketActor_1 && static_cast<int32>(AHomingRocket::ERocketState::DestroyWait) == HomingRocketActor_1->GetCurrentState())
	{
		HomingRocketActor_1->DestroyRocket();

		APlayerBase* TargetActor = Cast<APlayerBase>(PlayerActors[0]);
		HomingRocketActor_1 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
		HomingRocketActor_1->SetupTarget(TargetActor);
		HomingRocketActor_1->SetActorLocation(HomingRocketSpawnPointMesh1->GetComponentLocation());
		HomingRocketActor_1->SetOwner(this);
	}


	// ���� 2
	if (nullptr == HomingRocketActor_2)
	{
		APlayerBase* TargetActor = Cast<APlayerBase>(PlayerActors[1]);
		HomingRocketActor_2 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
		HomingRocketActor_2->SetupTarget(TargetActor);
		HomingRocketActor_2->SetActorLocation(HomingRocketSpawnPointMesh2->GetComponentLocation());
		HomingRocketActor_2->SetOwner(this);
	}

	else if (nullptr != HomingRocketActor_2 && static_cast<int32>(AHomingRocket::ERocketState::DestroyWait) == HomingRocketActor_2->GetCurrentState())
	{
		HomingRocketActor_2->DestroyRocket();

		APlayerBase* TargetActor = Cast<APlayerBase>(PlayerActors[1]);
		HomingRocketActor_2 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
		HomingRocketActor_2->SetupTarget(TargetActor);
		HomingRocketActor_2->SetActorLocation(HomingRocketSpawnPointMesh2->GetComponentLocation());
		HomingRocketActor_2->SetOwner(this);
	}
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

void AEnemyFlyingSaucer::Multicast_CreateGroundPoundEffect_Implementation()
{
	FVector TargetLocation = GetActorLocation();
	TargetLocation.Z = TargetLocation.Z - 1100.0f;
	AGroundPoundEffect* GroundPoundEffect = GetWorld()->SpawnActor<AGroundPoundEffect>(GroundPoundEffectClass, TargetLocation, FRotator::ZeroRotator);
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

	RotatingComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
	RotatingComp->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
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
				// FsmComp->ChangeState(EBossState::Phase1_Progress_LaserBeam_1);
				FsmComp->ChangeState(EBossState::Phase2_BreakThePattern);
				return;
			}
		},

		[this]
		{
			// ������ ���� �� ���� �� �ٴ� ����(�ٴ� �������� �̵��� ������ �Բ� �̵��ϱ� ����)
			Multicast_AttachToMoonBaboonActorAndFloor();

			// ���� �÷��̾ TArray ������ ����
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerBase::StaticClass(), PlayerActors);
			if (0 == PlayerActors.Num())
			{
				UE_LOG(LogTemp, Warning, TEXT("Player Actors Size : 0"));
			}

			if (true == PlayerActors[0]->ActorHasTag("Cody"))
			{
				PlayerCody = Cast<ACody>(PlayerActors[0]);
				PlayerMay = Cast<AMay>(PlayerActors[1]);
			}
			else
			{
				PlayerCody = Cast<ACody>(PlayerActors[1]);
				PlayerMay = Cast<AMay>(PlayerActors[0]);
			}

		});

	FsmComp->CreateState(EBossState::Phase1_Progress_LaserBeam_1,
		[this]
		{
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
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
				FsmComp->ChangeState(EBossState::Phase1_Progress_LaserBeam_3);

				// test
				++PatternDestroyCount;

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
			if (nullptr == OverlapCheckActor)
			{
				return;
			}

			APlayerBase* CurrentOverlapPlayer = OverlapCheckActor->GetCurrentOverlapPlayer();
			if (nullptr != CurrentOverlapPlayer)
			{
				if (true == CurrentOverlapPlayer->ActorHasTag("Cody"))
				{
					PlayerCody = Cast<ACody>(CurrentOverlapPlayer);
					if (nullptr != PlayerCody)
					{
						// �ڵ� Ŀ�� �����ΰ͵� üũ 
						bool bIsInteract = CurrentOverlapPlayer->GetIsInteract();
						if (true == bIsInteract)
						{
							Multicast_CheckCodyKeyPressedAndChangeState(bIsInteract);
						}
					}
				}
			}
		},

		[this]
		{

		});

	// �ڵ� �κη����ִ� ���� ��¦ ��� ����
	FsmComp->CreateState(EBossState::CodyHolding_Enter,
		[this]
		{
			// Multicast_SetFocusHoldingCody();

			// �����Ҷ� �ѹ� ������ ������ �ѹ� ������ 
			FVector TargetLocation = GetActorLocation();
			TargetLocation.Z = 0.0f;
			FVector StartLocation = PlayerCody->GetActorLocation();
			StartLocation.Z = 0.0f;

			FRotator TargetRotation = (TargetLocation - StartLocation).Rotation();
			TargetRotation.Yaw -= 14.0f;
			PlayerCody->SetActorRotation(TargetRotation);

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
				FsmComp->ChangeState(EBossState::CodyHolding_Low);
				return;
			}

			SetCodyHoldingEnter_CodyLocation();
		},

		[this]
		{
			bIsCodyHoldingEnter = false;
			bIsCodyHoldingLerpEnd = false;

			// Multicast_UnPossess();
		});

	// �ڵ� ��¦ ��� �ִ� ���� 
	FsmComp->CreateState(EBossState::CodyHolding_Low,
		[this]
		{
			// UFO �������� �ִϸ��̼� 
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Low_Anim"), 1, true);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_CodyHolding_Anim"), 1, true);
		},

		[this](float DT)
		{
			// ������Դٴ°� �̹� �������÷��̾ �ִٴ� �Ű�. ���� �˻� ���ʿ� ���� Ű�� �ѹ� �� �����ٸ� Ű�Է� ���·� ��ȯ 
			// �ٵ� ���� �÷��̾� ��ġ������ ���ѻ��´ϱ� nullptr �˻� �ѹ� ���ְ� �����Ұ���~~ 
			if (nullptr == OverlapCheckActor->GetCurrentOverlapPlayer())
			{
				return;
			}

			bool IsPressed = PlayerCody->GetIsInteract();
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
				FsmComp->ChangeState(EBossState::CodyHolding_InputKey);
				StateCompleteTime = 0.0f;
				return;
			}
		},

		[this]
		{
			
		});

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
			if (KeyInputTime <= 0.0f)
			{
				FsmComp->ChangeState(EBossState::CodyHolding_ChangeOfAngle_Reverse);
				return;
			}
				
			bool CodyInput = PlayerCody->GetIsInteract();
			if (true == CodyInput)
			{
				KeyInputTime = KeyInputAdditionalTime;
			}

			// ������ζ�� ���� ���ͷ�Ʈ üũ �ʿ�. 
			if (true)
			{
				Multicast_SetActivateUIComponent(CodyHoldingUIComp, false, true);
				FsmComp->ChangeState(EBossState::Phase1_ChangePhase_2);
			}


			//if (2.5f <= FsmComp->GetStateLiveTime())
			//{
			//	FsmComp->ChangeState(EBossState::Phase1_ChangePhase);
			//	
			//	// ���⼭ ���̽�����ġ ���� �ִϸ��̼��� ����ġ �޾ƿͼ� ����
			//	

			//	

			//	return;
			//}


			// ���� ���� ���ͷ�Ʈ ��ǲ���ε� ����� ���۾��ؼ� �׽�Ʈ�Ұ��� �ӽ÷� 2�ʵڿ� ��������ϴ� ���·� �׽�Ʈ.
			/*UE_LOG(LogTemp, Warning, TEXT("CodyHolding_InputKey Tick"));
			APlayerBase* OverlapPlayer = OverlapCheckActor->GetCurrentOverlapPlayer();*/
			/*if (OverlapPlayer != nullptr)
			{
				if (true == OverlapPlayer->ActorHasTag("May"))
				{
					
					PlayerMay = Cast<AMay>(OverlapPlayer);
					bool MayInput = PlayerMay->GetIsInteract();
					if (true == MayInput)
					{
						UE_LOG(LogTemp, Warning, TEXT("Key Input True"));
						FsmComp->ChangeState(EBossState::Phase1_ChangePhase);
						return;
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Key Input False"));
					}
				}
			}*/
				
			KeyInputTime -= DT;
		},

		[this]
		{
			bIsKeyInput = false;
			KeyInputTime = KeyInputMaxTime;

			Multicast_SetActivateUIComponent(MayLaserDestroyUIComp, false, true);
			
			PrevAnimBoneLocation = SkeletalMeshComp->GetBoneLocation(TEXT("Base"));
		});

	FsmComp->CreateState(EBossState::CodyHolding_ChangeOfAngle_Reverse,
		[this]
		{
			// ���� -> ������ �ִϸ��̼� 
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/BluePrints/ABP_EnemyFlyingSaucer_CodyHoldingRotationReverse"), 2, false);
			StateCompleteTime = 2.0f;
		},

		[this](float DT)
		{
			if (StateCompleteTime <= FsmComp->GetStateLiveTime())
			{
				FsmComp->ChangeState(EBossState::CodyHolding_Low);
				StateCompleteTime = 0.0f;
				return;
			}
		},

		[this]
		{
			
		});

	
	FsmComp->CreateState(EBossState::Phase1_ChangePhase_2,
		[this]
		{
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/PlayRoom_SpaceStation_BossFight_LaserRippedOff_FlyingSaucer_Anim"), 1, false);
		},

		[this](float DT)
		{
			// �ִϸ��̼��� ���� �Ǿ��� �� 
			if (false == SkeletalMeshComp->IsPlaying())
			{
				Multicast_HideLaserBaseBone();
				FsmComp->ChangeState(EBossState::Phase2_RotateSetting);
				PrevAnimBoneLocation = SkeletalMeshComp->GetBoneLocation(TEXT("Root"));
				return;
			}

			if (false == bIsCorretLocation)
			{
				SetActorLocation(PrevAnimBoneLocation);
				bIsCorretLocation = true;
			}
		},

		[this]
		{
			bIsCorretLocation = false;
			OverlapCheckActor->Destroy();
		});

	
	FsmComp->CreateState(EBossState::Phase2_RotateSetting,
		[this]
		{
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Left_Anim"), 1, true);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
			RotatingComp->PivotTranslation = RotatePivotVector;
		},

		[this](float DT)
		{
			// ���� ��ġ���� ����ȵǾ� ������ �����ϰ�.
			if (false == bIsCorretLocation)
			{
				SetActorLocation(PrevAnimBoneLocation);
				bIsCorretLocation = true;
			}

			FsmComp->ChangeState(EBossState::Phase2_Rotating);
		},

		[this]
		{
			RotatingComp->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
			bIsCorretLocation = false;
		});

	FsmComp->CreateState(EBossState::Phase2_Rotating,
		[this]
		{
			// ���� ���� ������Ʈ�� �̻��� ��Ʈ ������Ʈ ���ٸ� �ִϸ��̼� ��� 
			if (static_cast<int32>(EBossState::Phase2_RocketHit) == FsmComp->GetCurrentState())
			{
				Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Left_Anim"), 1, true);
			}

			RotatingComp->RotationRate = FRotator(0.0f, 7.0f, 0.0f);
		},

		[this](float DT)
		{
			if (CurrentHp <= 33.0f)
			{
				FsmComp->ChangeState(EBossState::Phase2_BreakThePattern);
				return;
			}

			// �������� �߻� 
			HomingRocketFireTime -= DT;
			if (0.0f >= HomingRocketFireTime)
			{
				FireHomingRocket();
				HomingRocketFireTime = HomingRocketCoolTime;
			}
		},

		[this]
		{
			HomingRocketFireTime = 3.0f;
		});

	FsmComp->CreateState(EBossState::Phase2_RocketHit,
		[this]
		{
			UE_LOG(LogTemp, Warning, TEXT("RocketHit State Start"));

			// ���ּ� ��Ʈ �ִϸ��̼� ���� 
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Laser_HitPod_Anim"), 1, false);
		},

		[this](float DT)
		{
			// �ִϸ��̼� ����� Phase2_Rotating���� ���º���
			if (false == SkeletalMeshComp->IsPlaying())
			{
				FsmComp->ChangeState(EBossState::Phase2_Rotating);
				return;
			}
		},

		[this]
		{

		});



	FsmComp->CreateState(EBossState::Phase2_BreakThePattern,
		[this]
		{
			RotatingComp->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
			
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/BluePrints/ABP_EnemyFlyingSaucer_RocketPhaseEnd"), 2, false);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/CutScenes/PlayRoom_SpaceStation_BossFight_RocketsPhaseFinished_MoonBaboon_Anim"), 1, false);
		},

		[this](float DT)
		{
			// ���⼭ ���ּ� �ִϸ��̼� ��� �Ϸ�� 3������ ���� �����·� ����
			// �긦 �ٸ��ڵ�� �����ؾ���, ���� ���� 
			if (6.5f <= FsmComp->GetStateLiveTime())
			{
				FsmComp->ChangeState(EBossState::Phase2_ChangePhase_Wait);
				return;
			}
		},

		[this]
		{
			
		});

	FsmComp->CreateState(EBossState::Phase2_ChangePhase_Wait,
		[this]
		{
			// ������ Ÿ�ڿ�����ġ�� �ִϸ��̼����� ����
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_KnockDownMh_Anim"), 1, true);
		},

		[this](float DT)
		{
			// ���Ǹ����� 
			
			// �ӽ�,
			if (2.0f <= FsmComp->GetStateLiveTime())
			{
				FsmComp->ChangeState(EBossState::Phase2_Fly);
				return;
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase2_Fly,
		[this]
		{
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_EnterUFO_FlyingSaucer_Anim"), 1, false);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/CutScenes/PlayRoom_SpaceStation_BossFight_EnterUFO_MoonBaboon_Anim"), 1, false);
		},

		[this](float DT)
		{
			// ���ּ� �ִϸ��̼� ��� �Ϸ�� �߾����� �̵�
			if (false == SkeletalMeshComp->IsPlaying())
			{
				FsmComp->ChangeState(EBossState::Phase2_MoveToCenter);
				return;
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase2_MoveToCenter,
		[this]
		{
			// ���ּ� �������̵� �ִϸ��̼� ����, 
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Fwd_Anim"), 1, false);
			MoveStartLocation = GetActorLocation();
		},

		[this](float DT)
		{
			MoveToCenterLerpRatio += DT / 2.0f;
			if (1.0f <= MoveToCenterLerpRatio)
			{
				MoveToCenterLerpRatio = 1.0f;
				FVector TargetLocation = FMath::Lerp(MoveStartLocation, FVector(0.0f, 0.0f, MoveStartLocation.Z), MoveToCenterLerpRatio);
				SetActorLocation(TargetLocation);
				FsmComp->ChangeState(EBossState::Phase3_MoveFloor);
				FloorObject->SetPhase(AFloor::Fsm::Phase3);
				return;
			}
			
			FVector TargetLocation = FMath::Lerp(MoveStartLocation, FVector(0.0f, 0.0f, MoveStartLocation.Z), MoveToCenterLerpRatio);
			SetActorLocation(TargetLocation);
		},

		[this]
		{
			MoveToCenterLerpRatio = 0.0f;
			MoveStartLocation = FVector::ZeroVector;
			// �߾����� �̵� ���� �Ǿ��� �� ������ �����ִϸ��̼� ����
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		});

	FsmComp->CreateState(EBossState::Phase3_MoveFloor,
		[this]
		{
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},
		
		[this](float DT)
		{
			if (static_cast<int32>(AFloor::Fsm::KeepPhase) == FloorObject->GetCurrentPhase())
			{
				FsmComp->ChangeState(EBossState::Phase3_MoveToTarget);
			}

		},

		[this]
		{
			UE_LOG(LogTemp, Warning, TEXT("Phase3_MoveToTarget Start"));

		});

	FsmComp->CreateState(EBossState::Phase3_MoveToTarget,
		[this]
		{
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Fwd_Anim"), 1, false);
			MoveStartLocation = GetActorLocation();
			GroundPoundTargetLocation = PlayerCody->GetActorLocation();
			GroundPoundTargetLocation.Z = MoveStartLocation.Z;

			AController* BossController = GetController();
			if (nullptr == BossController)
			{
				UE_LOG(LogTemp, Warning, TEXT("Boss Controller is nullptr"));
				return;
			}

			// ��Ŀ���� �ӽ÷� �ڵ� ����, 
			AFlyingSaucerAIController* Controller = Cast<AFlyingSaucerAIController>(GetController());
			if (nullptr != Controller)
			{
				Controller->SetFocus(Cast<AActor>(PlayerCody));
				// UE_LOG(LogTemp, Warning, TEXT("Player Cody Focus"));
			}

			// test
			bIsEject = true;
		},

		[this](float DT)
		{
			// ����Ʈ��ư ���ȴ��� üũ 
			/*if (true == bIsEject)
			{
				FsmComp->ChangeState(EBossState::Phase3_Eject);
				return;
			}*/


			MoveToTargetLerpRatio += DT;
			if (1.0f <= MoveToTargetLerpRatio)
			{
				MoveToTargetLerpRatio = 1.0f;
				FVector TargetLocation = FMath::Lerp(MoveStartLocation, GroundPoundTargetLocation, MoveToTargetLerpRatio);
				SetActorLocation(TargetLocation);
				FsmComp->ChangeState(EBossState::Phase3_GroundPounding);
				return;
			}

			FVector TargetLocation = FMath::Lerp(MoveStartLocation, GroundPoundTargetLocation, MoveToTargetLerpRatio);
			SetActorLocation(TargetLocation);
		},

		[this]
		{
			MoveToTargetLerpRatio = 0.0f;
			MoveStartLocation = FVector::ZeroVector;
		});

	FsmComp->CreateState(EBossState::Phase3_GroundPounding,
		[this]
		{
			// �׶����Ŀ�� �ִϸ��̼� 
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_GroundPound_Anim"), 1, false);

			// test
			bIsEject = true;
		},

		[this](float DT)
		{
			// ����Ʈ��ư ���ȴ��� üũ 
		/*	if (true == bIsEject)
			{
				FsmComp->ChangeState(EBossState::Phase3_Eject);
				return;
			}*/


			if (false == SkeletalMeshComp->IsPlaying())
			{
				FsmComp->ChangeState(EBossState::Phase3_MoveToTarget);
				return;
			}

			if (GroundPoundEffectCreateTime <= FsmComp->GetStateLiveTime() && false == bIsSetGroundPoundEffect)
			{
				Multicast_CreateGroundPoundEffect();
				bIsSetGroundPoundEffect = true;
			}

		},

		[this]
		{
			bIsSetGroundPoundEffect = false;
		});

	FsmComp->CreateState(EBossState::Phase3_Eject,
		[this]
		{
			// ī�޶� �����ҰŶ� �׳� Z��ġ + 200���ְ� ī�޶� ������ȯ �ϸ� �ɵ�. 
			// Ż��ִϸ��̼� ����
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_Eject_FlyingSaucer_Anim"), 1, false);
		},

		[this](float DT)
		{
		},

		[this]
		{
		});



	FsmComp->CreateState(EBossState::TestState,
		[this]
		{
			// �׶����Ŀ�� �ִϸ��̼� 
			// Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_GroundPound_Anim"), 1, true);
			
			//Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_Eject_FlyingSaucer_Anim"), 1, false);
			
		},

		[this](float DT)
		{
			FVector TargetLocation = GetActorLocation() + FVector(100.0f, 0.0f, 0.0f) * DT;
			SetActorLocation(TargetLocation);
			AddActorLocalRotation(FRotator(1.0f, 0.0f, 0.0f));
		},

		[this]
		{
		});
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
	switch (PatternDestroyCount)
	{
	case 0:
		for (AActor* Actor : PlayerActors)
		{
			if (nullptr != Actor && Actor->ActorHasTag(TEXT("May")))
			{
				LaserTargetActor = Actor;
			}
		}
	
		break;
	case 1:
		for (AActor* Actor : PlayerActors)
		{
			if (nullptr != Actor && Actor->ActorHasTag(TEXT("Cody")))
			{
				LaserTargetActor = Actor;
			}
		}
		break;
	case 2:
		for (AActor* Actor : PlayerActors)
		{
			if (nullptr != Actor && Actor->ActorHasTag(TEXT("May")))
			{
				LaserTargetActor = Actor;
			}
		}
		break;
	}
}



void AEnemyFlyingSaucer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// �ش� ���Ͱ� ȣ�� �����̰�, �÷��̾� ��Ţ ���¶�� 
	if (nullptr != OtherActor && true == OtherActor->ActorHasTag(TEXT("HomingRocket")))
	{
		AHomingRocket* HomingRocket = Cast<AHomingRocket>(OtherActor);
		int32 RocketStateToInt = HomingRocket->GetCurrentState();
		if (static_cast<int32>(AHomingRocket::ERocketState::PlayerEquip) == RocketStateToInt)
		{
			FsmComp->ChangeState(EBossState::Phase2_RocketHit);
		}
	}
}

void AEnemyFlyingSaucer::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// �ϴ�.. ������ ����� �÷��̾��̰�.. �ö��̻��¸�.. .. �ϴܵ�����������
	// UE_LOG(LogTemp, Warning, TEXT("Boss Overlap Begin End Check"));
}

void AEnemyFlyingSaucer::SetupOverlapEvent()
{
	if (nullptr != SkeletalMeshComp)
	{
		SkeletalMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyFlyingSaucer::OnOverlapBegin);
		SkeletalMeshComp->OnComponentEndOverlap.AddDynamic(this, &AEnemyFlyingSaucer::OnOverlapEnd);
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
