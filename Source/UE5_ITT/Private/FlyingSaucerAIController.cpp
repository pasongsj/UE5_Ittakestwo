// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSaucerAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Cody.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyFlyingSaucer.h"

AFlyingSaucerAIController::AFlyingSaucerAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlyingSaucerAIController::BeginPlay()
{
	Super::BeginPlay();

	ChangePhase(EBossPhase::Phase_1);
	SetupPlayerReference();

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

	// ü���� üũ�ؼ� ������ ��ȯ
	UpdatePhaseFromHealth(DeltaTime);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{

	}
}

void AFlyingSaucerAIController::SetupPlayerReference()
{
	// ���� ��ġ�� �޾ƿ´�. 
	PlayerCodyRef = Cast<ACody>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AFlyingSaucerAIController::UpdatePhaseFromHealth(float DeltaTime)
{
	// ü��üũ
	AEnemyFlyingSaucer* Boss = Cast<AEnemyFlyingSaucer>(GetPawn());
}

void AFlyingSaucerAIController::ChangePhase(EBossPhase Phase)
{
	// ����� �������� ���� �� Ŭ���� ����, �װ� �ƴ϶�� �������� ����
	if (CurrentBossPhase < Phase)
	{
		switch (Phase)
		{
		case AFlyingSaucerAIController::EBossPhase::Phase_1:
		{
			if (nullptr != AIBehaviorTreePhase1)
			{
				RunBehaviorTree(AIBehaviorTreePhase1);
			}
		}
			break;
		case AFlyingSaucerAIController::EBossPhase::Phase_2:
		{
			if (nullptr != AIBehaviorTreePhase2)
			{
				RunBehaviorTree(AIBehaviorTreePhase2);
			}
		}
			break;
		case AFlyingSaucerAIController::EBossPhase::Phase_3:
		{
			if (nullptr != AIBehaviorTreePhase3)
			{
				RunBehaviorTree(AIBehaviorTreePhase3);
			}
		}
			break;
		case AFlyingSaucerAIController::EBossPhase::Death:
			break;
		case AFlyingSaucerAIController::EBossPhase::Max:
			break;
		default:
			break;
		}
	}
	
}

