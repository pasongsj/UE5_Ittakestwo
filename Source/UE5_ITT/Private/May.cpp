// Fill out your copyright notice in the Description page of Project Settings.


#include "May.h"


// Sets default values
AMay::AMay()
{
	Tags.Add(FName("May"));
}


// Called when the game starts or when spawned
void AMay::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bOnGravityPath == true)
	{
		//GetMesh()->SetWorldRotation(GravityRotator);
		SetActorRotation(GravityRotator);
	}
}

void AMay::DashEnd()
{
	//�뽬�� ������ �⺻���� ����� �߷�,���� ���������� �ٽ� ����
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	//���� ��������� �뽬 ����� ����
	if (!GetCharacterMovement()->IsFalling())
	{
		bIsDashing = false;
		bIsDashingStart = false;
		bCanDash = false;
		DashDuration = 0.7f;
	}
}
