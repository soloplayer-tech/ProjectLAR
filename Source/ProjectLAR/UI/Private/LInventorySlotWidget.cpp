#include "ProjectLAR/UI/Public/LInventorySlotWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InputCoreTypes.h"
#include "ProjectLAR/Item/Public/LInventoryComponent.h"
#include "ProjectLAR/UI/Public/LDraggableWindowWidget.h"
#include "ProjectLAR/UI/Public/LUIDragDropOperation.h"
#include "ProjectLAR/UI/Public/LWeaponEnhanceWindowWidget.h"

void ULInventorySlotWidget::SetupInventorySlot(
	ULInventoryComponent* InInventoryComponent,
	int32 InSlotIndex
)
{
	InventoryComponent = InInventoryComponent;
	SlotIndex = InSlotIndex;
	RefreshSlot();
}

void ULInventorySlotWidget::RefreshSlot()
{
	FLInventorySlot InventorySlot;

	const bool bHasSlot =
		InventoryComponent
		&& InventoryComponent->GetSlot(SlotIndex, InventorySlot)
		&& !InventorySlot.IsEmpty();

	if (IMG_ItemIcon)
	{
		if (bHasSlot && InventorySlot.ItemData && InventorySlot.ItemData->IconTexture)
		{
			IMG_ItemIcon->SetBrushFromTexture(InventorySlot.ItemData->IconTexture, false);
			IMG_ItemIcon->SetDesiredSizeOverride(FVector2D(34.0f, 34.0f));
			IMG_ItemIcon->SetRenderOpacity(1.0f);
		}
		else
		{
			IMG_ItemIcon->SetRenderOpacity(0.0f);
		}
	}

	if (TXT_ItemCount)
	{
		const bool bShowCount = bHasSlot && InventorySlot.Quantity > 1;
		TXT_ItemCount->SetVisibility(
			bShowCount
				? ESlateVisibility::HitTestInvisible
				: ESlateVisibility::Collapsed
		);

		TXT_ItemCount->SetText(
			bShowCount
				? FText::AsNumber(InventorySlot.Quantity)
				: FText::GetEmpty()
		);
	}

	if (TXT_EnhancementLevel)
	{
		const bool bShowLevel =
			bHasSlot
			&& InventorySlot.ItemData
			&& InventorySlot.ItemData->IsWeapon()
			&& InventorySlot.EnhancementLevel > 0;

		TXT_EnhancementLevel->SetVisibility(
			bShowLevel
				? ESlateVisibility::HitTestInvisible
				: ESlateVisibility::Collapsed
		);

		TXT_EnhancementLevel->SetText(
			bShowLevel
				? FText::FromString(
					FString::Printf(TEXT("+%d"), InventorySlot.EnhancementLevel)
				)
				: FText::GetEmpty()
		);
	}

	if (TXT_ItemName)
	{
		TXT_ItemName->SetVisibility(
			bHasSlot
				? ESlateVisibility::HitTestInvisible
				: ESlateVisibility::Collapsed
		);

		TXT_ItemName->SetText(
			bHasSlot && InventorySlot.ItemData
				? InventorySlot.ItemData->DisplayName
				: FText::GetEmpty()
		);
	}
}

int32 ULInventorySlotWidget::GetSlotIndex() const
{
	return SlotIndex;
}

void ULInventorySlotWidget::SetOwningWindow(
	ULDraggableWindowWidget* InOwningWindow
)
{
	OwningWindow = InOwningWindow;
}

void ULInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Visible);

	if (IMG_ItemIcon)
	{
		IMG_ItemIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (TXT_ItemCount)
	{
		TXT_ItemCount->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (TXT_EnhancementLevel)
	{
		TXT_EnhancementLevel->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (TXT_ItemName)
	{
		TXT_ItemName->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	RefreshSlot();
}

void ULInventorySlotWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshSlot();
}

FReply ULInventorySlotWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	if (!InventoryComponent)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	FLInventorySlot InventorySlot;

	if (!InventoryComponent->GetSlot(SlotIndex, InventorySlot) || InventorySlot.IsEmpty())
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (InventorySlot.ItemData
			&& InventorySlot.ItemData->IsWeapon())
		{
			if (ULWeaponEnhanceWindowWidget* EnhanceWindow =
				ULWeaponEnhanceWindowWidget::GetActiveWeaponEnhanceWindow())
			{
				if (EnhanceWindow->TrySetTargetFromInventorySlot(SlotIndex))
				{
					RefreshSlot();
					return FReply::Handled();
				}
			}
		}

		if (InventoryComponent->UseItemAtSlot(SlotIndex))
		{
			RefreshSlot();
			return FReply::Handled();
		}
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(
			InMouseEvent,
			this,
			EKeys::LeftMouseButton
		).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply ULInventorySlotWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	const FReply SuperReply =
		Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if (OwningWindow)
	{
		OwningWindow->BringToFront();
	}

	return SuperReply;
}

void ULInventorySlotWidget::NativeOnDragDetected(
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

	if (!InventoryComponent)
	{
		return;
	}

	FLInventorySlot InventorySlot;

	if (!InventoryComponent->GetSlot(SlotIndex, InventorySlot) || InventorySlot.IsEmpty())
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
	DragOperation->ItemData = InventorySlot.ItemData;
	DragOperation->ItemCount = InventorySlot.Quantity;
	DragOperation->SourceInventorySlotIndex = SlotIndex;
	DragOperation->IconTexture =
		InventorySlot.ItemData ? InventorySlot.ItemData->IconTexture.Get() : nullptr;
	DragOperation->DisplayName =
		InventorySlot.ItemData ? InventorySlot.ItemData->DisplayName : FText::GetEmpty();
	DragOperation->DefaultDragVisual = this;
	DragOperation->Pivot = EDragPivot::MouseDown;

	OutOperation = DragOperation;
}

bool ULInventorySlotWidget::NativeOnDrop(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation
)
{
	ULUIDragDropOperation* DragOperation =
		Cast<ULUIDragDropOperation>(InOperation);

	if (!InventoryComponent || !DragOperation)
	{
		return false;
	}

	if (DragOperation->PayloadType != ELDragPayloadType::Item)
	{
		return false;
	}

	const bool bMoved = DragOperation->bFromEquipmentSlot
		? InventoryComponent->UnequipWeaponToSlot(SlotIndex)
		: InventoryComponent->MoveItem(
			DragOperation->SourceInventorySlotIndex,
			SlotIndex
		);

	if (!bMoved)
	{
		return false;
	}

	RefreshSlot();
	return true;
}
