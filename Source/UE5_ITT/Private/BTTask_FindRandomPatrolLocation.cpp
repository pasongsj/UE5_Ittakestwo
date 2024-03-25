// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindRandomPatrolLocation.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindRandomPatrolLocation::UBTTask_FindRandomPatrolLocation()
{
	NodeName = TEXT("Find Random Patrol Location");
}

EBTNodeResult::Type UBTTask_FindRandomPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// ������ ��ġ�� �����ϴ� ���� �ۼ� �� ������ Ű�� ����

	OwnerComp.GetBlackboardComponent()->SetValueAsVector("PatrolLocation", FVector::ZeroVector);

	return EBTNodeResult::Type();
}
