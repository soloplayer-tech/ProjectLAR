#include "ProjectLAR/UI/Public/LActionSlotWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "ProjectLAR/Item/Public/LInventoryComponent.h"
#include "ProjectLAR/Item/Public/LItemDataAsset.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"
#include "ProjectLAR/UI/Public/LUIDragDropOperation.h"

ELActionSlotKey ULActionSlotWidget::GetSlotKey() const
{
	return SlotKey;
}

ELActionSlotType ULActionSlotWidget::GetSlotType() const
{
	return SlotType;
}

void ULActionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedSkillID = ELPlayerSkillID::None;

	if (IMG_SlotIcon)
	{
		IMG_SlotIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		IMG_SlotIcon->SetRenderOpacity(0.0f);
	}

	if (TXT_SlotKey)
	{
		TXT_SlotKey->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (IMG_CooldownRadial)
	{
		IMG_CooldownRadial->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (TXT_Cooldown)
	{
		TXT_Cooldown->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	InitCooldownUI();
	RefreshSlotFromPlayer();
}

void ULActionSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	UpdateSlotLabel();
}

void ULActionSlotWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	RefreshSlotFromPlayer();
}

FReply ULActionSlotWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	int32 QuickItemSlotIndex = INDEX_NONE;

	if (TryConvertToQuickItemSlotIndex(QuickItemSlotIndex))
	{
		ULInventoryComponent* InventoryComponent =
			GetInventoryComponent();

		if (!InventoryComponent)
		{
			return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
		}

		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			if (InventoryComponent->GetQuickItemSlotData(QuickItemSlotIndex))
			{
				return UWidgetBlueprintLibrary::DetectDragIfPressed(
					InMouseEvent,
					this,
					EKeys::LeftMouseButton
				).NativeReply;
			}

			return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
		}

		if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			if (InventoryComponent->UseQuickItemSlot(QuickItemSlotIndex))
			{
				RefreshSlotFromPlayer();
				return FReply::Handled();
			}
		}

		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	if (SlotType != ELActionSlotType::Skill)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	ELPlayerSkillSlot PlayerSkillSlot = ELPlayerSkillSlot::Q;

	if (!TryConvertToPlayerSkillSlot(PlayerSkillSlot))
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	ALPlayerCharacter* PlayerCharacter = GetPlayerCharacter();

	if (!PlayerCharacter)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	const ELPlayerSkillID CurrentSkillID =
		PlayerCharacter->GetEquippedSkillID(PlayerSkillSlot);

	if (CurrentSkillID == ELPlayerSkillID::None)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	return UWidgetBlueprintLibrary::DetectDragIfPressed(
		InMouseEvent,
		this,
		EKeys::LeftMouseButton
	).NativeReply;
}

