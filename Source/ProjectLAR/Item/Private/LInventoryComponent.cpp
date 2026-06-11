#include "LInventoryComponent.h"

#include "Kismet/GameplayStatics.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"
#include "ProjectLAR/Save/Public/LPlayerSaveGames.h"

ULInventoryComponent::ULInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeInventorySlots();
	InitializeQuickItemSlots();

	bool bLoaded = false;

	if (bAutoLoadInventory)
	{
		bLoaded = LoadInventoryFromSaveGame();
	}

	if (!bLoaded && bGrantInitialItemsWhenNoSave)
	{
		GrantInitialItems();
	}

	AssignFirstConsumableToFirstQuickSlotIfEmpty();
	NotifyEquippedWeaponChanged();
}

void ULInventoryComponent::InitializeInventorySlots()
{
	constexpr int32 MinimumInventorySlotCount = 100;
	SlotCount = FMath::Max(MinimumInventorySlotCount, SlotCount);

	if (Slots.Num() < SlotCount)
	{
		Slots.SetNum(SlotCount);
	}

	InitializeQuickItemSlots();
}

TArray<FLInventorySlot> ULInventoryComponent::GetSlots() const
{
	return Slots;
}

bool ULInventoryComponent::GetSlot(
	int32 SlotIndex,
	FLInventorySlot& OutSlot
) const
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		OutSlot.Clear();
		return false;
	}

	OutSlot = Slots[SlotIndex];
	return true;
}

bool ULInventoryComponent::IsValidSlotIndex(int32 SlotIndex) const
{
	return Slots.IsValidIndex(SlotIndex);
}

void ULInventoryComponent::AddGold(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	Gold += Amount;
	AutoSaveIfEnabled();
}

bool ULInventoryComponent::SpendGold(int32 Amount)
{
	if (Amount <= 0)
	{
		return true;
	}

	if (Gold < Amount)
	{
		return false;
	}

	Gold -= Amount;
	AutoSaveIfEnabled();
	return true;
}

int32 ULInventoryComponent::GetGold() const
{
	return Gold;
}

bool ULInventoryComponent::AddItem(
	ULItemDataAsset* ItemData,
	int32 Quantity,
	int32& OutRemaining
)
{
	OutRemaining = Quantity;

	if (!ItemData || Quantity <= 0)
	{
		return false;
	}

	InitializeInventorySlots();

	const int32 MaxStack = FMath::Max(1, ItemData->MaxStack);
	const bool bStackable = ItemData->IsStackable();

	if (bStackable)
	{
		for (FLInventorySlot& Slot : Slots)
		{
			if (OutRemaining <= 0)
			{
				break;
			}

			if (!IsSameStackableItem(Slot, ItemData))
			{
				continue;
			}

			const int32 AddableCount = MaxStack - Slot.Quantity;

			if (AddableCount <= 0)
			{
				continue;
			}

			const int32 AddCount = FMath::Min(AddableCount, OutRemaining);
			Slot.Quantity += AddCount;
			OutRemaining -= AddCount;
		}
	}

	for (FLInventorySlot& Slot : Slots)
	{
		if (OutRemaining <= 0)
		{
			break;
		}

		if (!Slot.IsEmpty())
		{
			continue;
		}

		Slot.ItemData = ItemData;
		Slot.EnhancementLevel = 0;

		if (bStackable)
		{
			const int32 AddCount = FMath::Min(MaxStack, OutRemaining);
			Slot.Quantity = AddCount;
			OutRemaining -= AddCount;
		}
		else
		{
			Slot.Quantity = 1;
			OutRemaining -= 1;
		}
	}

	if (OutRemaining != Quantity)
	{
		AutoSaveIfEnabled();
	}

	return OutRemaining <= 0;
}

bool ULInventoryComponent::RemoveItemAtSlot(
	int32 SlotIndex,
	int32 Quantity
)
{
	if (!IsValidSlotIndex(SlotIndex) || Quantity <= 0)
	{
		return false;
	}

	FLInventorySlot& Slot = Slots[SlotIndex];

	if (Slot.IsEmpty() || Slot.Quantity < Quantity)
	{
		return false;
	}

	Slot.Quantity -= Quantity;

	if (Slot.Quantity <= 0)
	{
		Slot.Clear();

		if (EquippedWeaponSlotIndex == SlotIndex)
		{
			EquippedWeaponSlotIndex = INDEX_NONE;
			NotifyEquippedWeaponChanged();
		}
	}

	AutoSaveIfEnabled();
	return true;
}

