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
		BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
		MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
		RootComponent = MeshComp;
		BoxComp->SetupAttachment(MeshComp);

		PivotComp = CreateDefaultSubobject<USceneComponent>(TEXT("PivotComp"));;
		PivotComp->SetupAttachment(MeshComp);
		PivotComp->SetRelativeLocation(FVector(400.f, 0.f, 400.f));
	}
}

// Called when the game starts or when spawned
void AGravityPath::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AGravityPath::OnOverlapBegin);
		BoxComp->OnComponentEndOverlap.AddDynamic(this, &AGravityPath::OnOverlapEnd);
		
		//FWalkableSlopeOverride Slope;
		//Slope.SetWalkableSlopeBehavior(EWalkableSlopeBehavior::WalkableSlope_Increase);
		//Slope.SetWalkableSlopeAngle(100.f);
		//MeshComp->SetWalkableSlopeOverride(Slope);
	}
}

void AGravityPath::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerMay = Cast<AMay>(OtherActor);
}

void AGravityPath::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (PlayerMay!= nullptr)
	{
		PlayerMay->SetOnGravityPath(false);
		PlayerMay->GetCharacterMovement()->SetGravityDirection(-FVector::UpVector);

		FVector ControllerRotVec = PlayerMay->GetControlRotation().Vector();

		FVector NewForwardVector = FVector::CrossProduct(ControllerRotVec.RightVector, -FVector::UpVector);
		FVector NewRightVector = FVector::CrossProduct(-FVector::UpVector, -ControllerRotVec.ForwardVector);
		FRotator NewRotation = FMatrix(NewForwardVector, NewRightVector, -FVector::UpVector, FVector::OneVector).Rotator();
		PlayerMay->SetGravityRotator(NewRotation);
		PlayerMay = nullptr;
	}
}

// Called every frame
void AGravityPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (true == HasAuthority())
	{
		if (PlayerMay!=nullptr&&BoxComp->IsOverlappingActor(PlayerMay))
		{

			FVector StartPos = PlayerMay->GetActorLocation();
			FVector EndPos = StartPos *-PlayerMay->GetActorUpVector();

			UE_LOG(LogTemp, Display, TEXT("StartPos %s"), *StartPos.ToString());
			UE_LOG(LogTemp, Display, TEXT("EndPos %s"), *EndPos.ToString());
			UE_LOG(LogTemp, Display, TEXT("Foward %s"), *PlayerMay->GetActorForwardVector().ToString());

			bool IsHit = false;
			FCollisionQueryParams ColQueryParam;
			ColQueryParam.AddIgnoredActor(PlayerMay);
			//EndPos.Normalize();
			IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos *1000.f,ECollisionChannel::ECC_Visibility, ColQueryParam);

			DrawDebugLine(GetWorld(), StartPos, EndPos * 1000.f,FColor::Red,false,1.f,0,1.f);
			if (IsHit&& HitResult.GetActor() == this)
			{
				HitResult.ImpactNormal.Y = 0.f;
				PlayerMay->SetOnGravityPath(true);
				UE_LOG(LogTemp, Display, TEXT("ImpactNormal %s"), *HitResult.ImpactNormal.ToString());
				PlayerMay->GetCharacterMovement()->SetGravityDirection(-HitResult.ImpactNormal);

				FVector ControllerRotVec= PlayerMay->GetControlRotation().Vector(); 

				/*FVector NewForwardVector = FVector::CrossProduct(PlayerMay->GetActorRightVector(), HitResult.ImpactNormal);
				FVector NewRightVector = FVector::CrossProduct(-HitResult.ImpactNormal, -PlayerMay->GetActorForwardVector());
				FRotator NewRotation = FMatrix(NewForwardVector, NewRightVector, HitResult.ImpactNormal, FVector::OneVector).Rotator();*/
				FVector NewForwardVector = FVector::CrossProduct(ControllerRotVec.RightVector, HitResult.ImpactNormal);
				FVector NewRightVector = FVector::CrossProduct(-HitResult.ImpactNormal, -ControllerRotVec.ForwardVector);
				FRotator NewRotation = FMatrix(NewForwardVector, NewRightVector, HitResult.ImpactNormal, FVector::OneVector).Rotator();

				UE_LOG(LogTemp, Display, TEXT("Forward %s"), *NewForwardVector.ToString());
				UE_LOG(LogTemp, Display, TEXT("Right %s"), *NewForwardVector.ToString());


				//NewRotation.Yaw = PlayerMay->GetControlRotation().Yaw;
				PlayerMay->SetGravityRotator(NewRotation);

				PlayerMay->GetController()->SetControlRotation(NewRotation);
			}
		}
	}
}

