// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSaucerAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Cody.h"
#include "BehaviorTree/BlackboardComponent.h"

AFlyingSaucerAIController::AFlyingSaucerAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlyingSaucerAIController::BeginPlay()
{
	Super::BeginPlay();

	// �÷��̾� �� ����
	SetupPlayerReference();
	SetupBehaviorTree();

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

	if (nullptr != PlayerCodyRef)
	{
		if (LineOfSightTo(PlayerCodyRef))
		{
			//GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerCodyRef->GetActorLocation());
		}

		else
		{
			//GetBlackboardComponent()->ClearValue("PlayerLocation");
		}
	}


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

void AFlyingSaucerAIController::SetupBehaviorTree()
{
	if (nullptr != AIBehaviorTree)
	{
		RunBehaviorTree(AIBehaviorTree);
		GetBlackboardComponent()->SetValueAsObject(TEXT("SelfActor"), GetPawn());
		GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerCodyRef"), PlayerCodyRef);
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerCodyRef->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("CurrentActorLocation"), GetPawn()->GetActorLocation());
		GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentBossPhase"), static_cast<uint8>(CurrentBossPhase));
	}
}

