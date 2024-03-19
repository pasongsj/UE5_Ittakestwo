// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSaucerAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Cody.h"

AFlyingSaucerAIController::AFlyingSaucerAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlyingSaucerAIController::BeginPlay()
{
	Super::BeginPlay();

	// �÷��̾� �� ����
	SetPlayerReference();


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

	if (nullptr != AIBehaviorTree)
	{
		RunBehaviorTree(AIBehaviorTree);
	}

	/*if (nullptr != PlayerCody || true == LineOfSightTo(PlayerCody, PlayerCody->GetActorLocation(), false))
	{
		SetFocus(PlayerCody);
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);
	}*/

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{

	}
}

void AFlyingSaucerAIController::SetPlayerReference()
{
	// ���� ��ġ�� �޾ƿ´�. 
	PlayerCody = Cast<ACody>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

