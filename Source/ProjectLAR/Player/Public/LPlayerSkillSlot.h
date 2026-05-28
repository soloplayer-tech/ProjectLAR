#pragma once

#include "CoreMinimal.h"
#include "LPlayerSkillSlot.generated.h"

UENUM(BlueprintType)
enum class ELPlayerSkillSlot : uint8
{
	Q	UMETA(DisplayName = "Q"),
	W	UMETA(DisplayName = "W"),
	E	UMETA(DisplayName = "E"),
	R 	UMETA(DisplayName = "R"),
	V   UMETA(DisplayName = "V"),
};