int32 ULInventoryComponent::CountItemByID(FName ItemID) const
{
	if (ItemID.IsNone())
	{
		return 0;
	}

	int32 Count = 0;

	for (const FLInventorySlot& Slot : Slots)
	{
		if (Slot.IsEmpty())
		{
			continue;
		}

		if (DoesItemMatchID(Slot.ItemData, ItemID))
		{
			Count += Slot.Quantity;
		}
	}

	return Count;
}

bool ULInventoryComponent::RemoveItemByID(
	FName ItemID,
	int32 Quantity
)
{
	if (ItemID.IsNone() || Quantity <= 0)
	{
		return false;
	}

	if (CountItemByID(ItemID) < Quantity)
	{
		return false;
	}

	int32 Remaining = Quantity;

	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		FLInventorySlot& Slot = Slots[Index];

		if (Remaining <= 0)
		{
			break;
		}

		if (Slot.IsEmpty() || !DoesItemMatchID(Slot.ItemData, ItemID))
		{
			continue;
		}

		const int32 RemoveCount = FMath::Min(Slot.Quantity, Remaining);
		Slot.Quantity -= RemoveCount;
		Remaining -= RemoveCount;

		if (Slot.Quantity <= 0)
		{
			Slot.Clear();

			if (EquippedWeaponSlotIndex == Index)
			{
				EquippedWeaponSlotIndex = INDEX_NONE;
				NotifyEquippedWeaponChanged();
			}
		}
	}

	AutoSaveIfEnabled();
	return true;
}

bool ULInventoryComponent::MoveItem(
	int32 FromSlotIndex,
	int32 ToSlotIndex
)
{
	InitializeInventorySlots();

	if (!IsValidSlotIndex(FromSlotIndex) || !IsValidSlotIndex(ToSlotIndex))
	{
		return false;
	}

	if (FromSlotIndex == ToSlotIndex)
	{
		return true;
	}

	FLInventorySlot& FromSlot = Slots[FromSlotIndex];
	FLInventorySlot& ToSlot = Slots[ToSlotIndex];

	if (FromSlot.IsEmpty())
	{
		return false;
	}

	if (!ToSlot.IsEmpty() && IsSameStackableItem(ToSlot, FromSlot.ItemData))
	{
		const int32 MaxStack = FMath::Max(1, FromSlot.ItemData->MaxStack);
		const int32 AddableCount = MaxStack - ToSlot.Quantity;

		if (AddableCount > 0)
		{
			const int32 MoveCount = FMath::Min(AddableCount, FromSlot.Quantity);
			ToSlot.Quantity += MoveCount;
			FromSlot.Quantity -= MoveCount;

			if (FromSlot.Quantity <= 0)
			{
				FromSlot.Clear();
			}

			AutoSaveIfEnabled();
			return true;
		}
	}

	Swap(Slots[FromSlotIndex], Slots[ToSlotIndex]);

	if (EquippedWeaponSlotIndex == FromSlotIndex)
	{
		EquippedWeaponSlotIndex = ToSlotIndex;
	}
	else if (EquippedWeaponSlotIndex == ToSlotIndex)
	{
		EquippedWeaponSlotIndex = FromSlotIndex;
	}

	AutoSaveIfEnabled();
	return true;
}

bool ULInventoryComponent::UseItemAtSlot(int32 SlotIndex)
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		return false;
	}

	FLInventorySlot& Slot = Slots[SlotIndex];

	if (Slot.IsEmpty())
	{
		return false;
	}

	if (Slot.ItemData->IsWeapon())
	{
		return EquipWeaponFromSlot(SlotIndex);
	}

	if (!Slot.ItemData->IsConsumable())
	{
		return false;
	}

	ALPlayerCharacter* PlayerCharacter =
		Cast<ALPlayerCharacter>(GetOwner());

	if (!PlayerCharacter)
	{
		return false;
	}

	const float RestoreHPAmount = Slot.ItemData->RestoreHPAmount;
	const float RestoreManaAmount = Slot.ItemData->RestoreManaAmount;

	if (RestoreHPAmount <= 0.0f && RestoreManaAmount <= 0.0f)
	{
		return false;
	}

	if (RestoreHPAmount > 0.0f)
	{
		PlayerCharacter->RecoverHP(RestoreHPAmount);
	}

	if (RestoreManaAmount > 0.0f)
	{
		PlayerCharacter->RecoverMana(RestoreManaAmount);
	}

	return RemoveItemAtSlot(SlotIndex, 1);
}

