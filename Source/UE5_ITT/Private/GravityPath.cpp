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
	PlayerMay = Cast<AMay>(OtherActor);
}

void AGravityPath::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*if (PlayerMay!= nullptr)
	{
		PlayerMay->GetCharacterMovement()->SetGravityDirection(-FVector::UpVector);

		FVector NewForwardVector = FVector::CrossProduct(PlayerMay->GetActorRightVector(), HitResult.ImpactNormal);
		FVector NewRightVector = FVector::CrossProduct(-HitResult.ImpactNormal, -PlayerMay->GetActorForwardVector());
		FRotator NewRotation = FMatrix(NewForwardVector, NewRightVector, HitResult.ImpactNormal, FVector::OneVector).Rotator();

		PlayerMay->SetOnGravityPath(false);
		PlayerMay = nullptr;
	}*/
}

// Called every frame
void AGravityPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (true == HasAuthority())
	{
		if (PlayerMay!=nullptr&& ColBot->IsOverlappingActor(PlayerMay))
		{

			FVector StartPos = PlayerMay->GetActorLocation();
			FVector EndPos = StartPos *-PlayerMay->GetActorUpVector();

			UE_LOG(LogTemp, Display, TEXT("StartPos %s"), *StartPos.ToString());
			UE_LOG(LogTemp, Display, TEXT("EndPos %s"), *EndPos.ToString());
			UE_LOG(LogTemp, Display, TEXT("Foward %s"), *PlayerMay->GetActorForwardVector().ToString());

			bool IsHit = false;
			FCollisionQueryParams ColQueryParam;
			ColQueryParam.AddIgnoredActor(PlayerMay);

			IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos*1000.f,ECollisionChannel::ECC_Visibility, ColQueryParam);

			DrawDebugLine(GetWorld(), StartPos, HitResult.Location,FColor::Red,false,1.f,0,1.f);
			if (IsHit&& HitResult.GetActor() == this)
			{
				//�߷� ���� ���� �ö� �ִٴ°��� �˸�

				PlayerMay->SetOnGravityPath(true);

				UE_LOG(LogTemp, Display, TEXT("ImpactNormal %s"), *HitResult.ImpactNormal.ToString());

				//ǥ��븻�� ������ �߷� ���� ����
				PlayerMay->GetCharacterMovement()->SetGravityDirection(-HitResult.ImpactNormal);

				//ǥ�� �븻�� UpVector�� ���� ȸ���� ����
				FVector NewForwardVector = FVector::CrossProduct(PlayerMay->GetActorRightVector(),HitResult.ImpactNormal);
				FVector NewRightVector = FVector::CrossProduct(-HitResult.ImpactNormal, -PlayerMay->GetActorForwardVector());
				FRotator NewRotation = FMatrix(NewForwardVector, NewRightVector, HitResult.ImpactNormal,FVector::OneVector).Rotator();				

				UE_LOG(LogTemp, Display, TEXT("Forward %s"), *NewForwardVector.ToString());
				UE_LOG(LogTemp, Display, TEXT("Right %s"), *NewRightVector.ToString());

				UE_LOG(LogTemp, Display, TEXT("Bot"));

				
				PlayerMay->SetGravityRotator(NewRotation);

			}
		}
		else if (PlayerMay != nullptr && ColTop->IsOverlappingActor(PlayerMay))
		{
			PlayerMay->SetOnGravityPath(true);

			UE_LOG(LogTemp, Display, TEXT("ImpactNormal %s"), *GetActorForwardVector().ToString());

			//ǥ��븻�� ������ �߷� ���� ����
			PlayerMay->GetCharacterMovement()->SetGravityDirection(-GetActorForwardVector());

			//ǥ�� �븻�� UpVector�� ���� ȸ���� ����
			FVector NewForwardVector = FVector::CrossProduct(PlayerMay->GetActorRightVector(), GetActorForwardVector());
			FVector NewRightVector = FVector::CrossProduct(-GetActorForwardVector(), -PlayerMay-> GetActorForwardVector());
			FRotator NewRotation = FMatrix(NewForwardVector, NewRightVector, GetActorForwardVector(), FVector::OneVector).Rotator();

			UE_LOG(LogTemp, Display, TEXT("Forward %s"), *NewForwardVector.ToString());
			UE_LOG(LogTemp, Display, TEXT("Right %s"), *NewRightVector.ToString());

			UE_LOG(LogTemp, Display, TEXT("Top"));

			PlayerMay->SetGravityRotator(NewRotation);
		}
	}
}

