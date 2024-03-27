// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CodyAnimNotify_Dash.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Cody.generated.h"



UENUM(BlueprintType)
enum class Cody_State : uint8
{
	IDLE UMETA(DisPlayName = "IDLE"),
	MOVE UMETA(DisPlayName = "MOVE"),
	SIT UMETA(DisPlayName = "SIT"),
	JUMP UMETA(DisPlayName = "JUMP"),
	DASH UMETA(DisPlayName = "DASH"),
};


UCLASS()
class UE5_ITT_API ACody : public ACharacter
{
	GENERATED_BODY()

public:
	//basic
	ACody();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UInputComponent* Input;
	//friend
	friend class UCodyAnimNotify_Dash;


	//Cody�� ���� state�� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable) 
	inline Cody_State GetCodyState() const
	{
		return CodyState;
	}
	//�÷��̾��� �̵��� ���������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsMoveEnd() const
	{
		return IsMoveEnd;
	}
	//�̵��� �������� ȣ��˴ϴ�(CodyAnimNotify.cpp)
	inline void SetIsMoveEndT()
	{
		IsMoveEnd = true;
	}
	//�뽬 Ű�� ���������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetbIsDashing() const
	{
		return bIsDashing;
	}
	//�뽬���� �ִϸ��̼��� ���������� Ȯ���մϴ�
	UFUNCTION(BlueprintCallable)
	inline bool GetbIsDashingStart() const
	{
		return bIsDashingStart;
	}
	//�뽬 ���� �ִϸ��̼��� ������ �� ȣ��˴ϴ�(CodyNotify_DashStart.cpp)
	inline void SetbIsDashingStart()
	{
		bIsDashingStart = true;
	}

	///////////////////Input///////////////////

	//Ű�� ���ε� Context�Դϴ�
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* CodyMappingContext;

	//IA_Jump
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;
	//IA_Move
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;
	//IA_Look
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;
	//IA_LeftM
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LeftMAction;
	//IA_RightM
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* RightMAction;
	//IA_Sit
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* SitAction;
	//IA_Dash
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DashAction;




	///////////////////Test///////////////////
	void ChangeState(Cody_State _State);








	//////////////////////////////////////////

protected:
	
	virtual void BeginPlay() override;

	///////////////////Key Bind Function///////////////////
	void Idle(const FInputActionInstance& _Instance);
	void Move(const FInputActionInstance& _Instance);
	void Look(const FInputActionInstance& _Instance);
	void DashInput();
	void GroundDash();
	void JumpDash();
	void DashEnd();

private:
	/////////////////Controller///////////////////
	APlayerController* CodyController = nullptr;
	//////////////////////////////////////////////
	 
	///////////////////State/////////////////////
	UPROPERTY(EditAnywhere, Category = State)
	Cody_State CodyState = Cody_State::IDLE;
	//////////////////////////////////////////////
public:
	///////////////////Player/////////////////////
	UPROPERTY(EditAnywhere,BlueprintReadWrite ,Category = Player)
	int32 PlayerHP = 0;
	//////////////////////////////////////////////

	//////////////////Movement////////////////////
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDistance = 2000.0f; // �ձ����� �Ÿ�
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDuration = 0.7f; // �ձ����� ���� �ð�

	bool bIsDashing; // �ձ����� �� ���θ� ��Ÿ���� �÷���
	bool bIsDashingStart; //�ձ����� ���۴ܰ踦 ��Ÿ���� �÷���
	FTimerHandle DashTimerHandle; // �ձ����� Ÿ�̸� �ڵ�
	float DefaultGroundFriction; // �⺻ ���� ������
	float DefaultGravityScale;

private:
	//Test
	bool IsMoveEnd = true;
	UPROPERTY(EditAnywhere, Category = Player)
	float RotationInterpSpeed = 2.0f;
};
