#pragma once

#include "CoreMinimal.h"
#include "LPlayerSkillSlot.generated.h"

UENUM(BlueprintType)
enum class ELPlayerSkillSlot : uint8
{
	None UMETA(DisplayName = "None"),
	Q	UMETA(DisplayName = "Q"),
	W	UMETA(DisplayName = "W"),
	E	UMETA(DisplayName = "E"),
	R 	UMETA(DisplayName = "R"),
	A 	UMETA(DisplayName = "A"),
	S 	UMETA(DisplayName = "S"),
	D 	UMETA(DisplayName = "D"),
	F 	UMETA(DisplayName = "F"),
	V   UMETA(DisplayName = "V"),
};