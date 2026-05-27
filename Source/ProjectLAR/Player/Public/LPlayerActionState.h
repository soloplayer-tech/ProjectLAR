#pragma once

#include "CoreMinimal.h"
#include "LPlayerActionState.generated.h"

UENUM(BlueprintType)
enum class ELPlayerActionState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	BasicAttack		UMETA(DisplayName = "Basic Attack"),
	Dash			UMETA(DisplayName = "Dash"),
	Casting			UMETA(DisplayName = "Casting"),
	Skill			UMETA(DisplayName = "Skill"),
	HitReaction		UMETA(DisplayName = "Hit Reaction"),
	Dead			UMETA(DisplayName = "Dead")
};