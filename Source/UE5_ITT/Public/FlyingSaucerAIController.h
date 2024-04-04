// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FlyingSaucerAIController.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API AFlyingSaucerAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlyingSaucerAIController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	APawn* GetPlayerPawn() const { return PlayerRef1; }
	
	UFUNCTION(BlueprintCallable)
	FVector GetTargetPrevLocation() const { return PrevTargetLocation; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	enum class EBossPhase
	{
		None,
		Phase_1,
		Phase_2,
		Phase_3,
		Death,
		Max
	};

	void SetupPlayerReference();
	void SetupStartBehaviorTreePhase1();
	void SavePreviousTargetLocation();
	void UpdateLerpRatioForLaserBeam(float DeltaTime);
	void UpdatePhaseFromHealth(float DeltaTime);
	void ChangePhase(EBossPhase Phase);

	// Player Ref
	UPROPERTY(VisibleDefaultsOnly, Category = "Player Character")
	class APawn* PlayerRef1;

	// Player Ref Test
	UPROPERTY(VisibleDefaultsOnly, Category = "Player Character")
	class APawn* PlayerRef2;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	bool bFocus = false;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float FocusRange = 300;

	// Behavior Tree
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* AIBehaviorTreePhase1;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* AIBehaviorTreePhase2;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* AIBehaviorTreePhase3;

	UPROPERTY(EditDefaultsOnly)
	class AFloor* FloorObject = nullptr;

	FTimerHandle TargetLocationCheckHandle;
	FVector PrevTargetLocation = FVector::ZeroVector;
	FVector PrevTargetLocationBuffer = FVector::ZeroVector;
	bool bPrevTargetLocationValid = false;
	float LaserLerpRatio = 0.0f;
	float LaserLerpRate = 25.0f;

	// phase
	EBossPhase CurrentBossPhase = EBossPhase::Phase_1;
};
