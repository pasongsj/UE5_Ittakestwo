// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
//#include "Interfaces/OnlineSessionInterface.h"


#include "PlayerBase.generated.h"

UENUM(BlueprintType)
enum class CodySize : uint8
{
	NONE UMETA(DisPlayName = "NONE"),
	BIG UMETA(DisPlayName = "BIG"),
	NORMAL UMETA(DisPlayName = "NORMAL"),
	SMALL UMETA(DisPlayName = "SMALL"),
};
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
	HOLDENEMY UMETA(DisPlayName = "HoldEnemy"),
	FLYING UMETA(DisPlayName = "FLYING"),
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
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* TestAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* FlyMoveAction;


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
	UFUNCTION(BlueprintCallable)
	inline int32 GetPlayerFullHP() const
	{
		return FullHP;
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
	//�÷��̾ �׾��ִ� ���·� ����ϴ�.
	UFUNCTION(BlueprintCallable)
	inline void SetPlayerDeath()
	{
		IsPlayerDeath = true;
	}
	//Sit���� �������� �ð��� �Ǿ������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetCurrentSitTime() const
	{
		return CurrentSitTime >= SitStartTime + SitDuration - 0.15f;
	}
	//ĳ���Ͱ� ���鿡 �ִ����� ��ȯ�մϴ�
	UFUNCTION(BlueprintCallable)
	inline bool GetIsCharacterGround() const
	{
		return !GetMovementComponent()->IsFalling();
	}
	//Idle�� ���ư� �� �ִ� �������� ��ȯ�մϴ�(Sit)
	UFUNCTION(BlueprintCallable)
	inline void SetChangeIdle()
	{
		ChangeIdle = true;
	}
	UFUNCTION(BlueprintCallable)
	inline bool GetChangeIdle() const
	{
		return ChangeIdle;
	}
	//���� ĳ���Ͱ� ���° ������ �ϰ��ִ����� ��ȯ�մϴ�
	UFUNCTION(BlueprintCallable)
	inline int32 GetCharacterJumpCount() const
	{
		return CharacterJumpCount;
	}
	//�ɱ�����ϴ¸���� �������� ���θ� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsSit() const
	{
		return IsSit;
	}
	//Cody�� ū ������������ ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsBig() const
	{
		return IsBig;
	}
	//Player�� Fly ���������� ��ȯ�մϴ�
	UFUNCTION(BlueprintCallable)
	inline bool GetIsFly() const
	{
		return IsFly;
	}


	//////////////////////////////////////////
	void ChangeState(Cody_State _State);
	//���콺 ���ư��� ���ǵ�
	float RotationInterpSpeed = 2.0f;
	//��ȣ�ۿ�
	UPROPERTY(Replicated)
	bool IsInteract = false;

	//////////////////////////////////////////




	///////////////////Key Bind Function///////////////////
	void Idle(const FInputActionInstance& _Instance);
	
	UFUNCTION(Client, Reliable)
	void CustomClientIdle();
	UFUNCTION(Server, Reliable, WithValidation)
	void CustomServerIdle();

	void CustomMove(const FInputActionInstance& _Instance);

	void CustomFlyMove(const FInputActionInstance& _Instance);

	UFUNCTION(Client,Reliable)
	void ChangeClientDir(const FInputActionInstance& _Instance,FRotator _Rotator);
	UFUNCTION(Server,Reliable, WithValidation)
	void ChangeServerDir(const FInputActionInstance& _Instance, FRotator _Rotator);

	

	void Look(const FInputActionInstance& _Instance);
	void DashInput();
	UFUNCTION(Client, Reliable)
	void CustomClientDash();
	UFUNCTION(Server, Reliable, WithValidation)
	void CustomServerDash();
	void GroundDash();
	virtual void DashEnd() {};
	void JumpDash();
	void DashNoneInput();
	UFUNCTION(Server, Reliable)
	void InteractInput();
	UFUNCTION(Server, Reliable)
	void InteractNoneInput();
	virtual void SprintInput() {};
	virtual void SprintNoneInput() {};
	void Sit();
	void SitEnd();

	void TestFunction();

	///////////////////////////////////////////////////////


	
	/////////////////Controller///////////////////
	APlayerController* CodyController = nullptr;
	//////////////////////////////////////////////

	///////////////////State/////////////////////
	UPROPERTY(Replicated)
	Cody_State ITTPlayerState;
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
	UPROPERTY(Replicated)
	float DashDistance = 2500.0f; // �ձ����� �Ÿ�
	bool IsDGravity;
	float PlayerDefaultSpeed; //�÷��̾��� �⺻ �ӵ�(cody���� Normal)
	UPROPERTY(Replicated=OnRep_IsMoveEnd)
	bool IsMoveEnd; //�������� ��������
	UFUNCTION()
	void OnRep_IsMoveEnd();
	UPROPERTY(Replicated)
	float DashDuration; // �ձ����� ���� �ð�
	UPROPERTY(Replicated)
	float DashStartTime;//�ձ����� ���۽ð� üũ
	UPROPERTY(Replicated)
	bool bIsDashing; // �ձ����� �� ���θ� ��Ÿ���� �÷���
	UPROPERTY(Replicated)
	bool bIsDashingStart; //�ձ����� ���۴ܰ踦 ��Ÿ���� �÷���
	UPROPERTY(Replicated)
	bool bCanDash; //�뽬�� ������ ��������
	FTimerHandle DashTimerHandle; // �ձ����� Ÿ�̸� �ڵ�
	float DefaultGroundFriction; // �⺻ ���� ������
	UPROPERTY(Replicated)
	float DefaultGravityScale; //�⺻ �߷�
	bool BigCanDash; //Ŀ�����¿��� �뽬�� �������� ����
	UPROPERTY(Replicated)
	float DashCurrentTime;
	bool IsSprint; //�޸��� ����

	float SitStartTime;
	float SitDuration;
	UPROPERTY(Replicated)
	bool CanSit;
	UPROPERTY(Replicated)
	bool IsSit;
	float CurrentSitTime;
	UPROPERTY(Replicated)
	bool ChangeIdle;
	UPROPERTY(Replicated)
	int32 CharacterJumpCount;
	UPROPERTY(Replicated)
	FVector MoveDirection;
	UPROPERTY(Replicated)
	FVector2D MoveInput;
	UPROPERTY(Replicated)
	FRotator ControllerRotation;
	UPROPERTY(Replicated)
	FRotator CustomTargetRotation;
	UPROPERTY(Replicated)
	FVector WorldForwardVector;
	UPROPERTY(Replicated)
	FVector WorldRightVector;
	UPROPERTY(Replicated)
	FVector DashDirection;
	UPROPERTY(Replicated)
	FVector DashVelocity;

	UPROPERTY(Replicated)
	int32 CustomPlayerJumpCount;
	
	//////////////////////////////////////////////

	///////////////////Animation//////////////////
	UPROPERTY(Replicated)
	bool CurrentAnimationEnd; 
	//////////////////////////////////////////////

	//void GetOnlineSubsystem();
	//IOnlineSessionPtr OnlineSeesioninterface;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;



	CodySize CurCodySize = CodySize::NONE;
	CodySize NextCodySize = CodySize::NONE;

	//CodySize
	UPROPERTY(Replicated)
	bool IsBig;

	//Fly
	bool IsFly = false;
	FVector CurrentDirection;
	float FlyingSpeed;







	UPROPERTY(Replicated)
	FRotator TestRotator;

	UPROPERTY(EditAnywhere, Category = "UI")
	class UPlayerMarkerUI* MarkerUIWidget;
};
