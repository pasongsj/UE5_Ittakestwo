// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Logging/LogMacros.h"

// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	BigLength = 2000.0f;
	NormalLength = 1200.0f;
	SmallLength = 200.0f;

	// ĳ�����̵�
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//������ ��
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->TargetArmLength = NormalLength;
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false;

	//ī�޶� ����
	PlayerCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	PlayerCameraComponent->bUsePawnControlRotation = false;

	PrimaryActorTick.bStartWithTickEnabled = true;

}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	//�Է�
	CodyController = Cast<APlayerController>(Controller);
	if (CodyController != nullptr)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(CodyController->GetLocalPlayer());
		if (Subsystem != nullptr)
		{
			Subsystem->AddMappingContext(CodyMappingContext, 0);
		}
	}

	//Set
	PlayerHP = 12; //Player�⺻ Hp����
	DashDuration = 1.0f; //Dash �ð�
	DefaultGroundFriction = GetCharacterMovement()->GroundFriction; //�⺻ ���� ������
	DefaultGravityScale = GetCharacterMovement()->GravityScale; //�⺻ �߷� ������

	bIsDashing = false;
	bIsDashingStart = false;
	bCanDash = false;
	BigCanDash = true;
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//�뽬�� ���ӽð��� Tick���� ���������� Ȯ��
	if (bIsDashing && bCanDash)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime >= DashStartTime + DashDuration)
		{
			// ��� ���� �ð��� ������ ��� ����
			DashEnd();
		}
	}
}

// Called to bind functionality to input
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	Input = PlayerInputComponent;
	PlayerInputComponent->BindAction(TEXT("Player_Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Player_Sit"), EInputEvent::IE_Pressed, this, &APlayerBase::Sit);


	PlayerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput != nullptr)
	{
		PlayerInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerBase::Move);
		PlayerInput->BindAction(MoveAction, ETriggerEvent::None, this, &APlayerBase::Idle);
		PlayerInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);
		PlayerInput->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlayerBase::DashInput);
		PlayerInput->BindAction(DashAction, ETriggerEvent::None, this, &APlayerBase::DashNoneInput);
	}
}


//////////////////////////////ĳ���� �̵�����///////////////////////////
//���ο� �Է� Action
void APlayerBase::Idle(const FInputActionInstance& _Instance)
{
	IsMoveEnd = false;
	if (bCanDash == false)
		ChangeState(Cody_State::IDLE);
}

void APlayerBase::Move(const FInputActionInstance& _Instance)
{
	//UE_LOG(LogTemp, Warning, TEXT("Move function called"));
	IsMoveEnd = true;

	if (bCanDash == false)
	{
		ChangeState(Cody_State::MOVE);
		// Move�� ������ �� ī�޶��� ��ġ�� �ݿ��Ͽ� SetRotation��(�� �������� ��������)
		// 
		// ��Ʈ�ѷ��� ȸ�� ������ ������
		FRotator ControllerRotation = Controller->GetControlRotation();

		// ��Ʈ�ѷ��� ȸ�� ���⿡�� Yaw ������ ����Ͽ� ĳ���͸� ȸ����Ŵ
		FRotator TargetRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);
		SetActorRotation(TargetRotation);


		// ��Ʈ�ѷ��� ȸ�� ������ �������� ���� ���� ���͸� ���
		FVector WorldForwardVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Y);
		// ��Ʈ�ѷ��� ȸ�� ������ �������� ���� ������ ���͸� ���
		FVector WorldRightVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::X);

		// ĳ���͸� �Է� �������� �̵���Ŵ
		FVector2D MoveInput = _Instance.GetValue().Get<FVector2D>();
		if (!MoveInput.IsNearlyZero())
		{
			// �Է� ���� �븻������
			MoveInput = MoveInput.GetSafeNormal();

			// MoveDirection�������� Yaw�κи� Player Rotation�� ����
			FVector MoveDirection = WorldForwardVector * MoveInput.Y + WorldRightVector * MoveInput.X;
			FRotator CodyRotation(0.0f, MoveDirection.Rotation().Yaw, 0.0f);
			SetActorRotation(CodyRotation);


			// �Է� ������ ĳ������ ���� XY ��鿡 ���翵�Ͽ� ĳ������ �̵�����
			MoveDirection = FVector::VectorPlaneProject(MoveDirection, FVector::UpVector);
			MoveDirection.Normalize();

			// �Է� ���⿡ ���� ĳ���͸� �̵���Ŵ
			AddMovementInput(MoveDirection);
		}
	}
}


