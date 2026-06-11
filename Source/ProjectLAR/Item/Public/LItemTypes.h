#pragma once

#include "CoreMinimal.h"
#include "LItemTypes.generated.h"

UENUM(BlueprintType)
enum class ELItemType : uint8
{
	None		UMETA(DisplayName = "None"),
	Weapon		UMETA(DisplayName = "Weapon"),
	Material	UMETA(DisplayName = "Material"),
	Consumable	UMETA(DisplayName = "Consumable"),
	Currency	UMETA(DisplayName = "Currency")
};

UENUM(BlueprintType)
enum class ELWeaponType : uint8
{
	None	UMETA(DisplayName = "None"),
	Staff	UMETA(DisplayName = "Staff"),
	Sword	UMETA(DisplayName = "Sword")
};

USTRUCT(BlueprintType)
struct PROJECTLAR_API FLWeaponEnhanceResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Enhance")
	bool bAttempted = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Enhance")
	bool bSuccess = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Enhance")
	int32 PreviousLevel = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Enhance")
	int32 NewLevel = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Enhance")
	int32 GoldCost = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Enhance")
	int32 MaterialCost = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Enhance")
	float SuccessChance = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Enhance")
	FName MaterialItemID = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Enhance")
	FText FailureReason;
};
