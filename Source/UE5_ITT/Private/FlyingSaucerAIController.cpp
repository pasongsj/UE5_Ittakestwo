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

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{

	}
}

void AFlyingSaucerAIController::SetupPlayerReference()
{
	// ���� ��ġ�� �޾ƿ´�. 
	PlayerCody = Cast<ACody>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AFlyingSaucerAIController::SetupBehaviorTree()
{
	if (nullptr != AIBehaviorTree)
	{
		RunBehaviorTree(AIBehaviorTree);
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerCody->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("CurrentActorLocation"), GetPawn()->GetActorLocation());
	}
}

