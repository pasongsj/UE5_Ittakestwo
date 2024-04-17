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
#include "FsmComponent.h"
#include "InteractionUIComponent.h"
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
#include "DrawDebugHelpers.h"

// Sets default values
AEnemyFlyingSaucer::AEnemyFlyingSaucer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetupComponent();
	SetupFsmComponent();
	Tags.Add(FName("Boss"));
}

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

// Called when the game starts or when spawned
void AEnemyFlyingSaucer::BeginPlay()
{
	Super::BeginPlay();

	EnemyMoonBaboon = GetWorld()->SpawnActor<AEnemyMoonBaboon>(EnemyMoonBaboonClass);
	EnemyMoonBaboon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("ChairSocket"));
	EnemyMoonBaboon->SetOwner(this);

	FsmComp->ChangeState(EBossState::Phase1Start);
	// BossHitTestFireRocket();

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

// ���߿� ���� 
void AEnemyFlyingSaucer::ChangeAnimationFlyingSaucer(const FName& AnimationName)
{
	FName NewAnimationName = AnimationName;
	if (false == AnimationName.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Animation Name is not valid"));
		return;
	}
}

void AEnemyFlyingSaucer::ChangeAnimationMoonBaboon(const FName& AnimationName)
{
	FName NewAnimationName = AnimationName;
	if (false == AnimationName.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Animation Name is not valid"));
		return;
	}
}

// Called every frame
void AEnemyFlyingSaucer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		

		//DrawDebugMesh();
	}
}

// Called to bind functionality to input
void AEnemyFlyingSaucer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyFlyingSaucer::SetupRotateCenterPivotActor()
{
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
	++ArcingProjectileFireCount;

	AActor* FloorActor = Cast<AActor>(GetFloor());
	AArcingProjectile* Projectile = GetWorld()->SpawnActor<AArcingProjectile>(ArcingProjectileClass, ArcingProjectileSpawnPointMesh->GetComponentLocation(), FRotator::ZeroRotator);
	AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
	if (nullptr != Projectile && nullptr != AIController)
	{
		FVector TargetLocation = FVector::ZeroVector;
		if (ArcingProjectileFireCount % 2 == 0)
		{
			AActor* TargetActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerCody")));
			TargetLocation = TargetActor->GetActorLocation();
			
		}

		else if (ArcingProjectileFireCount % 2 == 1)
		{
			AActor* TargetActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerMay")));
			TargetLocation = TargetActor->GetActorLocation();
		}

		Projectile->SetupTargetLocation(TargetLocation);
	}
	
	Projectile->SetupProjectileMovementComponent();
	Projectile->SetOwner(this);
	if (Projectile != nullptr)
	{
		Projectile->AttachToActor(FloorActor, FAttachmentTransformRules::KeepWorldTransform);
	}
}

AEnergyChargeEffect* AEnemyFlyingSaucer::CreateEnergyChargeEffect()
{
	EnergyChargeEffect = nullptr;
	EnergyChargeEffect = GetWorld()->SpawnActor<AEnergyChargeEffect>(EnergyChargeEffectClass,LaserSpawnPointMesh->GetComponentLocation(), FRotator::ZeroRotator);
	EnergyChargeEffect->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("EnergyChargeEffectSpawnPointSocket"));

	return EnergyChargeEffect;
}

void AEnemyFlyingSaucer::SetupComponent()
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	SetRootComponent(CapsuleComp);
	GetArrowComponent()->SetupAttachment(CapsuleComp);
	GetCharacterMovement()->SetUpdatedComponent(GetCapsuleComponent());
	
	USkeletalMeshComponent* SkeletalMeshComp = GetMesh();
	SkeletalMeshComp->SetupAttachment(CapsuleComp);

	LaserSpawnPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserSpawnPointMesh"));
	LaserSpawnPointMesh->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("LaserSpawnPointSocket"));

	HomingRocketSpawnPointMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HomingRocketSpawnPointMesh1"));
	HomingRocketSpawnPointMesh1->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("HomingRocketSpawnPointSocket_01"));

	HomingRocketSpawnPointMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HomingRocketSpawnPointMesh2"));
	HomingRocketSpawnPointMesh2->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("HomingRocketSpawnPointSocket_02"));

	ArcingProjectileSpawnPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArcingProjectileSpawnPointMesh"));
	ArcingProjectileSpawnPointMesh->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("ArcingProjectileSpawnPointSocket"));

	UIComp = CreateDefaultSubobject<UInteractionUIComponent>(TEXT("InteractionUIComponent"));
	UIComp->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("UISocket"));
	
	// �θ� ��Ȱ��, �ڽ� ��Ȱ�� 
	UIComp->SetVisibility(false, true);
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