bool ULInventoryComponent::AssignQuickItemSlot(
	int32 QuickSlotIndex,
	ULItemDataAsset* ItemData
)
{
	InitializeQuickItemSlots();

	if (!IsValidQuickItemSlotIndex(QuickSlotIndex)
		|| !ItemData
		|| !IsQuickSlotAssignableItem(ItemData))
	{
		return false;
	}

	ClearDuplicateQuickItemSlots(ItemData, QuickSlotIndex);
	QuickItemSlots[QuickSlotIndex] = ItemData;
	AutoSaveIfEnabled();
	return true;
}

bool ULInventoryComponent::ClearQuickItemSlot(int32 QuickSlotIndex)
{
	InitializeQuickItemSlots();

	if (!IsValidQuickItemSlotIndex(QuickSlotIndex))
	{
		return false;
	}

	QuickItemSlots[QuickSlotIndex] = nullptr;
	AutoSaveIfEnabled();
	return true;
}

bool ULInventoryComponent::MoveQuickItemSlot(
	int32 FromQuickSlotIndex,
	int32 ToQuickSlotIndex
)
{
	InitializeQuickItemSlots();

	if (!IsValidQuickItemSlotIndex(FromQuickSlotIndex)
		|| !IsValidQuickItemSlotIndex(ToQuickSlotIndex))
	{
		return false;
	}

	if (FromQuickSlotIndex == ToQuickSlotIndex)
	{
		return true;
	}

	if (!QuickItemSlots[FromQuickSlotIndex])
	{
		return false;
	}

	if (IsSameQuickItemData(
		QuickItemSlots[FromQuickSlotIndex],
		QuickItemSlots[ToQuickSlotIndex]
	))
	{
		QuickItemSlots[FromQuickSlotIndex] = nullptr;
		AutoSaveIfEnabled();
		return true;
	}

	Swap(
		QuickItemSlots[FromQuickSlotIndex],
		QuickItemSlots[ToQuickSlotIndex]
	);

	AutoSaveIfEnabled();
	return true;
}

ULItemDataAsset* ULInventoryComponent::GetQuickItemSlotData(
	int32 QuickSlotIndex
) const
{
	if (!QuickItemSlots.IsValidIndex(QuickSlotIndex))
	{
		return nullptr;
	}

	return QuickItemSlots[QuickSlotIndex].Get();
}

int32 ULInventoryComponent::GetQuickItemSlotQuantity(
	int32 QuickSlotIndex
) const
{
	const ULItemDataAsset* ItemData =
		GetQuickItemSlotData(QuickSlotIndex);

	if (!ItemData)
	{
		return 0;
	}

	return CountItemByID(ItemData->ItemID);
}

bool ULInventoryComponent::UseQuickItemSlot(int32 QuickSlotIndex)
{
	ULItemDataAsset* ItemData =
		GetQuickItemSlotData(QuickSlotIndex);

	if (!IsQuickSlotAssignableItem(ItemData))
	{
		return false;
	}

	for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); ++SlotIndex)
	{
		const FLInventorySlot& Slot = Slots[SlotIndex];

		if (Slot.IsEmpty()
			|| !Slot.ItemData)
		{
			continue;
		}

		const bool bMatchesQuickItem =
			ItemData->ItemID.IsNone()
				? Slot.ItemData == ItemData
				: DoesItemMatchID(Slot.ItemData, ItemData->ItemID);

		if (!bMatchesQuickItem)
		{
			continue;
		}

		return UseItemAtSlot(SlotIndex);
	}

	return false;
}

