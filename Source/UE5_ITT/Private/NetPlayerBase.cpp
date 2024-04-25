// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerBase.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

// Sets default values
ANetPlayerBase::ANetPlayerBase()
	: CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ANetPlayerBase::OnCreateSessionComplete))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//GetOnlineSubsystem();

}

// Called when the game starts or when spawned
void ANetPlayerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANetPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANetPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANetPlayerBase::GetOnlineSubsystem()
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

void ANetPlayerBase::CreateGameSession()
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

void ANetPlayerBase::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
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
			//World->ServerTravel(FString("/Game/ThirdPerson/Maps/Lobby?listen"));
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