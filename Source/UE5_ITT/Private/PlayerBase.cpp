// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Logging/LogMacros.h"
//#include "OnlineSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "PlayerMarkerUI.h"

// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName("Player"));

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

	MarkerUIWidget = CreateDefaultSubobject<UPlayerMarkerUI>(TEXT("WidgetComponent"));
	MarkerUIWidget->SetupAttachment(RootComponent);
}

//void APlayerBase::GetOnlineSubsystem()
//{
//
//	// OnlineSubsystem�� Access
//	IOnlineSubsystem* CurOnlineSubsystem = IOnlineSubsystem::Get();
//	if (CurOnlineSubsystem)
//	{
//		// �¶��� ���� �޾ƿ���
//		OnlineSeesioninterface = CurOnlineSubsystem->GetSessionInterface();
//
//		if (GEngine)
//		{
//			// OnlineSubsystem �̸� ����ϱ�
//			//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Found subsystem %s"), *CurOnlineSubsystem->GetSubsystemName().ToString()));
//		}
//	}
//}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	//GetOnlineSubsystem();
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
	ITTPlayerState = Cody_State::IDLE;
	//Set
	PlayerHP = FullHP; //Player�⺻ Hp����
	DashDuration = 0.7f; //Dash �ð�
	DefaultGroundFriction = GetCharacterMovement()->GroundFriction; //�⺻ ���� ������
	DefaultGravityScale = GetCharacterMovement()->GravityScale; //�⺻ �߷� ������
	PlayerDefaultSpeed = GetCharacterMovement()->MaxWalkSpeed; //�⺻ �̵��ӵ�


	IsMoveEnd = false;

	bIsDashing = false;
	bIsDashingStart = false;
	bCanDash = false;
	BigCanDash = true;

	CurrentAnimationEnd = false;
	IsSprint = false;

	CanSit = true;
	SitDuration = 0.5f;
	ChangeIdle = true;
	TestRotator = FRotator::ZeroRotator;


	CustomPlayerJumpCount = ACharacter::JumpMaxCount;


	FlyingSpeed = 600.0f;
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bOnGravityPath==true)
	{
		TestRotator = GravityRotator;
	}
	/*SetActorRotation(TestRotator);*/
	//���� Ƚ�� Ȯ��
	CharacterJumpCount = JumpCurrentCount;
	//�߷»���Ȯ��(Sit)
	if (GetCharacterMovement()->GravityScale <=5.5f)
	{
		IsDGravity = true;
	}
	else
	{
		IsDGravity = false;
	}
	//�÷��̾� �������� Ȯ��
	PlayerDeathCheck();
	//�뽬�� ���ӽð��� Tick���� ���������� Ȯ��
	if (bIsDashing && bCanDash)
	{
		DashCurrentTime = GetWorld()->GetTimeSeconds();
		if (DashCurrentTime >= DashStartTime + DashDuration)
		{
			// ��� ���� �ð��� ������ ��� ����
			DashEnd();
		}
	}
	if (!CanSit)
	{
		CurrentSitTime = GetWorld()->GetTimeSeconds();
		if (CurrentSitTime >= SitStartTime + SitDuration)
		{
			GetCharacterMovement()->GravityScale = 10.0f;
			if (!GetCharacterMovement()->IsFalling())
			{
				SitEnd();
			}
		}
	}


	//FlyTest
	if (IsFly)
	{
		FVector ForwardVector = GetActorForwardVector();
		AddActorWorldOffset(ForwardVector * FlyingSpeed * DeltaTime);
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
		PlayerInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerBase::CustomMove);
		PlayerInput->BindAction(MoveAction, ETriggerEvent::None, this, &APlayerBase::Idle);

		PlayerInput->BindAction(FlyMoveAction, ETriggerEvent::Triggered, this, &APlayerBase::CustomFlyMove);

		PlayerInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);
		PlayerInput->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlayerBase::DashInput);
		PlayerInput->BindAction(DashAction, ETriggerEvent::None, this, &APlayerBase::DashNoneInput);
		PlayerInput->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerBase::InteractInput);
		PlayerInput->BindAction(InteractAction, ETriggerEvent::None, this, &APlayerBase::InteractNoneInput);
		PlayerInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerBase::SprintInput);
		PlayerInput->BindAction(SprintAction, ETriggerEvent::None, this, &APlayerBase::SprintNoneInput);

		PlayerInput->BindAction(TestAction, ETriggerEvent::Triggered, this, &APlayerBase::TestFunction);
	}
}


