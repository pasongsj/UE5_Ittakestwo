// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMarkerUI.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
// Constructor
UPlayerMarkerUI::UPlayerMarkerUI()
{
    // Set default values or initialize as needed
    PrimaryComponentTick.bCanEverTick = true;
    PlayerMarkerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    bIsOwner = true;
    DistanceThreshold = 2500.0f;

    WidgetDefaultLocation = FVector(0.0f, 0.0f, 180.0f);
    TargetPlayer = nullptr;
}
void UPlayerMarkerUI::BeginPlay()
{

    Super::BeginPlay();
    bIsOwner = false;

    PlayerMarkerWidget->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

    // Set widget classes
    if (PlayerMarkerWidget)
    {
        PlayerMarkerWidget->SetWidgetClass(PlayerMarkerType);
    }
    // Set widget space to screen
    PlayerMarkerWidget->SetWidgetSpace(EWidgetSpace::Screen);
    if(Cast<APawn>(GetOwner()) == GetWorld()->GetFirstPlayerController()->GetPawn())
    {
        SetVisibility(true, true);
    }
    else
    {
        SetVisibility(false, true);
        bIsOwner = true;
    }

}

void UPlayerMarkerUI::FindTarget()
{
    TArray<AActor*> AllPlayerActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerBase::StaticClass(), AllPlayerActors);

    APlayerBase* me = Cast<APlayerBase>(GetOwner());
    // PlayerActors�� ��ȸ�ϸ� �� �÷��̾��� ó�� ����
    for (AActor* Player : AllPlayerActors)
    {
        if (me != Cast<APlayerBase>(Player))
        {
            TargetPlayer = Cast<APlayerBase>(Player);
            return;
        }
        // Player�� ����Ͽ� �÷��̾� ó��
    }
}

void UPlayerMarkerUI::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (true == bIsOwner)
    {
        return;
    }

    if (TargetPlayer == nullptr)
    {
        FindTarget();
        return;
    }

    

    APlayerController* FirstPlayer = GetWorld()->GetFirstPlayerController();

    FVector2D ActorScreenPosition;
    if (!FirstPlayer->ProjectWorldLocationToScreen(TargetPlayer->GetActorLocation(), ActorScreenPosition))
    {
        return; // Failed to project actor's location to screen space
    }

    int32 ViewportX, ViewportY;
    FirstPlayer->GetViewportSize(ViewportX, ViewportY);

    if ((ActorScreenPosition.X >= 0 && ActorScreenPosition.X <= ViewportX &&
        ActorScreenPosition.Y >= 0 && ActorScreenPosition.Y <= ViewportY)) // ȭ�� ���� ����
    {
        float Distance = FVector::Distance(FirstPlayer->GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

        if (DistanceThreshold < Distance)
        {
            SetVisibility(true, true);
        }
        else
        {
            SetVisibility(false, true);
        }
        SetWorldLocation(TargetPlayer->GetActorLocation());
    }
    else // ȭ�� �ۿ� ����
    {
        if (ActorScreenPosition.X < 0)
        {
            ActorScreenPosition.X = 10.0f;
        }
        else if (ActorScreenPosition.X > ViewportX)
        {
            ActorScreenPosition.X = ViewportX - 10.0f;
        }

        if (ActorScreenPosition.Y < 0)
        {
            ActorScreenPosition.Y = 10.0f;
        }
        else if (ActorScreenPosition.Y > ViewportY)
        {
            ActorScreenPosition.Y = ViewportY - 10.0f;
        }

        FVector WorldLocation, WorldDirection;
        FirstPlayer->DeprojectScreenPositionToWorld(ActorScreenPosition.X, ActorScreenPosition.Y, WorldLocation, WorldDirection);

        // ī�޶� �����̽� ��ǥ�� ��ȯ�� ��, ī�޶�� WidgetComponent�� �Ÿ� ���
        FVector CameraLocation = FirstPlayer->PlayerCameraManager->GetCameraLocation();
        FVector WidgetLocation = CameraLocation + WorldDirection * 1200.0f; // ���÷� �Ÿ��� 100.0f�� ����
        SetWorldLocation(WidgetLocation);
    }

}
