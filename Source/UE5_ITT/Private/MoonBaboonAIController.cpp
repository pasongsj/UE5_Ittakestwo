// Fill out your copyright notice in the Description page of Project Settings.


#include "MoonBaboonAIController.h"

AMoonBaboonAIController::AMoonBaboonAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMoonBaboonAIController::BeginPlay()
{
	Super::BeginPlay();

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void AMoonBaboonAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		
	}
}