//////////////////////////////ĳ���� �̵�����///////////////////////////
//���ο� �Է� Action
void APlayerBase::Idle(const FInputActionInstance& _Instance)
{
	if (HasAuthority() == true)
	{
		CustomClientIdle();
	}
	else
	{
		CustomServerIdle();
	}
}

void APlayerBase::CustomClientIdle_Implementation()
{
	IsSprint = false;
	IsMoveEnd = false;
	if (bCanDash == false)
		ChangeState(Cody_State::IDLE);
}

bool APlayerBase::CustomServerIdle_Validate()
{
	return true;
}
void APlayerBase::CustomServerIdle_Implementation()
{
	OnRep_IsMoveEnd();
}

void APlayerBase::OnRep_IsMoveEnd()
{
	IsSprint = false;
	IsMoveEnd = false;
	if (bCanDash == false)
		ChangeState(Cody_State::IDLE);
}
void APlayerBase::CustomMove(const FInputActionInstance& _Instance)
{
	if (IsFly == false)
	{
		if (bCanDash == false && ChangeIdle)
		{
			ChangeState(Cody_State::MOVE);
			// ��Ʈ�ѷ��� ȸ�� ������ ������
			ControllerRotation = Controller->GetControlRotation();
			CustomTargetRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);
			// Move�� ������ �� ī�޶��� ��ġ�� �ݿ��Ͽ� SetRotation��(�� �������� ��������)
			// ��Ʈ�ѷ��� ȸ�� ���⿡�� Yaw ������ ����Ͽ� ĳ���͸� ȸ����Ŵ

			// ��Ʈ�ѷ��� ȸ�� ������ �������� ���� ���� ���͸� ���
			WorldForwardVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Y);
			// ��Ʈ�ѷ��� ȸ�� ������ �������� ���� ������ ���͸� ���
			WorldRightVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::X);

			// ĳ���͸� �Է� �������� �̵���Ŵ
			MoveInput = _Instance.GetValue().Get<FVector2D>();
			if (!MoveInput.IsNearlyZero())
			{
				// �Է� ���� �븻������
				MoveInput = MoveInput.GetSafeNormal();

				// MoveDirection�������� Yaw�κи� Player Rotation�� ����
				MoveDirection = WorldForwardVector * MoveInput.Y + WorldRightVector * MoveInput.X;
				FRotator CodyRotation(0.0f, MoveDirection.Rotation().Yaw, 0.0f);
				CustomTargetRotation = CodyRotation;

				// �Է� ���⿡ ���� ĳ���͸� �̵���Ŵ
				AddMovementInput(MoveDirection);
			}
		}
		if (HasAuthority() == true)
		{
			ChangeClientDir(_Instance, CustomTargetRotation);
		}
		else
		{
			ChangeServerDir(_Instance, CustomTargetRotation);
		}
	}
}

void APlayerBase::CustomFlyMove(const FInputActionInstance& _Instance)
{
	if (HasAuthority() == true)
	{
		CustomClientFly(_Instance);
	}
	else
	{
		CustomServerFly(_Instance);
	}
}