bool ULInventoryComponent::EquipWeaponFromSlot(int32 SlotIndex)
{
	InitializeInventorySlots();

	if (!IsValidSlotIndex(SlotIndex))
	{
		return false;
	}

	FLInventorySlot& Slot = Slots[SlotIndex];

	if (Slot.IsEmpty() || !Slot.ItemData->IsWeapon())
	{
		return false;
	}

	FLInventorySlot NewEquippedWeaponSlot = Slot;

	if (!EquippedWeaponSlot.IsEmpty())
	{
		Slot = EquippedWeaponSlot;
	}
	else
	{
		Slot.Clear();
	}

	EquippedWeaponSlot = NewEquippedWeaponSlot;
	EquippedWeaponSlotIndex = INDEX_NONE;
	AutoSaveIfEnabled();
	NotifyEquippedWeaponChanged();
	return true;
}

bool ULInventoryComponent::UnequipWeapon()
{
	if (EquippedWeaponSlot.IsEmpty())
	{
		return false;
	}

	InitializeInventorySlots();

	for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); ++SlotIndex)
	{
		if (Slots[SlotIndex].IsEmpty())
		{
			return UnequipWeaponToSlot(SlotIndex);
		}
	}

	return false;
}

bool ULInventoryComponent::UnequipWeaponToSlot(int32 SlotIndex)
{
	InitializeInventorySlots();

	if (!IsValidSlotIndex(SlotIndex)
		|| EquippedWeaponSlot.IsEmpty()
		|| !EquippedWeaponSlot.ItemData
		|| !EquippedWeaponSlot.ItemData->IsWeapon())
	{
		return false;
	}

	FLInventorySlot& TargetSlot = Slots[SlotIndex];

	if (TargetSlot.IsEmpty())
	{
		TargetSlot = EquippedWeaponSlot;
		EquippedWeaponSlot.Clear();
	}
	else if (TargetSlot.ItemData && TargetSlot.ItemData->IsWeapon())
	{
		Swap(TargetSlot, EquippedWeaponSlot);
	}
	else
	{
		return false;
	}

	EquippedWeaponSlotIndex = INDEX_NONE;
	AutoSaveIfEnabled();
	NotifyEquippedWeaponChanged();
	return true;
}

int32 ULInventoryComponent::GetEquippedWeaponSlotIndex() const
{
	return EquippedWeaponSlotIndex;
}

bool ULInventoryComponent::GetEquippedWeaponSlot(
	FLInventorySlot& OutSlot
) const
{
	OutSlot = EquippedWeaponSlot;

	return !OutSlot.IsEmpty()
		&& OutSlot.ItemData
		&& OutSlot.ItemData->IsWeapon();
}

int32 ULInventoryComponent::GetEquippedWeaponAttackPower() const
{
	FLInventorySlot EquippedSlot;

	if (!GetEquippedWeaponSlot(EquippedSlot))
	{
		return 0;
	}

	return EquippedSlot.ItemData->GetAttackPowerAtEnhancementLevel(
		EquippedSlot.EnhancementLevel
	);
}

bool ULInventoryComponent::CanEnhanceWeaponAtSlot(
	int32 SlotIndex,
	FText& OutFailureReason
) const
{
	if (!IsWeaponSlotEnhanceable(SlotIndex, OutFailureReason))
	{
		return false;
	}

	const FLInventorySlot& Slot = Slots[SlotIndex];
	const ULItemDataAsset* WeaponData = Slot.ItemData;
	const int32 GoldCost = WeaponData->GetEnhanceGoldCost(Slot.EnhancementLevel);
	const int32 MaterialCost = WeaponData->GetEnhanceMaterialCost(Slot.EnhancementLevel);

	if (Gold < GoldCost)
	{
		OutFailureReason = FText::FromString(TEXT("Not enough gold."));
		return false;
	}

	if (!WeaponData->EnhanceMaterialItemID.IsNone()
		&& CountItemByID(WeaponData->EnhanceMaterialItemID) < MaterialCost)
	{
		OutFailureReason = FText::FromString(TEXT("Not enough enhancement material."));
		return false;
	}

	OutFailureReason = FText::GetEmpty();
	return true;
}

