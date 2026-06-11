#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ProjectLAR/Item/Public/LItemDataAsset.h"
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

USTRUCT(BlueprintType)
struct FLInventorySlotSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	TSoftObjectPtr<ULItemDataAsset> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	int32 Quantity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	int32 EnhancementLevel = 0;
};

UCLASS()
class PROJECTLAR_API ULPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Skill")
	TArray<FLPlayerSkillSlotSaveData> SavedSkillSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	TArray<FLInventorySlotSaveData> SavedInventorySlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	int32 SavedGold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	int32 SavedEquippedWeaponSlotIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	FLInventorySlotSaveData SavedEquippedWeaponSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Inventory")
	TArray<TSoftObjectPtr<ULItemDataAsset>> SavedQuickItemSlots;
};
