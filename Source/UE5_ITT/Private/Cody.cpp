// Fill out your copyright notice in the Description page of Project Settings.


#include "Cody.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Logging/LogMacros.h"

// Sets default values
ACody::ACody()
{
	PrimaryActorTick.bCanEverTick = true;

	// ĳ�����̵�
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//������ ��
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetUsingAbsoluteRotation(true); 
	SpringArm->TargetArmLength = 1000.f;
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false; 

	//ī�޶� ����
	CodyCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CodyCamera"));
	CodyCameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	CodyCameraComponent->bUsePawnControlRotation = false;

	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void ACody::BeginPlay()
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
	PressDashKey = false;
}

// Called every frame
void ACody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsDashing && bCanDash)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime >= DashStartTime + DashDuration)
		{
			DashEnd(); // ��� ���� �ð��� ������ ��� ����
		}
	}
}

// Called to bind functionality to input
void ACody::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	Input = PlayerInputComponent;
	PlayerInputComponent->BindAction(TEXT("Player_Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Player_Sit"), EInputEvent::IE_Pressed, this, &ACody::Sit);


	UEnhancedInputComponent* CodyInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (CodyInput != nullptr)
	{
		CodyInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACody::Move);
		CodyInput->BindAction(MoveAction, ETriggerEvent::None, this, &ACody::Idle);
		CodyInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACody::Look);
		CodyInput->BindAction(DashAction, ETriggerEvent::Triggered, this, &ACody::DashInput);
		CodyInput->BindAction(DashAction, ETriggerEvent::None, this, &ACody::DashNoneInput);
	}
}


//////////////////////////////ĳ���� �̵�����///////////////////////////
//���ο� �Է� Action
void ACody::Idle(const FInputActionInstance& _Instance)
{
	IsMoveEnd = false;
	if(bCanDash==false)
		ChangeState(Cody_State::IDLE);
}
void ACody::Move(const FInputActionInstance& _Instance)
{
	UE_LOG(LogTemp, Warning, TEXT("Move function called"));
	IsMoveEnd = true;
	if (bCanDash == false)
	{
		ChangeState(Cody_State::MOVE);

		// ĳ������ �̵� �Է� ���� ������
		FVector2D MoveInput = _Instance.GetValue().Get<FVector2D>();
		if (!MoveInput.IsNearlyZero())
		{
			// �Է� ���� �븻������
			MoveInput = MoveInput.GetSafeNormal();
			// �Է� ���� ����
			FVector ForwardVector = FVector(MoveInput.X, MoveInput.Y, 0.0f);

			// �Է� �������� ĳ���͸� �̵���Ŵ
			AddMovementInput(ForwardVector);

			// �Է� �������� ĳ���͸� ȸ����Ŵ
			if (!ForwardVector.IsNearlyZero())
			{
				// �Է� ���� ���͸� ����Ͽ� ĳ���͸� ȸ����Ŵ
				FRotator TargetRotation = ForwardVector.Rotation();
				SetActorRotation(TargetRotation);
			}
		}
	}
}


void ACody::Look(const FInputActionInstance& _Instance)
{
	UE_LOG(LogTemp, Warning, TEXT("Look function called"));
	if (Controller != nullptr)
	{
		FVector2D LookVector = _Instance.GetValue().Get<FVector2D>();
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void ACody::DashInput()
{
	if (!bIsDashing && !bCanDash)
	{
		ChangeState(Cody_State::DASH);
		DashStartTime = GetWorld()->GetTimeSeconds();
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

void ACody::DashNoneInput()
{
	
}

void ACody::GroundDash()
{
	GetCharacterMovement()->GroundFriction = 0.0f; //������ ����

	FVector DashDirection = GetActorForwardVector(); // ���溤��
	DashDirection.Normalize(); // ���⺤��normalize

	FVector DashVelocity = DashDirection * DashDistance; //�Ÿ�x����
	GetCharacterMovement()->Velocity = DashVelocity; // �ð������� �ӵ�����
}

void ACody::JumpDash()
{
	GetCharacterMovement()->GravityScale = 0.0f; //�߷� ����

	FVector DashDirection = GetActorForwardVector(); // ���溤��
	DashDirection.Normalize(); // ���⺤��normalize

	FVector DashVelocity = DashDirection * DashDistance * 0.75f; //�Ÿ�x����
	GetCharacterMovement()->Velocity = DashVelocity; // �ð������� �ӵ�����
}

void ACody::DashEnd()
{
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	bIsDashing = false;
	bIsDashingStart = false;
	bCanDash = false;
}

void ACody::Sit()
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
void ACody::ChangeState(Cody_State _State)
{
	CodyState = _State;
}