bool ULInventoryComponent::CanEnhanceEquippedWeapon(
	FText& OutFailureReason
) const
{
	if (EquippedWeaponSlot.IsEmpty()
		|| !EquippedWeaponSlot.ItemData
		|| !EquippedWeaponSlot.ItemData->IsWeapon())
	{
		OutFailureReason = FText::FromString(TEXT("Weapon slot is empty."));
		return false;
	}

	const ULItemDataAsset* WeaponData = EquippedWeaponSlot.ItemData;

	if (EquippedWeaponSlot.EnhancementLevel >= WeaponData->MaxEnhancementLevel)
	{
		OutFailureReason = FText::FromString(TEXT("Weapon is already max enhanced."));
		return false;
	}

	const int32 GoldCost =
		WeaponData->GetEnhanceGoldCost(EquippedWeaponSlot.EnhancementLevel);
	const int32 MaterialCost =
		WeaponData->GetEnhanceMaterialCost(EquippedWeaponSlot.EnhancementLevel);

	if (Gold < GoldCost)
	{
		OutFailureReason = FText::FromString(TEXT("Not enough gold."));
		return false;
	}

	if (!WeaponData->EnhanceMaterialItemID.IsNone()
		&& CountItemByID(WeaponData->EnhanceMaterialItemID) < MaterialCost)
	{
		OutFailureReason = FText::FromString(TEXT("Not enough enhancement material."));
		return false;
	}

	OutFailureReason = FText::GetEmpty();
	return true;
}

bool ULInventoryComponent::EnhanceWeaponAtSlot(
	int32 SlotIndex,
	FLWeaponEnhanceResult& OutResult
)
{
	OutResult = FLWeaponEnhanceResult();

	FText FailureReason;

	if (!CanEnhanceWeaponAtSlot(SlotIndex, FailureReason))
	{
		OutResult.FailureReason = FailureReason;
		return false;
	}

	FLInventorySlot& Slot = Slots[SlotIndex];
	ULItemDataAsset* WeaponData = Slot.ItemData;

	OutResult.bAttempted = true;
	OutResult.PreviousLevel = Slot.EnhancementLevel;
	OutResult.NewLevel = Slot.EnhancementLevel;
	OutResult.GoldCost = WeaponData->GetEnhanceGoldCost(Slot.EnhancementLevel);
	OutResult.MaterialCost = WeaponData->GetEnhanceMaterialCost(Slot.EnhancementLevel);
	OutResult.SuccessChance = WeaponData->GetEnhanceSuccessChance(Slot.EnhancementLevel);
	OutResult.MaterialItemID = WeaponData->EnhanceMaterialItemID;

	if (!SpendGold(OutResult.GoldCost))
	{
		OutResult.FailureReason = FText::FromString(TEXT("Not enough gold."));
		return false;
	}

	if (!OutResult.MaterialItemID.IsNone()
		&& OutResult.MaterialCost > 0
		&& !RemoveItemByID(OutResult.MaterialItemID, OutResult.MaterialCost))
	{
		OutResult.FailureReason = FText::FromString(TEXT("Not enough enhancement material."));
		return false;
	}

	const float Roll = FMath::FRand();
	OutResult.bSuccess = Roll <= OutResult.SuccessChance;

	if (OutResult.bSuccess)
	{
		Slot.EnhancementLevel++;
		OutResult.NewLevel = Slot.EnhancementLevel;
	}

	AutoSaveIfEnabled();

	if (EquippedWeaponSlotIndex == SlotIndex)
	{
		NotifyEquippedWeaponChanged();
	}

	return true;
}