void APlayerBase::CustomClientFly_Implementation(const FInputActionInstance& _Instance)
{
	if (IsFly)
	{
		ChangeState(Cody_State::FLYING);
		GetCharacterMovement()->MaxFlySpeed = 1500.0f;
		if (bCanDash == false && ChangeIdle)
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			// ��Ʈ�ѷ��� ȸ�� ������ ������
			ControllerRotation = Controller->GetControlRotation();
			CustomTargetRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);
			// Move�� ������ �� ī�޶��� ��ġ�� �ݿ��Ͽ� SetRotation��(�� �������� ��������)
			// ��Ʈ�ѷ��� ȸ�� ���⿡�� Yaw ������ ����Ͽ� ĳ���͸� ȸ����Ŵ

			// ��Ʈ�ѷ��� ȸ�� ������ �������� ���� ���� ���͸� ���
			WorldForwardVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Y);
			WorldRightVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Z);

			// ĳ���͸� �Է� �������� �̵���Ŵ
			MoveInput = _Instance.GetValue().Get<FVector2D>();
			if (!MoveInput.IsNearlyZero())
			{
				// �Է� ���� �븻������
				MoveInput = MoveInput.GetSafeNormal();

				// MoveDirection�������� Yaw�κи� Player Rotation�� ����
				MoveDirection = WorldForwardVector * MoveInput.Y + WorldRightVector * MoveInput.X;
				// ��Ʈ�ѷ��� ȸ�� ������ �������� ���� ������ ���͸� ���.X;

					// �Է� ���⿡ ���� ĳ���͸� �̵���Ŵ
				if (MoveInput.X >= 0)
					AddMovementInput(FVector(MoveDirection.X, MoveDirection.Y, MoveDirection.Z * 0.5f));
				else
					AddMovementInput(FVector(-MoveDirection.X, -MoveDirection.Y, MoveDirection.Z * 0.5f));
				///////////////////////////////////////////////////////////////////////////////////////////
			}
			if (HasAuthority() == true)
			{
				ChangeClientDir(_Instance, CustomTargetRotation);
			}
			else
			{
				ChangeServerDir(_Instance, CustomTargetRotation);
			}
		}
	}
}

bool APlayerBase::CustomServerFly_Validate(const FInputActionInstance& _Instance)
{
	return true;
}

void APlayerBase::CustomServerFly_Implementation(const FInputActionInstance& _Instance)
{
	if (IsFly)
	{
		ChangeState(Cody_State::FLYING);
		GetCharacterMovement()->MaxFlySpeed = 1500.0f;
		if (bCanDash == false && ChangeIdle)
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			// ��Ʈ�ѷ��� ȸ�� ������ ������
			ControllerRotation = Controller->GetControlRotation();
			CustomTargetRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);
			// Move�� ������ �� ī�޶��� ��ġ�� �ݿ��Ͽ� SetRotation��(�� �������� ��������)
			// ��Ʈ�ѷ��� ȸ�� ���⿡�� Yaw ������ ����Ͽ� ĳ���͸� ȸ����Ŵ

			// ��Ʈ�ѷ��� ȸ�� ������ �������� ���� ���� ���͸� ���
			WorldForwardVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Y);
			WorldRightVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Z);

			// ĳ���͸� �Է� �������� �̵���Ŵ
			MoveInput = _Instance.GetValue().Get<FVector2D>();
			if (!MoveInput.IsNearlyZero())
			{
				// �Է� ���� �븻������
				MoveInput = MoveInput.GetSafeNormal();

				// MoveDirection�������� Yaw�κи� Player Rotation�� ����
				MoveDirection = WorldForwardVector * MoveInput.Y + WorldRightVector * MoveInput.X;
				// ��Ʈ�ѷ��� ȸ�� ������ �������� ���� ������ ���͸� ���.X;

					// �Է� ���⿡ ���� ĳ���͸� �̵���Ŵ
				if (MoveInput.X >= 0)
					AddMovementInput(FVector(MoveDirection.X, MoveDirection.Y, MoveDirection.Z * 0.5f));
				else
					AddMovementInput(FVector(-MoveDirection.X, -MoveDirection.Y, MoveDirection.Z * 0.5f));
				///////////////////////////////////////////////////////////////////////////////////////////
			}
			if (HasAuthority() == true)
			{
				ChangeClientDir(_Instance, CustomTargetRotation);
			}
			else
			{
				ChangeServerDir(_Instance, CustomTargetRotation);
			}
		}
	}
}

