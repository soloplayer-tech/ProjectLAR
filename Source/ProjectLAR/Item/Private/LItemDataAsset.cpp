#include "LItemDataAsset.h"

bool ULItemDataAsset::IsStackable() const
{
	return ItemType != ELItemType::Weapon && MaxStack > 1;
}

bool ULItemDataAsset::IsWeapon() const
{
	return ItemType == ELItemType::Weapon;
}

bool ULItemDataAsset::IsConsumable() const
{
	return ItemType == ELItemType::Consumable;
}

int32 ULItemDataAsset::GetAttackPowerAtEnhancementLevel(
	int32 EnhancementLevel
) const
{
	const int32 SafeLevel = FMath::Clamp(
		EnhancementLevel,
		0,
		MaxEnhancementLevel
	);

	return BaseAttackPower + AttackPowerPerEnhancement * SafeLevel;
}

int32 ULItemDataAsset::GetEnhanceGoldCost(
	int32 CurrentEnhancementLevel
) const
{
	const int32 SafeLevel = FMath::Max(0, CurrentEnhancementLevel);

	return FMath::Max(
		0,
		BaseEnhanceGoldCost + EnhanceGoldCostPerLevel * SafeLevel
	);
}

int32 ULItemDataAsset::GetEnhanceMaterialCost(
	int32 CurrentEnhancementLevel
) const
{
	if (EnhanceMaterialItemID.IsNone())
	{
		return 0;
	}

	const int32 SafeLevel = FMath::Max(0, CurrentEnhancementLevel);

	return FMath::Max(
		0,
		BaseEnhanceMaterialCost + EnhanceMaterialCostPerLevel * SafeLevel
	);
}

float ULItemDataAsset::GetEnhanceSuccessChance(
	int32 CurrentEnhancementLevel
) const
{
	const int32 SafeLevel = FMath::Max(0, CurrentEnhancementLevel);
	const float Chance =
		BaseEnhanceSuccessChance - SuccessChanceReductionPerLevel * SafeLevel;

	return FMath::Clamp(
		Chance,
		MinEnhanceSuccessChance,
		1.0f
	);
}