bool ULInventoryComponent::EnhanceEquippedWeapon(
	FLWeaponEnhanceResult& OutResult
)
{
	OutResult = FLWeaponEnhanceResult();

	if (EquippedWeaponSlot.IsEmpty()
		|| !EquippedWeaponSlot.ItemData
		|| !EquippedWeaponSlot.ItemData->IsWeapon())
	{
		OutResult.FailureReason = FText::FromString(TEXT("Weapon slot is empty."));
		return false;
	}

	ULItemDataAsset* WeaponData = EquippedWeaponSlot.ItemData;

	if (EquippedWeaponSlot.EnhancementLevel >= WeaponData->MaxEnhancementLevel)
	{
		OutResult.FailureReason = FText::FromString(TEXT("Weapon is already max enhanced."));
		return false;
	}

	OutResult.bAttempted = true;
	OutResult.PreviousLevel = EquippedWeaponSlot.EnhancementLevel;
	OutResult.NewLevel = EquippedWeaponSlot.EnhancementLevel;
	OutResult.GoldCost =
		WeaponData->GetEnhanceGoldCost(EquippedWeaponSlot.EnhancementLevel);
	OutResult.MaterialCost =
		WeaponData->GetEnhanceMaterialCost(EquippedWeaponSlot.EnhancementLevel);
	OutResult.SuccessChance =
		WeaponData->GetEnhanceSuccessChance(EquippedWeaponSlot.EnhancementLevel);
	OutResult.MaterialItemID = WeaponData->EnhanceMaterialItemID;

	if (!SpendGold(OutResult.GoldCost))
	{
		OutResult.FailureReason = FText::FromString(TEXT("Not enough gold."));
		return false;
	}

	if (!OutResult.MaterialItemID.IsNone()
		&& OutResult.MaterialCost > 0
		&& !RemoveItemByID(OutResult.MaterialItemID, OutResult.MaterialCost))
	{
		OutResult.FailureReason = FText::FromString(TEXT("Not enough enhancement material."));
		return false;
	}

	const float Roll = FMath::FRand();
	OutResult.bSuccess = Roll <= OutResult.SuccessChance;

	if (OutResult.bSuccess)
	{
		EquippedWeaponSlot.EnhancementLevel++;
		OutResult.NewLevel = EquippedWeaponSlot.EnhancementLevel;
	}

	AutoSaveIfEnabled();
	NotifyEquippedWeaponChanged();
	return true;
}

bool ULInventoryComponent::SaveInventoryToSaveGame()
{
	ULPlayerSaveGame* SaveGameInstance = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
	{
		SaveGameInstance = Cast<ULPlayerSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex)
		);
	}

	if (!SaveGameInstance)
	{
		SaveGameInstance = Cast<ULPlayerSaveGame>(
			UGameplayStatics::CreateSaveGameObject(
				ULPlayerSaveGame::StaticClass()
			)
		);
	}

	if (!SaveGameInstance)
	{
		return false;
	}

	SaveGameInstance->SavedInventorySlots.Empty();

	for (const FLInventorySlot& Slot : Slots)
	{
		FLInventorySlotSaveData SaveData;
		SaveData.ItemData = Slot.ItemData;
		SaveData.Quantity = Slot.Quantity;
		SaveData.EnhancementLevel = Slot.EnhancementLevel;

		SaveGameInstance->SavedInventorySlots.Add(SaveData);
	}

	SaveGameInstance->SavedGold = Gold;
	SaveGameInstance->SavedEquippedWeaponSlotIndex = INDEX_NONE;
	SaveGameInstance->SavedEquippedWeaponSlot.ItemData =
		EquippedWeaponSlot.ItemData.Get();
	SaveGameInstance->SavedEquippedWeaponSlot.Quantity =
		EquippedWeaponSlot.Quantity;
	SaveGameInstance->SavedEquippedWeaponSlot.EnhancementLevel =
		EquippedWeaponSlot.EnhancementLevel;
	SaveGameInstance->SavedQuickItemSlots.Empty();

	InitializeQuickItemSlots();

	for (ULItemDataAsset* QuickItemData : QuickItemSlots)
	{
		SaveGameInstance->SavedQuickItemSlots.Add(QuickItemData);
	}

	return UGameplayStatics::SaveGameToSlot(
		SaveGameInstance,
		SaveSlotName,
		SaveUserIndex
	);
}