void APlayerBase::ChangeClientDir_Implementation(const FInputActionInstance& _Instance, FRotator _Rotator)
{
	IsMoveEnd = true;
	SetActorRotation(_Rotator);
}

bool APlayerBase::ChangeServerDir_Validate(const FInputActionInstance& _Instance, FRotator _Rotator)
{
	return true;
}

void APlayerBase::ChangeServerDir_Implementation(const FInputActionInstance& _Instance, FRotator _Rotator)
{
	IsMoveEnd = true;
	SetActorRotation(_Rotator);
}




void APlayerBase::Look(const FInputActionInstance& _Instance)
{
	//UE_LOG(LogTemp, Warning, TEXT("Look function called"));
	if (Controller != nullptr)
	{
		const TArray<FName>& CheckTag = Tags;
		for (const FName& V : CheckTag)
		{
			if (V == FName("Cody") || V == FName("May"))
			{
				// 1. �̽����� �ٶ󺸴� ������ �÷��̾��� ���溤�Ͱ� �Ǿ���� �椷
				// 2. Move�� �Է����϶� ������-> Move�� ���� ������ ����Ǿ����(idle)
				// 3. Move�߿��� ���ʹ� �����ǰ�, ���� ������ �޶������� �椷
				// 4. ���� ���� -> ��ġ,�� �Ѵ� ���� �椷
				// 5. ���⺤�� -> �丸 ���� �椷

				CameraLookVector = _Instance.GetValue().Get<FVector2D>();

				if (IsFly)
				{
					AddControllerYawInput(CameraLookVector.X * 0.2f);
				}
				else
				{
					AddControllerYawInput(CameraLookVector.X);
					AddControllerPitchInput(-CameraLookVector.Y);
				}
				

				//// ī�޶��� ��ġ ���� ����
				//// 90�� �Ѿ�� �������� Ÿ�ٱ��̿� ������ ���ľ���.
				//float CurrentPitch = GetControlRotation().Pitch;
				//float NewPitch = FMath::ClampAngle(CurrentPitch + CameraLookVector.Y, 360.0f, 0.0f); // -90������ 0�� ���̷� ����
				//FRotator NewRotation = FRotator(NewPitch, GetControlRotation().Yaw, GetControlRotation().Roll);
				//Controller->SetControlRotation(NewRotation);
			}
		}
	}
}

void APlayerBase::DashInput()
{
	IsSprint = false;
	if (HasAuthority() == true)
	{
		CustomClientDash();
	}
	else
	{
		CustomServerDash();
	}
}


void APlayerBase::CustomClientDash_Implementation()
{
	if (!bIsDashing && !bCanDash && BigCanDash && ChangeIdle)
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
			DashDuration = 0.2f;
			JumpDash();
		}
		bIsDashing = true;
		bCanDash = true;
	}
}
bool APlayerBase::CustomServerDash_Validate()
{
	return true;
}
void APlayerBase::CustomServerDash_Implementation()
{
	if (!bIsDashing && !bCanDash && BigCanDash && ChangeIdle)
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
			DashDuration = 0.2f;
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
	DashDirection = GetActorForwardVector();
	// ����Normalize
	DashDirection.Normalize();
	// �Ÿ� x ���� ���
	DashVelocity = DashDirection * DashDistance;
	// �ð������� �ӵ�����
	GetCharacterMovement()->Velocity = DashVelocity;
}

