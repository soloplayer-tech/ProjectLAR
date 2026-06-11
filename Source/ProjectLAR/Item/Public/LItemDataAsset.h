#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LItemTypes.h"
#include "LItemDataAsset.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class PROJECTLAR_API ULItemDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName ItemID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	ELItemType ItemType = ELItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "1"))
	int32 MaxStack = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable", meta = (ClampMin = "0.0"))
	float RestoreHPAmount = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable", meta = (ClampMin = "0.0"))
	float RestoreManaAmount = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	ELWeaponType WeaponType = ELWeaponType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "0"))
	int32 BaseAttackPower = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "0"))
	int32 AttackPowerPerEnhancement = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = "0"))
	int32 MaxEnhancementLevel = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhancement", meta = (ClampMin = "0"))
	int32 BaseEnhanceGoldCost = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhancement", meta = (ClampMin = "0"))
	int32 EnhanceGoldCostPerLevel = 50;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhancement")
	FName EnhanceMaterialItemID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhancement", meta = (ClampMin = "0"))
	int32 BaseEnhanceMaterialCost = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhancement", meta = (ClampMin = "0"))
	int32 EnhanceMaterialCostPerLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhancement", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseEnhanceSuccessChance = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhancement", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SuccessChanceReductionPerLevel = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhancement", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinEnhanceSuccessChance = 0.1f;

	UFUNCTION(BlueprintPure, Category = "Item")
	bool IsStackable() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	bool IsWeapon() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	bool IsConsumable() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetAttackPowerAtEnhancementLevel(int32 EnhancementLevel) const;

	UFUNCTION(BlueprintPure, Category = "Enhancement")
	int32 GetEnhanceGoldCost(int32 CurrentEnhancementLevel) const;

	UFUNCTION(BlueprintPure, Category = "Enhancement")
	int32 GetEnhanceMaterialCost(int32 CurrentEnhancementLevel) const;

	UFUNCTION(BlueprintPure, Category = "Enhancement")
	float GetEnhanceSuccessChance(int32 CurrentEnhancementLevel) const;
};
