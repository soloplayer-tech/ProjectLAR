#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ProjectLAR/Player/Public/LPlayerSkillSlot.h"
#include "LPlayerSkillID.h"
#include "LPlayerSaveGames.generated.h"

USTRUCT(BlueprintType)
struct FLPlayerSkillSlotSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELPlayerSkillSlot SkillSlot = ELPlayerSkillSlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELPlayerSkillID SkillID = ELPlayerSkillID::None;
};

UCLASS()
class PROJECTLAR_API ULPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Skill")
	TArray<FLPlayerSkillSlotSaveData> SavedSkillSlots;
};