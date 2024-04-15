// Fill out your copyright notice in the Description page of Project Settings.


#include "CodyAnimNotify_DashStart.h"
#include "Cody.h"
#include "May.h"
void UCodyAnimNotify_DashStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp != nullptr)
	{
		ACody* Codys = Cast<ACody>(MeshComp->GetOwner());
		AMay* Mays = Cast<AMay>(MeshComp->GetOwner());
		if (Codys == nullptr && Mays == nullptr)
		{
			return;
		}
		//����� �����ִ� ĳ���Ϳ����� �޶������ؼ� �������� ���ý���� �� ����
		if(Codys!=nullptr)
			Codys->SetbIsDashingStart();
		if(Mays!=nullptr)
			Mays->SetbIsDashingStart();
	}
}