void ULActionSlotWidget::NativeOnDragDetected(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation
)
{
	Super::NativeOnDragDetected(
		InGeometry,
		InMouseEvent,
		OutOperation
	);

	int32 QuickItemSlotIndex = INDEX_NONE;

	if (TryConvertToQuickItemSlotIndex(QuickItemSlotIndex))
	{
		ULInventoryComponent* InventoryComponent =
			GetInventoryComponent();

		if (!InventoryComponent)
		{
			return;
		}

		ULItemDataAsset* ItemData =
			InventoryComponent->GetQuickItemSlotData(QuickItemSlotIndex);

		if (!ItemData)
		{
			return;
		}

		ULUIDragDropOperation* DragOperation =
			NewObject<ULUIDragDropOperation>();

		if (!DragOperation)
		{
			return;
		}

		DragOperation->PayloadType = ELDragPayloadType::Item;
		DragOperation->ItemData = ItemData;
		DragOperation->IconTexture = ItemData->IconTexture.Get();
		DragOperation->DisplayName = ItemData->DisplayName;
		DragOperation->ItemCount =
			InventoryComponent->GetQuickItemSlotQuantity(QuickItemSlotIndex);
		DragOperation->bFromActionSlot = true;
		DragOperation->SourceSlotKey = SlotKey;
		DragOperation->DefaultDragVisual = this;
		DragOperation->Pivot = EDragPivot::MouseDown;

		OutOperation = DragOperation;
		return;
	}

	if (SlotType != ELActionSlotType::Skill)
	{
		return;
	}

	ELPlayerSkillSlot PlayerSkillSlot = ELPlayerSkillSlot::Q;

	if (!TryConvertToPlayerSkillSlot(PlayerSkillSlot))
	{
		return;
	}

	ALPlayerCharacter* PlayerCharacter = GetPlayerCharacter();

	if (!PlayerCharacter)
	{
		return;
	}

	const ELPlayerSkillID CurrentSkillID =
		PlayerCharacter->GetEquippedSkillID(PlayerSkillSlot);

	if (CurrentSkillID == ELPlayerSkillID::None)
	{
		return;
	}

	ULUIDragDropOperation* DragOperation =
		NewObject<ULUIDragDropOperation>();

	if (!DragOperation)
	{
		return;
	}

	DragOperation->PayloadType = ELDragPayloadType::Skill;
	DragOperation->SkillID = CurrentSkillID;
	DragOperation->IconTexture = GetSkillIconTexture(CurrentSkillID);
	DragOperation->bFromActionSlot = true;
	DragOperation->SourceSlotKey = SlotKey;

	DragOperation->DefaultDragVisual = this;
	DragOperation->Pivot = EDragPivot::MouseDown;

	OutOperation = DragOperation;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("ActionSlot Drag Started / SourceSlot=%s / Skill=%s"),
		*UEnum::GetValueAsString(SlotKey),
		*UEnum::GetValueAsString(CurrentSkillID)
	);
}

void ULActionSlotWidget::NativeOnDragCancelled(
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation
)
{
	Super::NativeOnDragCancelled(
		InDragDropEvent,
		InOperation
	);

	ULUIDragDropOperation* DragOperation =
		Cast<ULUIDragDropOperation>(InOperation);

	if (!DragOperation)
	{
		return;
	}

	if (!DragOperation->bFromActionSlot)
	{
		return;
	}

	if (DragOperation->SourceSlotKey != SlotKey)
	{
		return;
	}

	int32 SourceQuickItemSlotIndex = INDEX_NONE;

	if (TryConvertToQuickItemSlotIndex(SourceQuickItemSlotIndex)
		&& DragOperation->PayloadType == ELDragPayloadType::Item)
	{
		ULInventoryComponent* InventoryComponent =
			GetInventoryComponent();

		if (!InventoryComponent)
		{
			return;
		}

		InventoryComponent->ClearQuickItemSlot(SourceQuickItemSlotIndex);
		RefreshSlotFromPlayer();

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ActionSlot Item Drag Cancelled / Clear Slot=%s"),
			*UEnum::GetValueAsString(SlotKey)
		);

		return;
	}

	ELPlayerSkillSlot SourceSkillSlot = ELPlayerSkillSlot::Q;

	if (!TryConvertToPlayerSkillSlot(SourceSkillSlot))
	{
		return;
	}

	ALPlayerCharacter* PlayerCharacter = GetPlayerCharacter();

	if (!PlayerCharacter)
	{
		return;
	}

	PlayerCharacter->EquipSkillToSlot(
		SourceSkillSlot,
		ELPlayerSkillID::None
	);

	RefreshSlotFromPlayer();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("ActionSlot Drag Cancelled / Clear Slot=%s"),
		*UEnum::GetValueAsString(SlotKey)
	);
}