void AEnemyFlyingSaucer::StartMotionUpdate(float DeltaTime)
{
	FVector CurrentLocation = GetMesh()->GetRelativeLocation();
	if (CurrentLocation.Z >= StartMotionTargetLocation.Z)
	{
		bIsStartMotion = false;
		return;
	}

	float MoveSpeed = 150.0f;
	FVector DeltaMovement = FVector(0.0f, 0.0f, MoveSpeed * DeltaTime);
	FVector NewLocaiton = CurrentLocation + DeltaMovement;

	GetMesh()->SetRelativeLocation(NewLocaiton);
}

void AEnemyFlyingSaucer::SetupFsmComponent()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComponent"));
	FsmComp->CreateState(EBossState::Phase1Start,
		[this]
		{
			// �޽ù޾ƿ���
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// �ִ��ν��Ͻ� �޾ƿͼ� �ִϸ��̼� ���
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_EnterUFO_FlyingSaucer_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, false);
				}
			}

			// MoonBaboon �ִϸ��̼� ����
			SkeletalMeshComponent = EnemyMoonBaboon->GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// �ִ��ν��Ͻ� �޾ƿͼ� �ִϸ��̼� ���
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Programming_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, false);
				}
			}
		},

		[this](float DT)
		{
			// �ִϸ��̼� ���� üũ 
			bool bIsAnimationPlaying = GetMesh()->IsPlaying();
			if (!bIsAnimationPlaying)
			{
				FsmComp->ChangeState(EBossState::Phase1Progress);
				return;
			}

		
		},

		[this]
		{
			// MoonBaboon �ִϸ��̼� ����
			USkeletalMeshComponent* SkeletalMeshComponent = EnemyMoonBaboon->GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// �ִ��ν��Ͻ� �޾ƿͼ� �ִϸ��̼� ���
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, true);
				}
			}
		});

	FsmComp->CreateState(EBossState::Phase1Progress,
		[this]
		{
			// FlyingSaucer �ִϸ��̼� ����
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// �ִ��ν��Ͻ� �޾ƿͼ� �ִϸ��̼� ���
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, true);
				}
			}

			
		},

		[this](float DT)
		{
			// ������ ���� �˻� �ӽ� 
			if (CurrentHp <= 67.0f)
			{
				FsmComp->ChangeState(EBossState::Phase1End);
				return;
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase1End,
		[this]
		{
			// ������ ��������� �߶� + ������ �ִϸ��̼� ���� �� 
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// �ִ��ν��Ͻ� �޾ƿͼ� �ִϸ��̼� ���
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_PowerCoresDestroyed_FlyingSaucer_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, false);
				}
			}

			// MoonBaboon �ִϸ��̼� ����
			SkeletalMeshComponent = EnemyMoonBaboon->GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// �ִ��ν��Ͻ� �޾ƿͼ� �ִϸ��̼� ���
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Programming_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, true);
				}
			}

			// 4.5�� �Ŀ� UI Ȱ��ȭ 
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyFlyingSaucer::ActivateUIComponent, 4.5f, false);

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
				// �ڵ��̰�.
				if (true == CurrentOverlapPlayer->ActorHasTag("Cody"))
				{
					ACody* PlayerCody = Cast<ACody>(CurrentOverlapPlayer);
					
					// �ڵ� Ŀ�� �����ΰ͵� üũ 
					bool bIsInteract = CurrentOverlapPlayer->GetIsInteract();
					CodySize Size = PlayerCody->GetCodySize();

					// ���� ���ͷ�Ʈ Ű �������� üũ
					if (true == bIsInteract && CodySize::BIG == Size)
					{
						UE_LOG(LogTemp, Warning, TEXT("Change State : CodyHolding Start"));

						// �������� ChangeState 
						FsmComp->ChangeState(EBossState::CodyHoldingStart);
						return;
					}
				}
			}
		},

		[this]
		{

		});


	FsmComp->CreateState(EBossState::CodyHoldingStart,
		[this]
		{
			// eŰ ������ �� ���� ������ 
			// ���ּ� -> �ڵ�Ȧ�� ���۸��
			// ������ -> �� �̻��� ��� loop �� ����

			// ������ ��������� �߶� + ������ �ִϸ��̼� ���� �� 
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// �ִ��ν��Ͻ� �޾ƿͼ� �ִϸ��̼� ���
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Enter_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, false);
				}
			}

			// MoonBaboon �ִϸ��̼� ����
			SkeletalMeshComponent = EnemyMoonBaboon->GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// �ִ��ν��Ͻ� �޾ƿͼ� �ִϸ��̼� ���
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_CodyHolding_Enter_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, false);
				}
			}
		},

		[this](float DT)
		{
			USkeletalMeshComponent* FlyingSaucerMesh = GetMesh();
			USkeletalMeshComponent* MoonBaboonMesh = EnemyMoonBaboon->GetMesh();
			if (false == FlyingSaucerMesh->IsPlaying() && false == MoonBaboonMesh->IsPlaying())
			{
				UE_LOG(LogTemp, Warning, TEXT("Change State CodyHoldingProgress_NotKeyMashing"));
				FsmComp->ChangeState(EBossState::CodyHoldingProgress_NotKeyMashing);
				return;
			}
		},

		[this]
		{

		});

	// ��� �ִ� ����, Ű��Ÿ X, ���⼭ ��ȣ�ۿ�Ű�� �ѹ��� �Է��ߴٸ� 
	// ���� ���� ���·� ��ȯ 
	FsmComp->CreateState(EBossState::CodyHoldingProgress_NotKeyMashing,
		[this]
		{
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// �ִ��ν��Ͻ� �޾ƿͼ� �ִϸ��̼� ���
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Low_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, true);
				}
			}

			// MoonBaboon �ִϸ��̼� ����
			SkeletalMeshComponent = EnemyMoonBaboon->GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// �ִ��ν��Ͻ� �޾ƿͼ� �ִϸ��̼� ���
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_CodyHolding_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, true);
				}
			}
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
				// �ڵ��̰�.
				if (true == CurrentOverlapPlayer->ActorHasTag("Cody"))
				{
					// �ڵ� Ŀ�� �����ΰ͵� üũ 
					bool bIsInteract = CurrentOverlapPlayer->GetIsInteract();

					// ���� ���ͷ�Ʈ Ű �������� üũ
					if (true == bIsInteract)
					{
						UE_LOG(LogTemp, Warning, TEXT("Change State : CodyHoldingProgress_ChangeOfAngle"));

						// �������� ChangeState 
						FsmComp->ChangeState(EBossState::CodyHoldingProgress_ChangeOfAngle);
						return;
					}
				}
			}
		},

		[this]
		{

		});

	// ���� ���� ���� ���⼭ ���� ���� ����
	FsmComp->CreateState(EBossState::CodyHoldingProgress_ChangeOfAngle,
		[this]
		{
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			const USkeletalMeshSocket* Socket = SkeletalMeshComponent->GetSocketByName(TEXT("RotationPivot"));
			FVector SpawnLocation = Socket->GetSocketLocation(SkeletalMeshComponent);

			/*USkeletalMeshComponent* SkeletalMeshComponent = OverlapCheckActor->GetCurrentOverlapPlayer()->GetMesh();
			FVector SpawnLocation = SkeletalMeshComponent->GetBoneLocation(TEXT("Neck"));*/
			PrevRotation = GetActorRotation();
			PrevLocation = GetActorLocation();
			// ���� �������� ���Ϳ��� ���� �� ����
			this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

			// �ǹ���ġ�� ���ͻ��� �� ���͸� �ӽ� ����
			CodyHoldingRotationPivotActor = GetWorld()->SpawnActor<ABossRotatePivotActor>(SpawnLocation, GetActorRotation());
			this->AttachToActor(CodyHoldingRotationPivotActor, FAttachmentTransformRules::KeepWorldTransform);

			LerpStartRotator = CodyHoldingRotationPivotActor->GetActorRotation();
			LerpEndRotator = LerpStartRotator + FRotator(ChangeAngle, 0.0f, 0.0f);
		},

		[this](float DT)
		{
			if (true == bIsLefpComplete)
			{
				FsmComp->ChangeState(EBossState::CodyHoldingProgress_KeyMashing);
				UE_LOG(LogTemp, Warning, TEXT("Change State : CodyHoldingProgress_KeyMashing"));
				return;
			}

			ChangeOfAngleRatio += DT / 2.0f;

			if (1.0f <= ChangeOfAngleRatio)
			{
				ChangeOfAngleRatio = 1.0f;
				bIsLefpComplete = true;
				
			}

			FRotator LerpedRotation = FMath::Lerp(LerpStartRotator, LerpEndRotator, ChangeOfAngleRatio);
			UE_LOG(LogTemp, Warning, TEXT("LerpedRotation : %f"), LerpedRotation.Pitch);
			CodyHoldingRotationPivotActor->SetActorRotation(LerpedRotation);

			FRotator Check = CodyHoldingRotationPivotActor->GetActorRotation();
			UE_LOG(LogTemp, Warning, TEXT("PivotActorRotation : %f"), Check.Pitch);

			FVector CheckActorLocation = CodyHoldingRotationPivotActor->GetActorLocation();

			UE_LOG(LogTemp, Warning, TEXT("PivotActorLocation : %s"), *CheckActorLocation.ToString());
		},

		[this]
		{
			this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			this->AttachToActor(RotateCenterPivotActor, FAttachmentTransformRules::KeepWorldTransform);

			//SetActorRotation(PrevRotation);
			//SetActorLocation(PrevLocation);
			// ����ȸ������ �ǵ������� 

			if (nullptr != CodyHoldingRotationPivotActor)
			{
				CodyHoldingRotationPivotActor->Destroy();
				CodyHoldingRotationPivotActor = nullptr;
			}

			ChangeOfAngleRatio = 0.0f;
			LerpStartRotator = FRotator::ZeroRotator;
			LerpEndRotator = FRotator::ZeroRotator;
			bIsLefpComplete = false;
		});


	FsmComp->CreateState(EBossState::CodyHoldingProgress_KeyMashing,
		[this]
		{
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// �ִ��ν��Ͻ� �޾ƿͼ� �ִϸ��̼� ���
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, true);
				}
			}
		},

		[this](float DT)
		{
			
		},

		[this]
		{

		});




	FsmComp->CreateState(EBossState::Phase2Start,
		[this]
		{
			
		},

		[this](float DT)
		{
			

		},

		[this]
		{
			
		});

	FsmComp->CreateState(EBossState::Phase2Progress,
		[this]
		{
		

		},

		[this](float DT)
		{

		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase2End,
		[this]
		{

		},

		[this](float DT)
		{

		},

		[this]
		{

		});
}

void AEnemyFlyingSaucer::ActivateUIComponent()
{
	if (nullptr != UIComp)
	{
		UIComp->SetVisibility(true, true);
	}
}

void AEnemyFlyingSaucer::SpawnOverlapCheckActor()
{
	OverlapCheckActor = GetWorld()->SpawnActor<AOverlapCheckActor>(OverlapCheckActorClass, GetActorLocation(), GetActorRotation());
	OverlapCheckActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("OverlapCheckActorSocket"));
	OverlapCheckActor->SetOverlapActorNameTag(TEXT("Player"));
}

