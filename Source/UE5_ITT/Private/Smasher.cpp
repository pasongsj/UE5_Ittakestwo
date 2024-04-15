// Fill out your copyright notice in the Description page of Project Settings.


#include "Smasher.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SceneComponent.h>
#include "FsmComponent.h"

// Sets default values
ASmasher::ASmasher()
{	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);

	SmasherMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SmasherMesh"));
	SmasherMesh->SetupAttachment(SceneComp);
	SetupFsm();
}

// Called when the game starts or when spawned
void ASmasher::BeginPlay()
{
	Super::BeginPlay();
	FsmComp->ChangeState(Fsm::SmashWait);
	ReleasePos = SmasherMesh->GetRelativeLocation();
	SmashPos = ReleasePos;
	SmashPos.X -= SmasherMesh->GetRelativeScale3D().X * SmashSize;
}

void ASmasher::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));

	FsmComp->CreateState(Fsm::SmashWait,
		[this]
		{

		},

		[this](float DeltaTime)
		{
			if (FsmComp->GetStateLiveTime() > 1.f)
			{
				FsmComp->ChangeState(Fsm::Smash);
			}
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Smash,
		[this]
		{
			SmashRatio = 0.f;
		},

		[this](float DeltaTime)
		{
			SmashRatio += DeltaTime*10.f;
			if (SmashRatio >= 1.f)
			{
				SmashRatio = 1.f;
				SmasherMesh->SetRelativeLocation(FMath::Lerp(ReleasePos, SmashPos, SmashRatio));
				FsmComp->ChangeState(Fsm::ReleaseWait);
				return;
			}
			SmasherMesh->SetRelativeLocation(FMath::Lerp(ReleasePos, SmashPos, SmashRatio));
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::ReleaseWait,
		[this]
		{

		},

		[this](float DeltaTime)
		{
			if (FsmComp->GetStateLiveTime() > 1.f)
			{
				FsmComp->ChangeState(Fsm::Release);
			}
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Release,
		[this]
		{
		},

		[this](float DeltaTime)
		{
			SmashRatio -= DeltaTime;
			if (SmashRatio <= 0.f)
			{
				SmashRatio = 0.f;
				SmasherMesh->SetRelativeLocation(FMath::Lerp(ReleasePos, SmashPos, SmashRatio));
				FsmComp->ChangeState(Fsm::SmashWait);
				return;
			}
			SmasherMesh->SetRelativeLocation(FMath::Lerp(ReleasePos, SmashPos, SmashRatio));
		},

		[this]
		{
		}
	);
}

// Called every frame
void ASmasher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

