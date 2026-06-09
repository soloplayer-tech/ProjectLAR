#include "LAnimNotify_BasicAttackHit.h"

#include "LPlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void ULAnimNotify_BasicAttackHit::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	ALPlayerCharacter* PlayerCharacter =
		Cast<ALPlayerCharacter>(MeshComp->GetOwner());

	if (!PlayerCharacter)
	{
		return;
	}

	PlayerCharacter->TriggerBasicAttackHit();
}

FString ULAnimNotify_BasicAttackHit::GetNotifyName_Implementation() const
{
	return TEXT("BasicAttack Hit");
}
