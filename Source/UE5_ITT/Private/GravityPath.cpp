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
	}
}

void AGravityPath::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerMay = Cast<AMay>(OtherActor);
}

void AGravityPath::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// Called every frame
void AGravityPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (true == HasAuthority())
	{
		if (PlayerMay!=nullptr&&BoxComp->IsOverlappingActor(PlayerMay))
		{
			//FVector Direction = MeshComp->getlocation//PlayerMay->GetActorLocation()
			//PlayerMay->GetCharacterMovement()->SetGravityDirection()
		}
	}
}

