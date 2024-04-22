// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Logging/LogMacros.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APlayerBase::APlayerBase()
	: CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
	, FindSessionCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete))
	, JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplate))
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
	SpringArm->bDoCollisionTest = true;

	//ī�޶� ����
	PlayerCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	PlayerCameraComponent->bUsePawnControlRotation = false;

	PrimaryActorTick.bStartWithTickEnabled = true;
	

	//// OnlineSubsystem�� Access
	//IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	//if (OnlineSubsystem)
	//{
	//	// �¶��� ���� �޾ƿ���
	//	OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

	//	if (GEngine)
	//	{
	//		// OnlineSubsystem �̸� ����ϱ�
	//		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Found subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString()));
	//	}
	//}



}

void APlayerBase::GetOnlineSubsystem()
{
	// OnlineSubsystem�� Access
	IOnlineSubsystem* CurOnlineSubsystem = IOnlineSubsystem::Get();

	if (CurOnlineSubsystem)
	{
		// �¶��� ���� �޾ƿ���
		OnlineSessionInterface = CurOnlineSubsystem->GetSessionInterface();

		if (GEngine)
		{
			// OnlineSubsystem �̸� ����ϱ�
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Found subsystem %s"), *CurOnlineSubsystem->GetSubsystemName().ToString()));
		}
	}
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	GetOnlineSubsystem();
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


	IsMoveEnd = true;

	bIsDashing = false;
	bIsDashingStart = false;
	bCanDash = false;
	BigCanDash = true;

	CurrentAnimationEnd = false;
	IsSprint = false;

	CanSit = true;
	SitDuration = 0.5f;
	ChangeIdle = true;
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime >= DashStartTime + DashDuration)
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
		PlayerInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerBase::Move_Implementation);
		PlayerInput->BindAction(MoveAction, ETriggerEvent::None, this, &APlayerBase::Idle);
		PlayerInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);
		PlayerInput->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlayerBase::DashInput);
		PlayerInput->BindAction(DashAction, ETriggerEvent::None, this, &APlayerBase::DashNoneInput);
		PlayerInput->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerBase::InteractInput);
		PlayerInput->BindAction(InteractAction, ETriggerEvent::None, this, &APlayerBase::InteractNoneInput);
		PlayerInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerBase::SprintInput);
		PlayerInput->BindAction(SprintAction, ETriggerEvent::None, this, &APlayerBase::SprintNoneInput);
	}
}


//////////////////////////////ĳ���� �̵�����///////////////////////////
//���ο� �Է� Action
void APlayerBase::Idle(const FInputActionInstance& _Instance)
{
	IsSprint = false;
	IsMoveEnd = false;
	if (bCanDash == false)
		ChangeState(Cody_State::IDLE);
}

void APlayerBase::Move_Implementation(const FInputActionInstance& _Instance)
{
	//UE_LOG(LogTemp, Warning, TEXT("Move function called"));
	IsMoveEnd = true;

	if (bCanDash == false && ChangeIdle)
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

				AddControllerYawInput(CameraLookVector.X);

				// ī�޶��� ��ġ ���� ����
				// 90�� �Ѿ�� �������� Ÿ�ٱ��̿� ������ ���ľ���.
				float CurrentPitch = GetControlRotation().Pitch;
				float NewPitch = FMath::ClampAngle(CurrentPitch + CameraLookVector.Y, -90.0f, 0.0f); // -90������ 0�� ���̷� ����
				FRotator NewRotation = FRotator(NewPitch, GetControlRotation().Yaw, GetControlRotation().Roll);
				Controller->SetControlRotation(NewRotation);
			}
		}
		
	}
}

void APlayerBase::DashInput()
{
	IsSprint = false;
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
	FVector DashVelocity = DashDirection * DashDistance * 0.8f;
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
void APlayerBase::InteractInput()
{
	IsInteract = true;
}
void APlayerBase::InteractNoneInput()
{
	IsInteract = false;
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
}



void APlayerBase::CreateGameSession()
{
	// Called when pressing the 1key
	if (!OnlineSessionInterface.IsValid())
	{
		// log
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Game Session Interface is invailed")));
		}
		return;
	}

	// �̹� ������ �����Ѵٸ� ���� ������ �����Ѵ�
	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);

		// Log
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("Destroy session : %s"), NAME_GameSession));
		}
	}

	// ���� ���� �Ϸ� �� ȣ��� delegate ����Ʈ�� CreateSessionCompleteDelegate �߰�
	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// ���� �����ϱ�
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

	SessionSettings->bIsLANMatch = false;			// LAN ����
	SessionSettings->NumPublicConnections = 4;		// �ִ� ���� ���� ��
	SessionSettings->bAllowJoinInProgress = true;	// Session �����߿� ���� ���
	SessionSettings->bAllowJoinViaPresence = true;  // ���� ���� ������ ���� �������� ���� (������ presence ���)
	SessionSettings->bShouldAdvertise = true;		// ���� ������ �������� (������ �ٸ� �÷��̾�� ���� ȫ�� ����)
	SessionSettings->bUsesPresence = true;			// ���� ������ ���� ǥ��
	SessionSettings->bUseLobbiesIfAvailable = true; // �÷����� �����ϴ� ��� �κ� API ���
	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing); // ������ MatchType�� ��ο��� ����, �¶��μ��񽺿� ���� ���� ���� ȫ�� �ɼ����� ����

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
}

void APlayerBase::JoinGameSession()
{
	// ���� �������̽� ��ȿ�� �˻�
	if (!OnlineSessionInterface.IsValid())
	{
		// log
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Game Session Interface is invailed")));
		}
		return;
	}

	// Find Session Complete Delegate ���
	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);

	// Find Game Session
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;	// �˻� ����� ������ ���� �� �ִ밪
	SessionSearch->bIsLanQuery = false;			// LAN ��� ����
	SessionSearch->QuerySettings.Set(FName(TEXT("PRESENCESEARCH")), true, EOnlineComparisonOp::Equals); // ã�� ���� ������ ����� �����Ѵ�

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void APlayerBase::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// ���� ���� ����!
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Created session : %s"), *SessionName.ToString()));
		}

		// �κ� ������ �̵�
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(FString("/Game/ThirdPerson/Maps/Lobby?listen"));
		}
	}

	// ���� ���� ����
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Failed to create session!")));
		}
	}
}

void APlayerBase::OnFindSessionComplete(bool bWasSuccessful)
{
	if (!OnlineSessionInterface.IsValid()
		|| !bWasSuccessful)
		return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("======== Search Result ========")));
	}

	for (auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;

		// ��ġ Ÿ�� Ȯ���ϱ�
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		// ã�� ������ ���� ����ϱ�
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Session ID : %s / Owner : %s"), *Id, *User));
		}

		// ������ ��ġ Ÿ���� "FreeForAll"�� ��� ���� ����
		if (MatchType == FString("FreeForAll"))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Joining Match Type : %s"), *MatchType));
			}

			// Join Session Complete Delegate ��� 
			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);


			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
		}
	}
}

void APlayerBase::OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid())
		return;

	// ���ǿ� �����ߴٸ� IP Address���ͼ� �ش� ������ ����
	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Connect String : %s"), *Address));
		}

		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}