bool ULActionSlotWidget::NativeOnDrop(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation
)
{
	ULUIDragDropOperation* DragOperation =
		Cast<ULUIDragDropOperation>(InOperation);

	if (!DragOperation)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ActionSlot Drop Failed: Invalid DragOperation")
		);

		return false;
	}

	if (!CanAcceptPayload(DragOperation))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ActionSlot Drop Failed: Payload mismatch / Slot=%s"),
			*UEnum::GetValueAsString(SlotKey)
		);

		return false;
	}

	if (DragOperation->PayloadType == ELDragPayloadType::Skill)
	{
		ELPlayerSkillSlot PlayerSkillSlot = ELPlayerSkillSlot::Q;

		if (!TryConvertToPlayerSkillSlot(PlayerSkillSlot))
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("ActionSlot Drop Failed: Cannot convert SlotKey to PlayerSkillSlot / Slot=%s"),
				*UEnum::GetValueAsString(SlotKey)
			);

			return false;
		}

		ALPlayerCharacter* PlayerCharacter = GetPlayerCharacter();

		if (!PlayerCharacter)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("ActionSlot Drop Failed: PlayerCharacter is null")
			);

			return false;
		}

		PlayerCharacter->EquipSkillToSlot(
			PlayerSkillSlot,
			DragOperation->SkillID
		);

		RefreshSlotFromPlayer();

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Dropped Skill %s on Slot %s"),
			*UEnum::GetValueAsString(DragOperation->SkillID),
			*UEnum::GetValueAsString(SlotKey)
		);

		return true;
	}

	if (DragOperation->PayloadType == ELDragPayloadType::Item)
	{
		int32 QuickItemSlotIndex = INDEX_NONE;

		if (!TryConvertToQuickItemSlotIndex(QuickItemSlotIndex))
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("ActionSlot Drop Failed: Cannot convert SlotKey to QuickItemSlot / Slot=%s"),
				*UEnum::GetValueAsString(SlotKey)
			);

			return false;
		}

		ULInventoryComponent* InventoryComponent =
			GetInventoryComponent();

		if (!InventoryComponent)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("ActionSlot Drop Failed: InventoryComponent is null")
			);

			return false;
		}

		if (DragOperation->bFromActionSlot)
		{
			int32 SourceQuickItemSlotIndex = INDEX_NONE;

			if (!TryConvertActionSlotKeyToQuickItemSlotIndex(
				DragOperation->SourceSlotKey,
				SourceQuickItemSlotIndex
			))
			{
				return false;
			}

			if (!InventoryComponent->MoveQuickItemSlot(
				SourceQuickItemSlotIndex,
				QuickItemSlotIndex
			))
			{
				return false;
			}
		}
		else if (!InventoryComponent->AssignQuickItemSlot(
			QuickItemSlotIndex,
			DragOperation->ItemData
		))
		{
			return false;
		}

		RefreshSlotFromPlayer();

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Dropped Item %s on Slot %s"),
			DragOperation->ItemData
				? *DragOperation->ItemData->ItemID.ToString()
				: TEXT("None"),
			*UEnum::GetValueAsString(SlotKey)
		);

		return true;
	}

	return false;
}

bool ULActionSlotWidget::CanAcceptPayload(
	const ULUIDragDropOperation* DragOperation
) const
{
	if (!DragOperation)
	{
		return false;
	}

	int32 QuickItemSlotIndex = INDEX_NONE;

	if (TryConvertToQuickItemSlotIndex(QuickItemSlotIndex)
		&& DragOperation->PayloadType == ELDragPayloadType::Item)
	{
		return DragOperation->ItemData
			&& DragOperation->ItemData->IsConsumable()
			&& (DragOperation->ItemData->RestoreHPAmount > 0.0f
				|| DragOperation->ItemData->RestoreManaAmount > 0.0f);
	}

	switch (SlotType)
	{
	case ELActionSlotType::Skill:
		return DragOperation->PayloadType == ELDragPayloadType::Skill;

	case ELActionSlotType::Ultimate:
		return DragOperation->PayloadType == ELDragPayloadType::Ultimate;

	case ELActionSlotType::Item:
		return DragOperation->PayloadType == ELDragPayloadType::Item
			&& DragOperation->ItemData
			&& DragOperation->ItemData->IsConsumable()
			&& (DragOperation->ItemData->RestoreHPAmount > 0.0f
				|| DragOperation->ItemData->RestoreManaAmount > 0.0f);

	default:
		break;
	}

	return false;
}

