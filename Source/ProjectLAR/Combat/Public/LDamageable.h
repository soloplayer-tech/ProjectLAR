#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LPlayerSkillID.h"
#include "LDamageable.generated.h"

UINTERFACE(BlueprintType)
class PROJECTLAR_API ULDamageable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTLAR_API ILDamageable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	void ReceiveSkillDamage(
		float Damage,
		AActor* DamageCauser,
		ELPlayerSkillID SkillID
	);
};