void APlayerBase::Look(const FInputActionInstance& _Instance)
{
	//UE_LOG(LogTemp, Warning, TEXT("Look function called"));
	if (Controller != nullptr)
	{
		// 1. �̽����� �ٶ󺸴� ������ �÷��̾��� ���溤�Ͱ� �Ǿ���� �椷
		// 2. Move�� �Է����϶� ������-> Move�� ���� ������ ����Ǿ����(idle)
		// 3. Move�߿��� ���ʹ� �����ǰ�, ���� ������ �޶������� �椷
		// 4. ���� ���� -> ��ġ,�� �Ѵ� ���� �椷
		// 5. ���⺤�� -> �丸 ���� �椷

		CameraLookVector = _Instance.GetValue().Get<FVector2D>();

		AddControllerYawInput(CameraLookVector.X);

		// ī�޶��� ��ġ ���� ����
		// 90�� �Ѿ�� �������� Ÿ�ٱ��̿� ������ ���ľ���.
		float CurrentPitch = GetControlRotation().Pitch;
		float NewPitch = FMath::ClampAngle(CurrentPitch + CameraLookVector.Y, -90.0f, 0.0f); // -90������ 0�� ���̷� ����
		FRotator NewRotation = FRotator(NewPitch, GetControlRotation().Yaw, GetControlRotation().Roll);
		Controller->SetControlRotation(NewRotation);
	}
}

void APlayerBase::DashInput()
{
	if (!bIsDashing && !bCanDash && BigCanDash)
	{
		ChangeState(Cody_State::DASH);
		//�뽬 ���۽ð��� üũ
		DashStartTime = GetWorld()->GetTimeSeconds();
		//���鿡 ����ִ����� üũ�Ͽ� ������ �Լ� ����
		if (!GetCharacterMovement()->IsFalling())
		{
			GroundDash();
		}
		else
		{
			JumpDash();
		}
		bIsDashing = true;
		bCanDash = true;
	}
}

void APlayerBase::DashNoneInput()
{

}

void APlayerBase::GroundDash()
{
	// ������ ����
	GetCharacterMovement()->GroundFriction = 0.0f;
	// Cody�� ���溤��
	FVector DashDirection = GetActorForwardVector();
	// ����Normalize
	DashDirection.Normalize();
	// �Ÿ� x ���� ���
	FVector DashVelocity = DashDirection * DashDistance;
	// �ð������� �ӵ�����
	GetCharacterMovement()->Velocity = DashVelocity;
}

void APlayerBase::JumpDash()
{
	// �߷� ����
	GetCharacterMovement()->GravityScale = 0.0f;
	// Cody�� ���溤��
	FVector DashDirection = GetActorForwardVector();
	// ���⺤��normalize
	DashDirection.Normalize();
	// �Ÿ� x ���� ���
	FVector DashVelocity = DashDirection * DashDistance * 0.75f;
	// �ð������� �ӵ�����
	GetCharacterMovement()->Velocity = DashVelocity;
}

void APlayerBase::DashEnd()
{
	//�뽬�� ������ �⺻���� ����� �߷�,���� ���������� �ٽ� ����
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	bIsDashing = false;
	bIsDashingStart = false;
	bCanDash = false;
}

void APlayerBase::Sit()
{
	ChangeState(Cody_State::SIT);
	if (GetCharacterMovement()->IsFalling())
	{
		//GetCharacterMovement()->GravityScale = 10.0f;
	}
	else
	{
		//GetCharacterMovement()->GravityScale = DefaultGravityScale;
	}
	//�ۼ���
}

//////////////FSM//////////
void APlayerBase::ChangeState(Cody_State _State)
{
	ITTPlayerState = _State;
}