bool ULInventoryComponent::LoadInventoryFromSaveGame()
{
	if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
	{
		return false;
	}

	ULPlayerSaveGame* SaveGameInstance = Cast<ULPlayerSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex)
	);

	if (!SaveGameInstance)
	{
		return false;
	}

	SlotCount = FMath::Max(SlotCount, SaveGameInstance->SavedInventorySlots.Num());
	Slots.Empty();
	Slots.SetNum(FMath::Max(1, SlotCount));

	for (int32 Index = 0; Index < SaveGameInstance->SavedInventorySlots.Num(); ++Index)
	{
		const FLInventorySlotSaveData& SaveData =
			SaveGameInstance->SavedInventorySlots[Index];

		if (!Slots.IsValidIndex(Index))
		{
			break;
		}

		Slots[Index].ItemData = SaveData.ItemData.LoadSynchronous();
		Slots[Index].Quantity = SaveData.Quantity;
		Slots[Index].EnhancementLevel = FMath::Max(0, SaveData.EnhancementLevel);

		if (Slots[Index].IsEmpty())
		{
			Slots[Index].Clear();
		}
	}

	Gold = FMath::Max(0, SaveGameInstance->SavedGold);
	EquippedWeaponSlot.Clear();
	EquippedWeaponSlot.ItemData =
		SaveGameInstance->SavedEquippedWeaponSlot.ItemData.LoadSynchronous();
	EquippedWeaponSlot.Quantity =
		SaveGameInstance->SavedEquippedWeaponSlot.Quantity;
	EquippedWeaponSlot.EnhancementLevel =
		FMath::Max(0, SaveGameInstance->SavedEquippedWeaponSlot.EnhancementLevel);

	if (EquippedWeaponSlot.IsEmpty()
		&& Slots.IsValidIndex(SaveGameInstance->SavedEquippedWeaponSlotIndex)
		&& !Slots[SaveGameInstance->SavedEquippedWeaponSlotIndex].IsEmpty()
		&& Slots[SaveGameInstance->SavedEquippedWeaponSlotIndex].ItemData
		&& Slots[SaveGameInstance->SavedEquippedWeaponSlotIndex].ItemData->IsWeapon())
	{
		EquippedWeaponSlot =
			Slots[SaveGameInstance->SavedEquippedWeaponSlotIndex];
		Slots[SaveGameInstance->SavedEquippedWeaponSlotIndex].Clear();
	}

	if (EquippedWeaponSlot.IsEmpty()
		|| !EquippedWeaponSlot.ItemData
		|| !EquippedWeaponSlot.ItemData->IsWeapon())
	{
		EquippedWeaponSlot.Clear();
	}

	EquippedWeaponSlotIndex = INDEX_NONE;
	InitializeQuickItemSlots();

	for (int32 Index = 0; Index < SaveGameInstance->SavedQuickItemSlots.Num(); ++Index)
	{
		if (!QuickItemSlots.IsValidIndex(Index))
		{
			break;
		}

		QuickItemSlots[Index] =
			SaveGameInstance->SavedQuickItemSlots[Index].LoadSynchronous();
	}

	for (int32 Index = 0; Index < QuickItemSlots.Num(); ++Index)
	{
		if (QuickItemSlots[Index])
		{
			ClearDuplicateQuickItemSlots(QuickItemSlots[Index], Index);
		}
	}

	NotifyEquippedWeaponChanged();
	return true;
}

void ULInventoryComponent::AutoSaveIfEnabled()
{
	if (bAutoSaveInventory)
	{
		SaveInventoryToSaveGame();
	}
}

void ULInventoryComponent::GrantInitialItems()
{
	if (InitialGold > 0)
	{
		Gold += InitialGold;
	}

	for (const FLInventoryInitialItem& InitialItem : InitialItems)
	{
		if (!InitialItem.ItemData || InitialItem.Quantity <= 0)
		{
			continue;
		}

		int32 Remaining = 0;
		AddItem(InitialItem.ItemData, InitialItem.Quantity, Remaining);
	}

	AutoSaveIfEnabled();
}

void ULInventoryComponent::InitializeQuickItemSlots()
{
	constexpr int32 QuickItemSlotCount = 4;

	if (QuickItemSlots.Num() < QuickItemSlotCount)
	{
		QuickItemSlots.SetNum(QuickItemSlotCount);
	}
}

void ULInventoryComponent::AssignFirstConsumableToFirstQuickSlotIfEmpty()
{
	InitializeQuickItemSlots();

	if (QuickItemSlots.IsValidIndex(0)
		&& IsQuickSlotAssignableItem(QuickItemSlots[0]))
	{
		return;
	}

	for (const FLInventorySlot& Slot : Slots)
	{
		if (Slot.IsEmpty() || !IsQuickSlotAssignableItem(Slot.ItemData))
		{
			continue;
		}

		QuickItemSlots[0] = Slot.ItemData;
		AutoSaveIfEnabled();
		return;
	}
}

void ULInventoryComponent::NotifyEquippedWeaponChanged() const
{
	ALPlayerCharacter* PlayerCharacter =
		Cast<ALPlayerCharacter>(GetOwner());

	if (!PlayerCharacter)
	{
		return;
	}

	PlayerCharacter->RefreshEquippedWeaponVisual();
}

