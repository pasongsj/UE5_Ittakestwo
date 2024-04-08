// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "Cody.generated.h"





UENUM(BlueprintType)
enum class CodySize
{
	NONE UMETA(DisPlayName = "NONE"),
	BIG UMETA(DisPlayName = "BIG"),
	NORMAL UMETA(DisPlayName = "NORMAL"),
	SMALL UMETA(DisPlayName = "SMALL"),
};

UCLASS()
class UE5_ITT_API ACody : public APlayerBase
{
public:
	GENERATED_BODY()
	ACody();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	//CodySize Enum�� �����մϴ�
	void ChangeCodySizeEnum(CodySize _Enum)
	{
		CodySizes = _Enum;
	}
	//Cody�� Size�� Enum���� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline CodySize GetCodySize() const
	{
		return CodySizes;
	}
	//��ȯ�ϰ����ϴ� Size�� TargetScale�� ��ȯ�մϴ�
	UFUNCTION(BlueprintCallable)
	inline FVector GetTargetScale() const
	{
		return TargetScale;
	}

	void ChangeBigSize();
	void ChangeSmallSize();


	CodySize CodySizes = CodySize::NONE;
	//Set Speed
	float ScaleSpeed;
	float CameraSpeed;
	FVector TargetScale;
	//Size
	FVector BigSize;
	FVector NormalSize;
	FVector SmallSize;
	FVector BigSizeCapsule;
	FVector NormalSizeCapsule;
	FVector SmallSizeCapsule;
	//Transform
	FTransform CodyCapsuleComponent;
	FTransform CodyTransform;
	FTransform CodyLocalTransform;
};