bool ULActionSlotWidget::TryConvertToPlayerSkillSlot(
	ELPlayerSkillSlot& OutSkillSlot
) const
{
	switch (SlotKey)
	{
	case ELActionSlotKey::Skill_Q:
		OutSkillSlot = ELPlayerSkillSlot::Q;
		return true;

	case ELActionSlotKey::Skill_W:
		OutSkillSlot = ELPlayerSkillSlot::W;
		return true;

	case ELActionSlotKey::Skill_E:
		OutSkillSlot = ELPlayerSkillSlot::E;
		return true;

	case ELActionSlotKey::Skill_R:
		OutSkillSlot = ELPlayerSkillSlot::R;
		return true;

	case ELActionSlotKey::Skill_A:
		OutSkillSlot = ELPlayerSkillSlot::A;
		return true;

	case ELActionSlotKey::Skill_S:
		OutSkillSlot = ELPlayerSkillSlot::S;
		return true;

	case ELActionSlotKey::Skill_D:
		OutSkillSlot = ELPlayerSkillSlot::D;
		return true;

	case ELActionSlotKey::Skill_F:
		OutSkillSlot = ELPlayerSkillSlot::F;
		return true;

	case ELActionSlotKey::Ultimate_V:
		OutSkillSlot = ELPlayerSkillSlot::V;
		return true;

	default:
		return false;
	}
}

bool ULActionSlotWidget::TryConvertToQuickItemSlotIndex(
	int32& OutQuickItemSlotIndex
) const
{
	return TryConvertActionSlotKeyToQuickItemSlotIndex(
		SlotKey,
		OutQuickItemSlotIndex
	);
}

bool ULActionSlotWidget::TryConvertActionSlotKeyToQuickItemSlotIndex(
	ELActionSlotKey InSlotKey,
	int32& OutQuickItemSlotIndex
) const
{
	switch (InSlotKey)
	{
	case ELActionSlotKey::Item_1:
		OutQuickItemSlotIndex = 0;
		return true;

	case ELActionSlotKey::Item_2:
		OutQuickItemSlotIndex = 1;
		return true;

	case ELActionSlotKey::Item_3:
		OutQuickItemSlotIndex = 2;
		return true;

	case ELActionSlotKey::Item_4:
		OutQuickItemSlotIndex = 3;
		return true;

	default:
		OutQuickItemSlotIndex = INDEX_NONE;
		return false;
	}
}

ALPlayerCharacter* ULActionSlotWidget::GetPlayerCharacter() const
{
	if (APlayerController* OwningPlayer = GetOwningPlayer())
	{
		if (ALPlayerCharacter* PlayerCharacter =
			Cast<ALPlayerCharacter>(OwningPlayer->GetPawn()))
		{
			return PlayerCharacter;
		}
	}

	return Cast<ALPlayerCharacter>(
		UGameplayStatics::GetPlayerCharacter(this, 0)
	);
}

ULInventoryComponent* ULActionSlotWidget::GetInventoryComponent() const
{
	ALPlayerCharacter* PlayerCharacter = GetPlayerCharacter();

	return PlayerCharacter
		? PlayerCharacter->GetInventoryComponent()
		: nullptr;
}

void ULActionSlotWidget::RefreshSlotFromPlayer()
{
	int32 QuickItemSlotIndex = INDEX_NONE;

	if (TryConvertToQuickItemSlotIndex(QuickItemSlotIndex))
	{
		RefreshItemSlotFromInventory();
		return;
	}

	if (SlotType != ELActionSlotType::Skill)
	{
		return;
	}

	RefreshSkillSlotFromPlayer();
}

