// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "PlayerBase.generated.h"


UENUM(BlueprintType)
enum class Cody_State : uint8
{
	IDLE UMETA(DisPlayName = "IDLE"),
	MOVE UMETA(DisPlayName = "MOVE"),
	SIT UMETA(DisPlayName = "SIT"),
	JUMP UMETA(DisPlayName = "JUMP"),
	DASH UMETA(DisPlayName = "DASH"),
	DASHEND UMETA(DisPlayName = "DASHEND"),
	PlayerDeath UMETA(DisPlayName = "PlayerDeath"),
};

UCLASS()
class UE5_ITT_API APlayerBase : public ACharacter
{
	GENERATED_BODY()
	UInputComponent* Input;
public:
	UEnhancedInputComponent* PlayerInput;
	// Sets default values for this character's properties
	APlayerBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InteractAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* SprintAction;


	//Cody�� ���� state�� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline Cody_State GetITTPlayerState() const
	{
		return ITTPlayerState;
	}
	//ĳ���� �������Ͽ� ���յ� ī�޶� ������Ʈ�� ��ȯ�մϴ�.
	FORCEINLINE class UCameraComponent* GetCodyCameraComponent() const
	{
		return PlayerCameraComponent;
	}
	//ĳ������ ���������� ��ȯ�մϴ�.
	FORCEINLINE class USpringArmComponent* GetSpringArm() const
	{
		return SpringArm;
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
	//�뽬�� ���ӽð��� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline float GetDashDuration()
	{
		return DashDuration;
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
	// �뽬�� �ٽ� �Է°����� ���������� Ȯ���մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetbCanDash() const
	{
		return bCanDash;
	}
	//Player�� HP�� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline int32 GetPlayerHP() const
	{
		return PlayerHP;
	}
	//ī�޶��� Vector2D�� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline FVector2D GetCameraVector() const
	{
		return CameraLookVector;
	}
	//ĳ���Ϳ� ����Ǿ��ִ� �⺻ ���� �������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline float GetPlayerGroundFriction() const
	{
		return DefaultGroundFriction;
	}
	//ĳ���Ϳ� ����Ǿ��ִ� �⺻ �߷� �������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline float GetPlayerDefaultGravityScale() const
	{
		return DefaultGravityScale;
	}
	//ĳ���Ϳ� ���� ����Ǿ��ִ� �߷� �������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline float GetPlayerGravityScale() const
	{
		return GetCharacterMovement()->GravityScale;
	}
	//���� �߷��� ĳ���� Scale�� ������� ���� ����ǰ��ִ����� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline float GetIsDGravityScale() const
	{
		return IsDGravity;
	}
	//�ִϸ��̼��� ������ �� ȣ��˴ϴ�
	UFUNCTION(BlueprintCallable)
	inline void SetCurrentAnimationEndT()
	{
		CurrentAnimationEnd = true;
	}
	UFUNCTION(BlueprintCallable)
	inline void SetCurrentAnimationEndF()
	{
		CurrentAnimationEnd = false;
	}
	//���� �ִϸ��̼��� ���������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetCurrentAnimationEnd() const
	{
		return CurrentAnimationEnd;
	}
	//��ȣ�ۿ� Ű�� �������� true�� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsInteract() const
	{
		return IsInteract;
	}
	// Player�� �޸����ִ� ���������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsSprint() const
	{
		return IsSprint;
	}
	//Player�� �������������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsPlayerDeath() const
	{
		return IsPlayerDeath;
	}
	inline void SetPlayerDeath()
	{
		IsPlayerDeath = true;
	}




	//////////////////////////////////////////
	void ChangeState(Cody_State _State);
	//���콺 ���ư��� ���ǵ�
	float RotationInterpSpeed = 2.0f;
	//��ȣ�ۿ�
	bool IsInteract = false;

	//////////////////////////////////////////




	///////////////////Key Bind Function///////////////////
	void Idle(const FInputActionInstance& _Instance);
	void Move(const FInputActionInstance& _Instance);
	void Look(const FInputActionInstance& _Instance);
	void DashInput();
	void GroundDash();
	virtual void DashEnd() {};
	void JumpDash();
	void DashNoneInput();
	void InteractInput();
	void InteractNoneInput();
	virtual void SprintInput() {};
	virtual void SprintNoneInput() {};
	void Sit();
	void SitEnd();
	///////////////////////////////////////////////////////



	/////////////////Controller///////////////////
	APlayerController* CodyController = nullptr;
	//////////////////////////////////////////////

	///////////////////State/////////////////////
	UPROPERTY(EditAnywhere, Category = State)
	Cody_State ITTPlayerState = Cody_State::IDLE;
	//////////////////////////////////////////////

	///////////////////Player/////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player)
	int32 PlayerHP = 0;
	int32 FullHP = 12;
	bool IsPlayerDeath = false;
	void PlayerDeathCheck();
	//////////////////////////////////////////////

	//////////////////Camera////////////////////
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PlayerCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	float BigLength;
	float NormalLength;
	float SmallLength;

	FVector2D CameraLookVector = FVector2D::ZeroVector;
	//////////////////////////////////////////////

	//////////////////Movement////////////////////
	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashDistance = 2500.0f; // �ձ����� �Ÿ�
	
	bool IsDGravity;

	float PlayerDefaultSpeed; //�÷��̾��� �⺻ �ӵ�(cody���� Normal)
	bool IsMoveEnd = true; //�������� ��������
	float DashDuration; // �ձ����� ���� �ð�
	float DashStartTime;//�ձ����� ���۽ð� üũ
	bool bIsDashing; // �ձ����� �� ���θ� ��Ÿ���� �÷���
	bool bIsDashingStart; //�ձ����� ���۴ܰ踦 ��Ÿ���� �÷���
	bool bCanDash; //�뽬�� ������ ��������
	FTimerHandle DashTimerHandle; // �ձ����� Ÿ�̸� �ڵ�
	float DefaultGroundFriction; // �⺻ ���� ������
	float DefaultGravityScale; //�⺻ �߷�
	bool BigCanDash; //Ŀ�����¿��� �뽬�� �������� ����

	bool IsSprint; //�޸��� ����

	float SitStartTime;
	float SitDuration;
	bool CanSit;
	
	//////////////////////////////////////////////

	///////////////////Animation//////////////////
	bool CurrentAnimationEnd; 
	//////////////////////////////////////////////
};
