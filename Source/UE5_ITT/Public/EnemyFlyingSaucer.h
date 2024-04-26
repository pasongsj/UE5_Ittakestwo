// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyFlyingSaucer.generated.h"

UCLASS()
class UE5_ITT_API AEnemyFlyingSaucer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyFlyingSaucer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// 2������ ���� �������� ���� pivot actor ����, ����, ���� �� ȸ�� ����
	UFUNCTION(BlueprintCallable)
	void SetupRotateCenterPivotActor();

	UFUNCTION(BlueprintCallable)
	void DetachRotateCenterPivotActor();

	UFUNCTION(BlueprintCallable)
	ABossRotatePivotActor* GetRotateCenterPivotActor() { return RotateCenterPivotActor; }

	UFUNCTION(BlueprintCallable)
	void RotationCenterPivotActor(float DeltaTime);

	// ���� ���� �� ����ü �߻� ���� �Լ� 
	UFUNCTION(BlueprintCallable)
	void FireHomingRocket();

	UFUNCTION(BlueprintCallable)
	void DisCountHomingRocketFireCount()
	{
		if (0 < HomingRocketFireCount)
		{
			--HomingRocketFireCount;
		}
	}

	UFUNCTION(BlueprintCallable)
	void FireArcingProjectile();

	UPROPERTY(BlueprintReadWrite, Replicated)
	float ArcingProjectileFireTime = 3.0f;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float ArcingProjectileMaxFireTime = 3.0f;


	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_CreateEnergyChargeEffect();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_SetFocusTarget();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_AttachToMoonBaboonActorAndFloor();

	// ü��(�ӽ�) 
	UFUNCTION(BlueprintCallable)
	void SetCurrentHp(float HpValue) { CurrentHp = HpValue; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentHp() const { return CurrentHp; }

	// Get, Set 
	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetLaserSpawnPointMesh() const { return LaserSpawnPointMesh; }

	UFUNCTION(BlueprintCallable)
	class AEnemyMoonBaboon* GetMoonBaboonActor() const { return EnemyMoonBaboon; }

	UFUNCTION(BlueprintCallable)
	AFloor* GetFloor() { return FloorObject; }

	// test, ���� �׽�Ʈ ������ ���� 
	void BossHitTestFireRocket();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	const bool IsCodyHoldingEnter() { return bIsCodyHoldingEnter; }


	// ������ ���� ���� ���� ����
	UPROPERTY(BlueprintReadWrite, Replicated)
	FVector PrevTargetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Replicated)
	FVector PrevTargetLocationBuffer = FVector::ZeroVector;	

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bPrevTargetLocationValid = false;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float LaserLerpRatio = 0.0f;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float LaserLerpRate = 25.0f;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 LaserFireCount = 0;
 
	UPROPERTY(VisibleDefaultsOnly)
	class AActor* LaserTargetActor = nullptr;

	UPROPERTY(VisibleDefaultsOnly)
	TArray<AActor*> PlayerActors;

	UPROPERTY(BlueprintReadWrite)
	float CoreExplodeDamage = 11.0f;

	UFUNCTION(BlueprintCallable)
	void SetDamage(const float Damage)
	{
		CurrentHp -= Damage;
		if (CurrentHp <= 0.0f)
		{
			CurrentHp = 0.0f;
		}
	}

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 PatternDestroyCount = 0;

	UFUNCTION(BlueprintCallable)
	void AddPatternDestoryCount() { ++PatternDestroyCount; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	enum class EBossState
	{
		None,

		Intro,						 
		Phase1_Progress_LaserBeam_1,			
		Phase1_Progress_LaserBeam_1_Destroy,
		Phase1_Progress_ArcingProjectile_1,

		Phase1_Progress_LaserBeam_2,			
		Phase1_Progress_LaserBeam_2_Destroy,
		Phase1_Progress_ArcingProjectile_2,

		Phase1_Progress_LaserBeam_3,
		Phase1_Progress_LaserBeam_3_Destroy,
		Phase1_Progress_ArcingProjectile_3,

		Phase1_BreakThePattern,

		CodyHolding_Enter,
		CodyHolding_Low,
		CodyHolding_ChangeOfAngle,
		CodyHolding_InputKey,
		CodyHolding_ChangeOfAngle_Reverse,



		FireHomingRocket,
		FireArcingProjectile,

	};

	enum class EAnimationAssetType : uint8
	{
		None,
		Sequence,
		Blueprint,
	};

	// �ٴڻ��°� �޾ƿ���
	int32 GetFloorCurrentState();
	
	// multicast �Լ� 
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangeAnimationFlyingSaucer(const FString& AnimPath, const uint8 AnimType, bool AnimLoop);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangeAnimationMoonBaboon(const FString& AnimPath, const uint8 AnimType, bool AnimLoop);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_CheckCodyKeyPressedAndChangeState();

	void DrawDebugMesh();

	UPROPERTY(EditDefaultsOnly)
	float ServerDelayTime = 4.0f;

	void SetupComponent();

	UFUNCTION(BlueprintCallable)
	void SetupFsmComponent();
	
	// ���� ���ѽ� �÷��̾� �߰� Ű �Է� ���� ���� 
	UPROPERTY(EditDefaultsOnly)
	bool bIsKeyInput = false;
	float KeyInputTime = 0.0f;
	float KeyInputMaxTime = 1.25f;
	float KeyInputAdditionalTime = 0.75f;

	UPROPERTY(EditAnywhere)
	float CurrentHp = 100.0f;

	UPROPERTY(EditAnywhere)
	float MaxHp = 100.0f;

	// Ư���ð� ���� State ���� �� �ش� ���� ���
	UPROPERTY(EditDefaultsOnly)
	float StateCompleteTime = 0.0f;

	// tsubclass 
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEnemyMoonBaboon> EnemyMoonBaboonClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AHomingRocket> HomingRocketClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AArcingProjectile> ArcingProjectileClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AFloor> FloorClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEnergyChargeEffect> EnergyChargeEffectClass = nullptr;

	// ���� ����ü ���� 
	UFUNCTION(BlueprintCallable)
	void ResetArcingProjectileFireCount() { ArcingProjectileFireCount = 0; }
	
	UPROPERTY(EditAnywhere)
	int32 ArcingProjectileFireCount = 0;

	UPROPERTY(EditAnywhere)
	int32 HomingRocketFireCount = 0;

	// ���� 2������ ȸ���� �������� �Ǵ� ���� 
	UPROPERTY(EditDefaultsOnly)
	class ABossRotatePivotActor* RotateCenterPivotActor = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ABossRotatePivotActor> RotateCenterPivotActorClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float RotateCenterPivotActorMoveSpeed = 9.0f;

	// ������ �����ϴ� ���� 
	UPROPERTY(Replicated, EditDefaultsOnly)
	class AEnemyMoonBaboon* EnemyMoonBaboon = nullptr;

	UPROPERTY(EditAnywhere)
	class AFloor* FloorObject = nullptr;
	
	UPROPERTY(Replicated, EditDefaultsOnly)
	class AEnergyChargeEffect* EnergyChargeEffect = nullptr;

	// Component
	UPROPERTY(Replicated, EditDefaultsOnly)
	class UFsmComponent* FsmComp = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class UInteractionUIComponent* UIComp = nullptr;

	UPROPERTY(Replicated, EditAnywhere)
	class USkeletalMeshComponent* SkeletalMeshComp = nullptr;


	UFUNCTION(BlueprintCallable)
	void ActivateUIComponent();

	// ������ üũ ���� 
	UFUNCTION(BlueprintCallable)
	void SpawnOverlapCheckActor();

	UPROPERTY(Replicated, EditDefaultsOnly)
	class AOverlapCheckActor* OverlapCheckActor = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AOverlapCheckActor> OverlapCheckActorClass = nullptr;

	

	// ���� ���� ���� ���� ������Ʈ 
	UPROPERTY(Replicated, EditDefaultsOnly)
	class UStaticMeshComponent* LaserSpawnPointMesh = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class UStaticMeshComponent* HomingRocketSpawnPointMesh1 = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class UStaticMeshComponent* HomingRocketSpawnPointMesh2 = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class UStaticMeshComponent* ArcingProjectileSpawnPointMesh = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class UAnimInstance* AnimInstance = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class UAnimSequence* AnimSequence = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	bool bIsCodyHoldingEnter = false;


	// ������ ���� ���� ����
	void UpdateLerpRatioForLaserBeam(float DeltaTime);
	void SavePreviousTargetLocation();

	UFUNCTION(BlueprintCallable)
	void SetupLaserTargetActor();
};
