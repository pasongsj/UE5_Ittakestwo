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
#include "ITTGameModeBase.h"
#include "Net/UnrealNetwork.h"

AFlyingSaucerAIController::AFlyingSaucerAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		CurrentBehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("CurrentBehaviorTreeCompnent"));
		bReplicates = true;
		SetReplicateMovement(true);
	}
}

void AFlyingSaucerAIController::AddPatternMatchCount()
{
	++PatternMatchCount;
	GetBlackboardComponent()->SetValueAsInt(TEXT("PatternMatchCount"), PatternMatchCount);
}

void AFlyingSaucerAIController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// �޽� ������Ʈ�� Replication�ϱ� ���� ���� �߰�
	DOREPLIFETIME(AFlyingSaucerAIController, AIBehaviorTreePhase1);
	DOREPLIFETIME(AFlyingSaucerAIController, AIBehaviorTreePhase2);
	DOREPLIFETIME(AFlyingSaucerAIController, AIBehaviorTreePhase3);
	DOREPLIFETIME(AFlyingSaucerAIController, CurrentBehaviorTree);
	DOREPLIFETIME(AFlyingSaucerAIController, CurrentBehaviorTreeComp);

	/*DOREPLIFETIME(AFlyingSaucerAIController, PrevTargetLocation);
	DOREPLIFETIME(AFlyingSaucerAIController, PrevTargetLocationBuffer);
	DOREPLIFETIME(AFlyingSaucerAIController, bPrevTargetLocationValid);
	DOREPLIFETIME(AFlyingSaucerAIController, LaserLerpRatio);
	DOREPLIFETIME(AFlyingSaucerAIController, LaserLerpRate);
	DOREPLIFETIME(AFlyingSaucerAIController, PatternMatchCount);*/
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

		// ������ �Ǿ��ٸ� �׶����� ����
		else if (true == bIsSetupPlayerRef)
		{
			// UpdateLerpRatioForLaserBeam(DeltaTime);
		}
	}
}

void AFlyingSaucerAIController::SetupPlayerRefAndBehaviorTreePhase1()
{
	// ���⼭ ���Ӹ�带 �޾ƿͼ� ī��Ʈ�� 2�� �Ǿ����� Ȯ���ϰ� 2�� �Ǿ��ٸ� bool ���� true�� ����
	AITTGameModeBase* CurrentGameMode = Cast<AITTGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	int32 LoginCount = CurrentGameMode->GetPlayerLoginCount();
	if (2 == LoginCount)
	{
		TArray<AActor*> Players = CurrentGameMode->GetLoginPlayerControllers();
		APlayerController* PlayerController0 = Cast<APlayerController>(Players[0]);
		APlayerController* PlayerController1 = Cast<APlayerController>(Players[1]);
		PlayerRef1 = PlayerController0->GetPawn();
		PlayerRef2 = PlayerController1->GetPawn();
		SetupStartBehaviorTreePhase1();
		//GetBlackboardComponent()->SetValueAsVector(TEXT("PrevTargetLocation"), PrevTargetLocation);
		bIsSetupPlayerRef = true;
	}
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

//void AFlyingSaucerAIController::SavePreviousTargetLocation()
//{
//	APawn* TargetPawn = Cast<APawn>(GetBlackboardComponent()->GetValueAsObject(TEXT("LaserBeamTarget")));
//	
//	if (nullptr != TargetPawn)
//	{
//		FVector CurrentTargetLocation = TargetPawn->GetActorLocation();
//
//		// ���� Ÿ�� ��ġ�� ��ȿ�ϴٸ� 
//		if (true == bPrevTargetLocationValid)
//		{
//			// Ÿ�� ��ġ�� ����Ǿ��ִ� ����Ÿ����ġ�� �����ϰ� false ó��
//			PrevTargetLocation = PrevTargetLocationBuffer;
//			bPrevTargetLocationValid = false;
//		}
//
//		else
//		{
//			// ��ȿ���� �ʴٸ� Ÿ�� ��ġ�� ���� ��ġ�� ����
//			PrevTargetLocation = CurrentTargetLocation;
//		}
//
//		// Ÿ����ġ�� ����
//		GetBlackboardComponent()->SetValueAsVector(TEXT("PrevTargetLocation"), PrevTargetLocation);
//		PrevTargetLocationBuffer = CurrentTargetLocation;
//		bPrevTargetLocationValid = true;
//	}
//
//}
//
//void AFlyingSaucerAIController::UpdateLerpRatioForLaserBeam(float DeltaTime)
//{
//	LaserLerpRatio += DeltaTime * LaserLerpRate;
//	if (1.0f <= LaserLerpRatio)
//	{
//		GetBlackboardComponent()->SetValueAsVector(TEXT("PrevLaserAttackLocation"), PrevTargetLocation);
//		SavePreviousTargetLocation();
//		LaserLerpRatio -= 1.0f;
//		if (0.1f <= LaserLerpRatio)
//		{
//			LaserLerpRatio = 0.0f;
//		}
//	}
//
//	GetBlackboardComponent()->SetValueAsFloat(TEXT("LaserLerpRatio"), LaserLerpRatio);
//}
//
