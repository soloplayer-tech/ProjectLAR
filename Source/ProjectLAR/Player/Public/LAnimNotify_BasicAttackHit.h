#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "LAnimNotify_BasicAttackHit.generated.h"

UCLASS(meta = (DisplayName = "L Basic Attack Hit"))
class PROJECTLAR_API ULAnimNotify_BasicAttackHit : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;

	virtual FString GetNotifyName_Implementation() const override;
};
