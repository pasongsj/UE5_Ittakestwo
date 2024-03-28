// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetRandomTarget.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetRandomTarget::UBTTask_SetRandomTarget()
{
	NodeName = TEXT("BTTask_SetRandomTarget");
}

EBTNodeResult::Type UBTTask_SetRandomTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// ������ ������Ʈ�� Ű���� ����������. ����,�ڵ�,���� ������ Ÿ�� �����ϵ���
	// �ӽ÷� ������ �ڵ� ����
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (nullptr != BlackBoardComp)
	{
		int32 TargetCount = BlackBoardComp->GetValueAsInt(TEXT("Phase1TargetCount"));
		switch (TargetCount)
		{
		case 1:
		{
			UObject* PlayerRef1 = BlackBoardComp->GetValueAsObject(TEXT("PlayerRef1"));
			BlackBoardComp->SetValueAsObject(TEXT("LaserBeamTarget"), PlayerRef1);
		}
			break;
		case 2:
		{
			UObject* PlayerRef1 = BlackBoardComp->GetValueAsObject(TEXT("PlayerRef1"));
			BlackBoardComp->SetValueAsObject(TEXT("LaserBeamTarget"), PlayerRef1);
		}
			break;
		case 3:
		{
			UObject* PlayerRef1 = BlackBoardComp->GetValueAsObject(TEXT("PlayerRef1"));
			BlackBoardComp->SetValueAsObject(TEXT("LaserBeamTarget"), PlayerRef1);
		}
			break;
		default:
			return EBTNodeResult::Failed;
			break;
		}
	}

	return EBTNodeResult::Succeeded;
}
