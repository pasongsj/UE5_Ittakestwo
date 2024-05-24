// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionWidget.h"
#include "PlayerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UInteractionWidget::UInteractionWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Default initialization
    WidgetInstance = nullptr;
    //NearWidgetInstance = nullptr;
    //FarWidgetInstance = nullptr;
    DistanceThreshold = 1000.f; // Default distance threshold
    Distance = 1500.0f;

    bOnlyMay = false;
    bOnlyCody = false;
}


// ƽ �Լ� ����
void UInteractionWidget::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!TargetActor)
    {
        FindTargetActor();
        return;
    }

    FVector PlayerLocation = TargetActor->GetActorLocation();
    FVector SceneComponentLocation = GetComponentLocation();
    Distance = FVector::Distance(PlayerLocation, SceneComponentLocation);
    if (!WidgetInstance)
    {
        if(true == bOnlyMay)
        {
            WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), UIWidgetClass);
            SetWidget(WidgetInstance);
        }
        else if (true == bOnlyCody)
        {
            WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), UIWidgetClass);
            WidgetInstance->AddToViewport();
        }
        return;
    } 


    if (bOnlyCody && WidgetInstance)
    {
        FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
        int32 NumofPlayer = GetWorld()->GetNumPlayerControllers();
		if (NumofPlayer == 2)
        {
            It++;
        }
        else if (NumofPlayer == 4)
        {
            It++;
            It++;
        }
        //for (int index = 1; index < NumofPlayer; ++index)
        //{
        //    It++;
        //}
        
        if (!It)
        {
            return;
        }
        It->Get();
        FVector TargetActorLocation = TargetActor->GetActorLocation();

        // ������ ���� ��ġ�� ��ũ�� ��ǥ�� ��ȯ�Ͽ� ��ġ Ȯ��
        FVector2D ScreenPosition;
        if (UGameplayStatics::ProjectWorldToScreen(It->Get(), GetComponentLocation(), ScreenPosition))
        {
            ScreenPosition.X -= 50.0f;
            ScreenPosition.Y -= 50.0f;
            // ��ũ�� ��ǥ�� ����Ʈ�� ���� ���� �ִ��� Ȯ��
            int32 ViewportSizex, ViewportSizey;
            It->Get()->GetViewportSize(ViewportSizex, ViewportSizey);

            if (ScreenPosition.X >= ViewportSizex / 2 && ScreenPosition.X <= ViewportSizex &&
                ScreenPosition.Y >= 0 && ScreenPosition.Y <= ViewportSizey)
            {
                SetCodyWidget(ScreenPosition, true);
                // ���Ͱ� ��ũ���� �ִ� ���
                //UE_LOG(LogTemp, Warning, TEXT("Actor %s is on screen at (%f, %f)"), *TargetActor->GetName(), ScreenPosition.X, ScreenPosition.Y);

                //WidgetInstance->SetPositionInViewport(ScreenPosition);
                //WidgetInstance->SetVisibility(ESlateVisibility::Visible);
            }
            else
            {
                SetCodyWidget(ScreenPosition, false);

                // ���Ͱ� ��ũ���� ���� ���
                //UE_LOG(LogTemp, Warning, TEXT("Actor %s is not on screen"), *TargetActor->GetName());
                //WidgetInstance->SetVisibility(ESlateVisibility::Hidden);
            }
        }
    }

}

void UInteractionWidget::SetCodyWidget_Implementation(const FVector2D _Pos, const bool isvisible)
{
    if(true == isvisible)
    {
        WidgetInstance->SetPositionInViewport(_Pos);
        WidgetInstance->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        WidgetInstance->SetVisibility(ESlateVisibility::Hidden);
    }
}

bool UInteractionWidget::SetCodyWidget_Validate(const FVector2D _Pos, const bool isvisible)
{
    return true;
}

void UInteractionWidget::SetVisibilityBasedOnDistance()
{
    FVector PlayerLocation = TargetActor->GetActorLocation();
    FVector SceneComponentLocation = GetComponentLocation();
    Distance = FVector::Distance(PlayerLocation, SceneComponentLocation);

    // Toggle widget visibility based on distance threshold
    if (Distance < DistanceThreshold)
    {
        // Player is near, show near widget and hide far widget
        //SetWidget(NearWidgetInstance);
        //NearWidgetComponent->SetVisibility(true);
        //FarWidgetComponent->SetVisibility(false);
    }
    else
    {
        // Player is far, hide near widget and show far widget
        //SetWidget(FarWidgetInstance);
        //NearWidgetComponent->SetVisibility(false);
        //FarWidgetComponent->SetVisibility(true);
    }
}




void UInteractionWidget::FindTargetActor()
{
    TArray<AActor*> AllPlayerActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerBase::StaticClass(), AllPlayerActors);

    APlayerBase* me = Cast<APlayerBase>(GetOwner());
    // PlayerActors�� ��ȸ�ϸ� �� �÷��̾��� ó�� ����
    for (AActor* Player : AllPlayerActors)
    {
        if (me != Cast<APlayerBase>(Player))
        {
            APlayerBase* NextPlayer = Cast<APlayerBase>(Player);
            const TArray<FName>& CheckTag = NextPlayer->Tags;
            for (const FName& V : CheckTag)
            {
                if (V == FName("Cody") || V == FName("May"))
                {
                    if (true == bOnlyMay && V == FName("May"))
                    {
                        TargetActor = NextPlayer;
                        //SetWidget(NearWidgetInstance);
                        //SetWidget(FarWidgetInstance);
                        return;
                    }
                    else if (true == bOnlyCody && V == FName("Cody"))
                    {
                        TargetActor = NextPlayer;
                        //SetWidget(NearWidgetInstance);
                        //SetWidget(FarWidgetInstance);
                        return;
                    }
                }
            }
        }
    }
    // Player�� ����Ͽ� �÷��̾� ó��
}