bool ULInventoryComponent::IsValidQuickItemSlotIndex(
	int32 QuickSlotIndex
) const
{
	return QuickItemSlots.IsValidIndex(QuickSlotIndex);
}

bool ULInventoryComponent::IsQuickSlotAssignableItem(
	const ULItemDataAsset* ItemData
) const
{
	return ItemData
		&& ItemData->IsConsumable()
		&& (ItemData->RestoreHPAmount > 0.0f
			|| ItemData->RestoreManaAmount > 0.0f);
}

bool ULInventoryComponent::IsSameQuickItemData(
	const ULItemDataAsset* FirstItemData,
	const ULItemDataAsset* SecondItemData
) const
{
	if (!FirstItemData || !SecondItemData)
	{
		return false;
	}

	if (!FirstItemData->ItemID.IsNone()
		&& !SecondItemData->ItemID.IsNone())
	{
		return FirstItemData->ItemID == SecondItemData->ItemID;
	}

	return FirstItemData == SecondItemData;
}

void ULInventoryComponent::ClearDuplicateQuickItemSlots(
	const ULItemDataAsset* ItemData,
	int32 ExceptQuickSlotIndex
)
{
	if (!ItemData)
	{
		return;
	}

	InitializeQuickItemSlots();

	for (int32 Index = 0; Index < QuickItemSlots.Num(); ++Index)
	{
		if (Index == ExceptQuickSlotIndex)
		{
			continue;
		}

		if (IsSameQuickItemData(QuickItemSlots[Index], ItemData))
		{
			QuickItemSlots[Index] = nullptr;
		}
	}
}

bool ULInventoryComponent::IsSameStackableItem(
	const FLInventorySlot& Slot,
	const ULItemDataAsset* ItemData
) const
{
	if (Slot.IsEmpty()
		|| !Slot.ItemData
		|| !ItemData
		|| !Slot.ItemData->IsStackable()
		|| !ItemData->IsStackable()
		|| Slot.EnhancementLevel != 0)
	{
		return false;
	}

	if (!Slot.ItemData->ItemID.IsNone() && !ItemData->ItemID.IsNone())
	{
		return Slot.ItemData->ItemID == ItemData->ItemID;
	}

	return Slot.ItemData == ItemData;
}

bool ULInventoryComponent::DoesItemMatchID(
	const ULItemDataAsset* ItemData,
	FName ItemID
) const
{
	if (!ItemData || ItemID.IsNone())
	{
		return false;
	}

	if (ItemData->ItemID == ItemID)
	{
		return true;
	}

	const FString ExpectedID = ItemID.ToString();

	if (ExpectedID.IsEmpty())
	{
		return false;
	}

	const FString AssetName = ItemData->GetName();
	const FString DataAssetName = FString(TEXT("DA_")) + ExpectedID;

	if (AssetName.Equals(ExpectedID, ESearchCase::IgnoreCase)
		|| AssetName.Equals(DataAssetName, ESearchCase::IgnoreCase))
	{
		return true;
	}

	const FString DisplayName = ItemData->DisplayName.ToString();

	return !DisplayName.IsEmpty()
		&& DisplayName.Equals(ExpectedID, ESearchCase::IgnoreCase);
}

bool ULInventoryComponent::IsWeaponSlotEnhanceable(
	int32 SlotIndex,
	FText& OutFailureReason
) const
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		OutFailureReason = FText::FromString(TEXT("Invalid weapon slot."));
		return false;
	}

	const FLInventorySlot& Slot = Slots[SlotIndex];

	if (Slot.IsEmpty())
	{
		OutFailureReason = FText::FromString(TEXT("Weapon slot is empty."));
		return false;
	}

	if (!Slot.ItemData->IsWeapon())
	{
		OutFailureReason = FText::FromString(TEXT("Item is not a weapon."));
		return false;
	}

	if (Slot.EnhancementLevel >= Slot.ItemData->MaxEnhancementLevel)
	{
		OutFailureReason = FText::FromString(TEXT("Weapon is already max enhanced."));
		return false;
	}

	OutFailureReason = FText::GetEmpty();
	return true;
}