void APlayerBase::JumpDash()
{
	// �߷� ����
	GetCharacterMovement()->GravityScale = 0.0f;
	// Cody�� ���溤��
	DashDirection = GetActorForwardVector();
	// ���⺤��normalize
	DashDirection.Normalize();
	// �Ÿ� x ���� ���
	DashVelocity = DashDirection * DashDistance * 0.7f;
	// �ð������� �ӵ�����
	GetCharacterMovement()->Velocity = DashVelocity;
}



void APlayerBase::Sit()
{
	SitStartTime = GetWorld()->GetTimeSeconds();
	if (GetCharacterMovement()->IsFalling() && CanSit && !bIsDashing)
	{
		ChangeState(Cody_State::SIT);
		IsSit = true;		
		ChangeIdle = false;
		//�ϴ� �߷¾���
		GetCharacterMovement()->GravityScale = 0.0f;
		//�ϴ� ����
		GetCharacterMovement()->Velocity = FVector::ZeroVector;

		CanSit = false;
	}
}


void APlayerBase::SitEnd()
{
	IsSit = false;
	CanSit = true;
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
}
void APlayerBase::InteractInput_Implementation()
{
	IsInteract = true;
}
void APlayerBase::InteractNoneInput_Implementation()
{
	IsInteract = false;
	if (IsPlayerDeath)
	{
		PlayerHP += 1;
	}
}

//////////////FSM//////////
void APlayerBase::ChangeState(Cody_State _State)
{
	ITTPlayerState = _State;
}

void APlayerBase::PlayerDeathCheck()
{
	ChangeState(Cody_State::PlayerDeath);
	//�÷��̾�hp�� 0���� �۰ų� death�Լ��� ȣ��Ǿ����� ����˴ϴ�.
	if (0 <= PlayerHP || IsPlayerDeath == true)
	{
		IsPlayerDeath = true;
	}
	if (FullHP == PlayerHP)
	{
		IsPlayerDeath = false;
		ChangeState(Cody_State::IDLE);
	}
}

void APlayerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerBase, ITTPlayerState);
	DOREPLIFETIME(APlayerBase, IsMoveEnd);
	DOREPLIFETIME(APlayerBase, CurrentAnimationEnd);
	DOREPLIFETIME(APlayerBase, bCanDash);
	DOREPLIFETIME(APlayerBase, bIsDashingStart);
	DOREPLIFETIME(APlayerBase, IsSit);
	DOREPLIFETIME(APlayerBase, CanSit);
	DOREPLIFETIME(APlayerBase, CharacterJumpCount);
	DOREPLIFETIME(APlayerBase, IsBig);
	DOREPLIFETIME(APlayerBase, ChangeIdle);
	DOREPLIFETIME(APlayerBase, TestRotator);
	DOREPLIFETIME(APlayerBase, MoveDirection);
	DOREPLIFETIME(APlayerBase, MoveInput);
	DOREPLIFETIME(APlayerBase, ControllerRotation);
	DOREPLIFETIME(APlayerBase, CustomTargetRotation);
	DOREPLIFETIME(APlayerBase, WorldForwardVector);
	DOREPLIFETIME(APlayerBase, WorldRightVector);
	DOREPLIFETIME(APlayerBase, DashDirection)
	DOREPLIFETIME(APlayerBase, DashDistance);
	DOREPLIFETIME(APlayerBase, DashVelocity);
	DOREPLIFETIME(APlayerBase, bIsDashing);
	DOREPLIFETIME(APlayerBase, bIsDashingStart);
	DOREPLIFETIME(APlayerBase, bCanDash);
	DOREPLIFETIME(APlayerBase, DashDuration);
	DOREPLIFETIME(APlayerBase, DashStartTime);
	DOREPLIFETIME(APlayerBase, DashCurrentTime);
	DOREPLIFETIME(APlayerBase, CustomPlayerJumpCount);
	DOREPLIFETIME(APlayerBase, IsInteract);
	DOREPLIFETIME(APlayerBase, IsFly);
}






void APlayerBase::TestFunction()
{
	ChangeState(Cody_State::FLYING);
	IsFly = !IsFly;
}

