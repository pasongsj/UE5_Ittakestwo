// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSaucerAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Cody.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "EnemyFlyingSaucer.h"
#include "TimerManager.h"
#include "EnemyMoonBaboon.h"

AFlyingSaucerAIController::AFlyingSaucerAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentBehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("CurrentBehaviorTreeCompnent"));
}

void AFlyingSaucerAIController::AddPatternMatchCount()
{
	++PatternMatchCount;
	GetBlackboardComponent()->SetValueAsInt(TEXT("PatternMatchCount"), PatternMatchCount);
}

void AFlyingSaucerAIController::BeginPlay()
{
	Super::BeginPlay();
	
	SetupPlayerReference();
	SetupStartBehaviorTreePhase1();
	GetBlackboardComponent()->SetValueAsVector(TEXT("PrevTargetLocation"), PrevTargetLocation);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void AFlyingSaucerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		UpdateLerpRatioForLaserBeam(DeltaTime);
	}
}

void AFlyingSaucerAIController::SetupPlayerReference()
{
	// ���� ��ġ�� �޾ƿ´�. 
	PlayerRef1 = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	PlayerRef2 = UGameplayStatics::GetPlayerPawn(GetWorld(), 1);
}

void AFlyingSaucerAIController::SetupStartBehaviorTreePhase1()
{
	if (nullptr != AIBehaviorTreePhase1)
	{
		RunBehaviorTree(AIBehaviorTreePhase1);
		CurrentBehaviorTree = AIBehaviorTreePhase1;
		CurrentBehaviorTreeComp->StartTree(*AIBehaviorTreePhase1, EBTExecutionMode::Looped);
		GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerCody"), PlayerRef1);
		GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerMay"), PlayerRef2);
		GetBlackboardComponent()->SetValueAsInt(TEXT("Phase1TargetCount"), 1);
		GetBlackboardComponent()->SetValueAsInt(TEXT("PatternMatchCount"), PatternMatchCount);
	}
}

void AFlyingSaucerAIController::SetupStartBehaviorTreePhase2()
{
	if (nullptr != CurrentBehaviorTree && nullptr != AIBehaviorTreePhase2)
	{
		CurrentBehaviorTreeComp->StopTree();
		RunBehaviorTree(AIBehaviorTreePhase2);
		CurrentBehaviorTreeComp->StartTree(*AIBehaviorTreePhase2);
		CurrentBehaviorTree = AIBehaviorTreePhase2;
	}
}

void AFlyingSaucerAIController::SetupStartBehaviorTreePhase3()
{
	if (nullptr != CurrentBehaviorTree && nullptr != AIBehaviorTreePhase3)
	{
		CurrentBehaviorTreeComp->StopTree();
		RunBehaviorTree(AIBehaviorTreePhase3);
		CurrentBehaviorTreeComp->StartTree(*AIBehaviorTreePhase3);
		CurrentBehaviorTree = AIBehaviorTreePhase3;
	}
}

void AFlyingSaucerAIController::SavePreviousTargetLocation()
{
	APawn* TargetPawn = Cast<APawn>(GetBlackboardComponent()->GetValueAsObject(TEXT("LaserBeamTarget")));
	
	if (nullptr != TargetPawn)
	{
		FVector CurrentTargetLocation = TargetPawn->GetActorLocation();

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
		GetBlackboardComponent()->SetValueAsVector(TEXT("PrevTargetLocation"), PrevTargetLocation);
		PrevTargetLocationBuffer = CurrentTargetLocation;
		bPrevTargetLocationValid = true;
	}

}

void AFlyingSaucerAIController::UpdateLerpRatioForLaserBeam(float DeltaTime)
{
	LaserLerpRatio += DeltaTime * LaserLerpRate;
	if (1.0f <= LaserLerpRatio)
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("PrevLaserAttackLocation"), PrevTargetLocation);
		SavePreviousTargetLocation();
		LaserLerpRatio -= 1.0f;
	}

	GetBlackboardComponent()->SetValueAsFloat(TEXT("LaserLerpRatio"), LaserLerpRatio);
}