void ULActionSlotWidget::RefreshSkillSlotFromPlayer()
{
	ELPlayerSkillSlot PlayerSkillSlot = ELPlayerSkillSlot::Q;

	if (!TryConvertToPlayerSkillSlot(PlayerSkillSlot))
	{
		return;
	}

	ALPlayerCharacter* PlayerCharacter = GetPlayerCharacter();

	if (!PlayerCharacter)
	{
		return;
	}

	const ELPlayerSkillID CurrentSkillID =
		PlayerCharacter->GetEquippedSkillID(PlayerSkillSlot);

	if (CachedSkillID != CurrentSkillID)
	{
		CachedSkillID = CurrentSkillID;

		SetSlotIcon(
			GetSkillIconTexture(CurrentSkillID)
		);
	}

	UpdateCooldownUI(PlayerCharacter, CurrentSkillID);
}

void ULActionSlotWidget::RefreshItemSlotFromInventory()
{
	int32 QuickItemSlotIndex = INDEX_NONE;

	if (!TryConvertToQuickItemSlotIndex(QuickItemSlotIndex))
	{
		return;
	}

	ULInventoryComponent* InventoryComponent =
		GetInventoryComponent();

	if (!InventoryComponent)
	{
		return;
	}

	ULItemDataAsset* ItemData =
		InventoryComponent->GetQuickItemSlotData(QuickItemSlotIndex);
	const int32 ItemCount =
		InventoryComponent->GetQuickItemSlotQuantity(QuickItemSlotIndex);

	if (CachedItemData != ItemData || CachedItemCount != ItemCount)
	{
		CachedItemData = ItemData;
		CachedItemCount = ItemCount;

		SetSlotIcon(
			ItemData ? ItemData->IconTexture.Get() : nullptr
		);
	}

	if (IMG_CooldownRadial)
	{
		IMG_CooldownRadial->SetVisibility(
			ESlateVisibility::Collapsed
		);
	}

	if (CooldownMaterial)
	{
		CooldownMaterial->SetScalarParameterValue(
			TEXT("CooldownPercent"),
			0.0f
		);
	}

	if (TXT_Cooldown)
	{
		const bool bShowCount = ItemData && ItemCount > 0;

		TXT_Cooldown->SetVisibility(
			bShowCount
				? ESlateVisibility::HitTestInvisible
				: ESlateVisibility::Collapsed
		);

		TXT_Cooldown->SetText(
			bShowCount
				? FText::AsNumber(ItemCount)
				: FText::GetEmpty()
		);
	}
}

void ULActionSlotWidget::SetSlotIcon(UTexture2D* IconTexture)
{
	if (!IMG_SlotIcon)
	{
		return;
	}

	if (!IconTexture)
	{
		IMG_SlotIcon->SetRenderOpacity(0.0f);
		return;
	}

	IMG_SlotIcon->SetBrushFromTexture(IconTexture, false);
	IMG_SlotIcon->SetDesiredSizeOverride(FVector2D(36.0f, 36.0f));
	IMG_SlotIcon->SetRenderOpacity(1.0f);
}

UTexture2D* ULActionSlotWidget::GetSkillIconTexture(
	ELPlayerSkillID SkillID
) const
{
	if (ALPlayerCharacter* PlayerCharacter = GetPlayerCharacter())
	{
		if (UTexture2D* DataAssetIcon =
			PlayerCharacter->GetSkillIconTexture(SkillID))
		{
			return DataAssetIcon;
		}
	}

	switch (SkillID)
	{
	case ELPlayerSkillID::Meteor:
		return MeteorIconTexture;

	case ELPlayerSkillID::IceLance:
		return IceLanceIconTexture;

	case ELPlayerSkillID::Thunder:
		return ThunderIconTexture;

	case ELPlayerSkillID::Wind:
		return WindIconTexture;

	case ELPlayerSkillID::FrostField:
		return FrostFieldIconTexture ? FrostFieldIconTexture.Get() : WindIconTexture.Get();

	case ELPlayerSkillID::None:
	default:
		return nullptr;
	}
}

