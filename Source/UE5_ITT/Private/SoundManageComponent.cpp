#include "SoundManageComponent.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManageComponent.h"
#include "Sound/SoundCue.h"
#include "GameManager.h"
#include "Kismet/GameplayStatics.h"

USoundCue* USoundManageComponent::GetCurSound()
{
	return CurSound;
}

void USoundManageComponent::MulticastSoundStop_Implementation()
{
	Stop();
}

void USoundManageComponent::MulticastChangeSound_Implementation(const FString& KeyName, bool bPlay, float StartTime)
{	
	//�Ŵ������� ���� �ҷ�����
	CurSound = Cast<UGameManager>(GetWorld()->GetGameInstance())->GetSound(KeyName);

	//���� ���� ����
	if (CurSound!=nullptr)
	{
		SetSound(CurSound);

		if (bPlay)
		{
			Play(StartTime);
		}
	}

}

void USoundManageComponent::MulticastPlaySoundDirect_Implementation(const FString& KeyName)
{
	USoundCue* TempCue = Cast<UGameManager>(GetWorld()->GetGameInstance())->GetSound(KeyName);
	if (TempCue!=nullptr)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), TempCue);
	}
}
