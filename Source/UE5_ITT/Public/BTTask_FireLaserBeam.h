// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FireLaserBeam.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API UBTTask_FireLaserBeam : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_FireLaserBeam();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
