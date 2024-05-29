// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManageComponent.h"
#include "Sound/SoundCue.h"
#include "GameManager.h"

USoundCue* USoundManageComponent::GetCurSound()
{
	return CurSound;
}

void USoundManageComponent::ChangeSound(const FString& KeyName)
{	
	//�Ŵ������� ���� �ҷ�����
	CurSound = Cast<UGameManager>(GetWorld()->GetGameInstance())->GetSound(KeyName);

	//���� ���� ����
	SetSound(CurSound);
}