void ULActionSlotWidget::InitCooldownUI()
{
	if (IMG_CooldownRadial)
	{
		CooldownMaterial = IMG_CooldownRadial->GetDynamicMaterial();

		IMG_CooldownRadial->SetVisibility(
			ESlateVisibility::Collapsed
		);

		if (CooldownMaterial)
		{
			CooldownMaterial->SetScalarParameterValue(
				TEXT("CooldownPercent"),
				0.0f
			);
		}
	}

	if (TXT_Cooldown)
	{
		TXT_Cooldown->SetVisibility(
			ESlateVisibility::Collapsed
		);

		TXT_Cooldown->SetText(FText::GetEmpty());
	}
}

void ULActionSlotWidget::UpdateCooldownUI(
	ALPlayerCharacter* PlayerCharacter,
	ELPlayerSkillID SkillID
)
{
	if (!PlayerCharacter || SkillID == ELPlayerSkillID::None)
	{
		if (IMG_CooldownRadial)
		{
			IMG_CooldownRadial->SetVisibility(
				ESlateVisibility::Collapsed
			);
		}

		if (TXT_Cooldown)
		{
			TXT_Cooldown->SetVisibility(
				ESlateVisibility::Collapsed
			);

			TXT_Cooldown->SetText(FText::GetEmpty());
		}

		if (CooldownMaterial)
		{
			CooldownMaterial->SetScalarParameterValue(
				TEXT("CooldownPercent"),
				0.0f
			);
		}

		return;
	}

	const bool bOnCooldown =
		PlayerCharacter->IsSkillIDOnCooldown(SkillID);

	const float Remaining =
		PlayerCharacter->GetSkillIDCooldownRemaining(SkillID);

	const float Ratio =
		PlayerCharacter->GetSkillIDCooldownRatio(SkillID);

	if (IMG_CooldownRadial)
	{
		IMG_CooldownRadial->SetVisibility(
			bOnCooldown
				? ESlateVisibility::HitTestInvisible
				: ESlateVisibility::Collapsed
		);
	}

	if (CooldownMaterial)
	{
		CooldownMaterial->SetScalarParameterValue(
			TEXT("CooldownPercent"),
			bOnCooldown ? Ratio : 0.0f
		);
	}

	if (TXT_Cooldown)
	{
		if (bOnCooldown)
		{
			TXT_Cooldown->SetVisibility(
				ESlateVisibility::HitTestInvisible
			);

			TXT_Cooldown->SetText(
				FText::FromString(
					FString::Printf(TEXT("%.1f"), Remaining)
				)
			);
		}
		else
		{
			TXT_Cooldown->SetVisibility(
				ESlateVisibility::Collapsed
			);

			TXT_Cooldown->SetText(FText::GetEmpty());
		}
	}
}

void ULActionSlotWidget::UpdateSlotLabel()
{
	if (!TXT_SlotKey)
	{
		return;
	}

	FString SlotText = TEXT("");

	switch (SlotKey)
	{
	case ELActionSlotKey::Skill_Q:
		SlotText = TEXT("Q");
		break;

	case ELActionSlotKey::Skill_W:
		SlotText = TEXT("W");
		break;

	case ELActionSlotKey::Skill_E:
		SlotText = TEXT("E");
		break;

	case ELActionSlotKey::Skill_R:
		SlotText = TEXT("R");
		break;

	case ELActionSlotKey::Skill_A:
		SlotText = TEXT("A");
		break;

	case ELActionSlotKey::Skill_S:
		SlotText = TEXT("S");
		break;

	case ELActionSlotKey::Skill_D:
		SlotText = TEXT("D");
		break;

	case ELActionSlotKey::Skill_F:
		SlotText = TEXT("F");
		break;

	case ELActionSlotKey::Ultimate_V:
		SlotText = TEXT("V");
		break;

	case ELActionSlotKey::Item_1:
		SlotText = TEXT("1");
		break;

	case ELActionSlotKey::Item_2:
		SlotText = TEXT("2");
		break;

	case ELActionSlotKey::Item_3:
		SlotText = TEXT("3");
		break;

	case ELActionSlotKey::Item_4:
		SlotText = TEXT("4");
		break;

	default:
		SlotText = TEXT("");
		break;
	}

	TXT_SlotKey->SetText(FText::FromString(SlotText));
}
