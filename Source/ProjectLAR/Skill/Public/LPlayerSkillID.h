#pragma once

#include "CoreMinimal.h"
#include "LPlayerSkillID.generated.h"

UENUM(BlueprintType)
enum class ELPlayerSkillID : uint8
{
	None			UMETA(DisplayName = "None"),
	MeteorRain		UMETA(DisplayName="MeteorRain"),
	Meteor			UMETA(DisplayName="Meteor"),
	IceLance		UMETA(DisplayName="IceLance"),
	Thunder			UMETA(DisplayName="Thunder"),
	Wind			UMETA(DisplayName="Wind"),
};