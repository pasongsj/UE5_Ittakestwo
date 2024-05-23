// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSaucerAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Net/UnrealNetwork.h"
#include "EnemyFlyingSaucer.h"
#include "TimerManager.h"
#include "EnemyMoonBaboon.h"
#include "ITTGameModeBase.h"
#include "Cody.h"

AFlyingSaucerAIController::AFlyingSaucerAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);
		CurrentBehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("CurrentBehaviorTreeCompnent"));
	}
}

void AFlyingSaucerAIController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// �޽� ������Ʈ�� Replication�ϱ� ���� ���� �߰�
	DOREPLIFETIME(AFlyingSaucerAIController, AIBehaviorTreePhase1);
	DOREPLIFETIME(AFlyingSaucerAIController, CurrentBehaviorTree);
	DOREPLIFETIME(AFlyingSaucerAIController, CurrentBehaviorTreeComp);
}

void AFlyingSaucerAIController::BeginPlay()
{
	Super::BeginPlay();
	
	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		
	}
}

void AFlyingSaucerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		// �÷��̾� �¾��� ���� �ʾ��� �� 
		if (false == bIsSetupPlayerRef)
		{
			SetupPlayerRefAndBehaviorTreePhase1();
		}
	}
}

void AFlyingSaucerAIController::SetupPlayerRefAndBehaviorTreePhase1()
{
	// ���⼭ ���Ӹ�带 �޾ƿͼ� ī��Ʈ�� 2�� �Ǿ����� Ȯ���ϰ� 2�� �Ǿ��ٸ� bool ���� true�� ����
	AITTGameModeBase* CurrentGameMode = Cast<AITTGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (nullptr != CurrentGameMode)
	{
		int32 LoginCount = CurrentGameMode->GetPlayerLoginCount();
		if (2 == LoginCount)
		{
			SetupStartBehaviorTreePhase1();
			bIsSetupPlayerRef = true;
		}
	}
}

void AFlyingSaucerAIController::SetupStartBehaviorTreePhase1()
{
	if (nullptr != AIBehaviorTreePhase1)
	{
		RunBehaviorTree(AIBehaviorTreePhase1);
		CurrentBehaviorTree = AIBehaviorTreePhase1;
		CurrentBehaviorTreeComp->StartTree(*AIBehaviorTreePhase1, EBTExecutionMode::Looped);
		GetBlackboardComponent()->SetValueAsInt(TEXT("Phase1TargetCount"), 1);
	}
}