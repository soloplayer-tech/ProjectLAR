#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LItemDataAsset.h"
#include "LItemTypes.h"
#include "LInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct PROJECTLAR_API FLInventorySlot
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<ULItemDataAsset> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "0"))
	int32 Quantity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "0"))
	int32 EnhancementLevel = 0;

	bool IsEmpty() const
	{
		return !ItemData || Quantity <= 0;
	}

	void Clear()
	{
		ItemData = nullptr;
		Quantity = 0;
		EnhancementLevel = 0;
	}
};

USTRUCT(BlueprintType)
struct PROJECTLAR_API FLInventoryInitialItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<ULItemDataAsset> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 Quantity = 1;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTLAR_API ULInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULInventoryComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeInventorySlots();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FLInventorySlot> GetSlots() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool GetSlot(int32 SlotIndex, FLInventorySlot& OutSlot) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsValidSlotIndex(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Currency")
	void AddGold(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Currency")
	bool SpendGold(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Inventory|Currency")
	int32 GetGold() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(ULItemDataAsset* ItemData, int32 Quantity, int32& OutRemaining);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemAtSlot(int32 SlotIndex, int32 Quantity);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 CountItemByID(FName ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemByID(FName ItemID, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool MoveItem(int32 FromSlotIndex, int32 ToSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItemAtSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickSlot")
	bool AssignQuickItemSlot(int32 QuickSlotIndex, ULItemDataAsset* ItemData);

	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickSlot")
	bool ClearQuickItemSlot(int32 QuickSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickSlot")
	bool MoveQuickItemSlot(int32 FromQuickSlotIndex, int32 ToQuickSlotIndex);

	UFUNCTION(BlueprintPure, Category = "Inventory|QuickSlot")
	ULItemDataAsset* GetQuickItemSlotData(int32 QuickSlotIndex) const;

	UFUNCTION(BlueprintPure, Category = "Inventory|QuickSlot")
	int32 GetQuickItemSlotQuantity(int32 QuickSlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickSlot")
	bool UseQuickItemSlot(int32 QuickSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	bool EquipWeaponFromSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	bool UnequipWeapon();

	UFUNCTION(BlueprintCallable, Category = "Inventory|Weapon")
	bool UnequipWeaponToSlot(int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category = "Inventory|Weapon")
	int32 GetEquippedWeaponSlotIndex() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Weapon")
	bool GetEquippedWeaponSlot(FLInventorySlot& OutSlot) const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Weapon")
	int32 GetEquippedWeaponAttackPower() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Enhance")
	bool CanEnhanceWeaponAtSlot(int32 SlotIndex, FText& OutFailureReason) const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Enhance")
	bool CanEnhanceEquippedWeapon(FText& OutFailureReason) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Enhance")
	bool EnhanceWeaponAtSlot(int32 SlotIndex, FLWeaponEnhanceResult& OutResult);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Enhance")
	bool EnhanceEquippedWeapon(FLWeaponEnhanceResult& OutResult);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Save")
	bool SaveInventoryToSaveGame();

	UFUNCTION(BlueprintCallable, Category = "Inventory|Save")
	bool LoadInventoryFromSaveGame();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 SlotCount = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FLInventorySlot> Slots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Currency", meta = (ClampMin = "0"))
	int32 Gold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Weapon")
	int32 EquippedWeaponSlotIndex = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Weapon")
	FLInventorySlot EquippedWeaponSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Initial")
	bool bGrantInitialItemsWhenNoSave = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Initial", meta = (ClampMin = "0"))
	int32 InitialGold = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Initial")
	TArray<FLInventoryInitialItem> InitialItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|QuickSlot")
	TArray<TObjectPtr<ULItemDataAsset>> QuickItemSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Save")
	bool bAutoLoadInventory = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Save")
	bool bAutoSaveInventory = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Save")
	FString SaveSlotName = TEXT("PlayerSaveSlot");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Save")
	int32 SaveUserIndex = 0;

private:
	void AutoSaveIfEnabled();
	void GrantInitialItems();
	void InitializeQuickItemSlots();
	void AssignFirstConsumableToFirstQuickSlotIfEmpty();
	void NotifyEquippedWeaponChanged() const;
	bool IsValidQuickItemSlotIndex(int32 QuickSlotIndex) const;
	bool IsQuickSlotAssignableItem(const ULItemDataAsset* ItemData) const;
	bool IsSameQuickItemData(
		const ULItemDataAsset* FirstItemData,
		const ULItemDataAsset* SecondItemData
	) const;
	void ClearDuplicateQuickItemSlots(
		const ULItemDataAsset* ItemData,
		int32 ExceptQuickSlotIndex
	);
	bool IsSameStackableItem(const FLInventorySlot& Slot, const ULItemDataAsset* ItemData) const;
	bool DoesItemMatchID(const ULItemDataAsset* ItemData, FName ItemID) const;
	bool IsWeaponSlotEnhanceable(int32 SlotIndex, FText& OutFailureReason) const;
};
