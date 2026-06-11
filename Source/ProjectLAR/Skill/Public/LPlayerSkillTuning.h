#pragma once

#include "CoreMinimal.h"
#include "LPlayerSkillTuning.generated.h"

USTRUCT(BlueprintType)
struct PROJECTLAR_API FLPlayerSkillTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Tuning", meta = (ClampMin = "0.0"))
	float ManaCost = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Tuning", meta = (ClampMin = "0.0"))
	float CooldownDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Tuning", meta = (ClampMin = "0.0"))
	float CastDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Tuning", meta = (ClampMin = "0.0"))
	float SkillLockDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Tuning")
	bool bRequiresCasting = false;
};
