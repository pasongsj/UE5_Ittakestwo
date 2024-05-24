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
    ViewportSizex = 0;
    ViewportSizey = 0;

    SpawnTime = FDateTime::Now();
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
            WidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }


        return;
    } 

    FDateTime CurTime = FDateTime::Now();
    if ((CurTime - SpawnTime).GetTotalSeconds() < 10)
    {
        return;
    }
    if (bOnlyCody && WidgetInstance)
    {
       /* if (false == IsWidgetVisible())
        {
            WidgetInstance->SetVisibility(ESlateVisibility::Hidden);
            return;
        }
        else
        {
            WidgetInstance->SetVisibility(ESlateVisibility::Visible);
        }*/

        if (false == WidgetInstance->IsVisible())
        {
            return;
        }

        if(!CodyViewController)
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
            else
            {
                return;
            }


            if (!It)
            {
                return;
            }
            if(It->Get())
            {
                CodyViewController = It->Get();
                CodyViewController->GetViewportSize(ViewportSizex, ViewportSizey);
            }
        }
		FVector TargetActorLocation = TargetActor->GetActorLocation();

        // ������ ���� ��ġ�� ��ũ�� ��ǥ�� ��ȯ�Ͽ� ��ġ Ȯ��
        FVector2D ScreenPosition;
        if (UGameplayStatics::ProjectWorldToScreen(CodyViewController, GetComponentLocation(), ScreenPosition))
        {
            ScreenPosition.X -= 50.0f;
            ScreenPosition.Y -= 50.0f;
            // ��ũ�� ��ǥ�� ����Ʈ�� ���� ���� �ִ��� Ȯ��

            if (ScreenPosition.X >= ViewportSizex / 2 && ScreenPosition.X <= ViewportSizex &&
                ScreenPosition.Y >= 0 && ScreenPosition.Y <= ViewportSizey)
            {
                SetCodyWidget(ScreenPosition, true);
            }
            else
            {
                SetCodyWidget(ScreenPosition, false);
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
}