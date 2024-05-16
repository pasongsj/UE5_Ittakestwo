// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityPath.h"
#include "Components/BoxComponent.h"
#include "May.h"

// Sets default values
AGravityPath::AGravityPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		bReplicates = true;
		SetReplicateMovement(true);
		MeshBot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshBot"));
		RootComponent = MeshBot;

		MeshTop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshTop"));
		MeshTop->SetupAttachment(MeshBot);

		ColBot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ColBot"));
		ColBot->SetupAttachment(MeshBot);

		ColTop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ColTop"));
		ColTop->SetupAttachment(MeshTop);
	}
}

// Called when the game starts or when spawned
void AGravityPath::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		ColBot->OnComponentBeginOverlap.AddDynamic(this, &AGravityPath::OnOverlapBegin);
		ColBot->OnComponentEndOverlap.AddDynamic(this, &AGravityPath::OnOverlapEnd);

		ColTop->OnComponentBeginOverlap.AddDynamic(this, &AGravityPath::OnOverlapBegin);
		ColTop->OnComponentEndOverlap.AddDynamic(this, &AGravityPath::OnOverlapEnd);
	}
}

void AGravityPath::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("May") == true)
	{
		PlayerMay = Cast<AMay>(OtherActor);
	}
}

void AGravityPath::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerMay = nullptr;
}

// Called every frame
void AGravityPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (true == HasAuthority())
	{
		if (PlayerMay != nullptr && ColTop->IsOverlappingActor(PlayerMay))
		{

			PlayerMay->GetCharacterMovement()->SetGravityDirection(-GetActorForwardVector());

			PlayerMay->GetCharacterMovement()->MovementMode = MOVE_Walking;


			//ǥ�� �븻�� UpVector�� ���� ȸ���� ����
			FVector NewForwardVector = FVector::CrossProduct(PlayerMay->GetActorRightVector(), HitResult.ImpactNormal);
			FVector NewRightVector = FVector::CrossProduct(-HitResult.ImpactNormal, -PlayerMay->GetActorForwardVector());
			FRotator NewRotation = FMatrix(NewForwardVector, NewRightVector, HitResult.ImpactNormal, FVector::OneVector).Rotator();
			PlayerMay->SetActorRotation(NewRotation);
		}
		else if (PlayerMay!=nullptr&& ColBot->IsOverlappingActor(PlayerMay))
		{

			FVector StartPos = PlayerMay->GetActorLocation();
			FVector EndPos = StartPos *-PlayerMay->GetActorUpVector();

			bool IsHit = false;
			FCollisionQueryParams ColQueryParam;
			ColQueryParam.AddIgnoredActor(PlayerMay);

			IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos*1000.f,ECollisionChannel::ECC_Visibility, ColQueryParam);

			if (IsHit&& HitResult.GetActor() == this)
			{
				//ǥ��븻�� ������ �߷� ���� ����
				PlayerMay->GetCharacterMovement()->SetGravityDirection(-HitResult.ImpactNormal);

				
				//ǥ�� �븻�� UpVector�� ���� ȸ���� ����
				FVector NewForwardVector = FVector::CrossProduct(PlayerMay->GetActorRightVector(),HitResult.ImpactNormal);
				FVector NewRightVector = FVector::CrossProduct(-HitResult.ImpactNormal, -PlayerMay->GetActorForwardVector());
				FRotator NewRotation = FMatrix(NewForwardVector, NewRightVector, HitResult.ImpactNormal,FVector::OneVector).Rotator();			

			}
		}
	}
}

