#include "ProjectLAR/UI/Public/LEquipmentWeaponSlotWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InputCoreTypes.h"
#include "ProjectLAR/Item/Public/LInventoryComponent.h"
#include "ProjectLAR/UI/Public/LDraggableWindowWidget.h"
#include "ProjectLAR/UI/Public/LUIDragDropOperation.h"

void ULEquipmentWeaponSlotWidget::SetupWeaponSlot(
	ULInventoryComponent* InInventoryComponent
)
{
	InventoryComponent = InInventoryComponent;
	RefreshWeaponSlot();
}

void ULEquipmentWeaponSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResolveDesignerWidgets();
	BuildDefaultWidgetTree();
	ResolveDesignerWidgets();
	SetVisibility(ESlateVisibility::Visible);
	RefreshWeaponSlot();
}

void ULEquipmentWeaponSlotWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshWeaponSlot();
}

void ULEquipmentWeaponSlotWidget::SetOwningWindow(
	ULDraggableWindowWidget* InOwningWindow
)
{
	OwningWindow = InOwningWindow;
}

FReply ULEquipmentWeaponSlotWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	if (!InventoryComponent)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	FLInventorySlot EquippedWeaponSlot;

	if (!InventoryComponent->GetEquippedWeaponSlot(EquippedWeaponSlot)
		|| EquippedWeaponSlot.IsEmpty())
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (InventoryComponent->UnequipWeapon())
		{
			RefreshWeaponSlot();
			return FReply::Handled();
		}

		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	return UWidgetBlueprintLibrary::DetectDragIfPressed(
		InMouseEvent,
		this,
		EKeys::LeftMouseButton
	).NativeReply;
}

FReply ULEquipmentWeaponSlotWidget::NativeOnMouseButtonUp(
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

void ULEquipmentWeaponSlotWidget::NativeOnDragDetected(
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

	const int32 EquippedWeaponSlotIndex =
		InventoryComponent->GetEquippedWeaponSlotIndex();

	FLInventorySlot EquippedWeaponSlot;

	if (!InventoryComponent->GetEquippedWeaponSlot(EquippedWeaponSlot)
		|| EquippedWeaponSlot.IsEmpty())
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
	DragOperation->bFromEquipmentSlot = true;
	DragOperation->ItemData = EquippedWeaponSlot.ItemData;
	DragOperation->ItemCount = EquippedWeaponSlot.Quantity;
	DragOperation->SourceInventorySlotIndex = EquippedWeaponSlotIndex;
	DragOperation->IconTexture =
		EquippedWeaponSlot.ItemData
			? EquippedWeaponSlot.ItemData->IconTexture.Get()
			: nullptr;
	DragOperation->DisplayName =
		EquippedWeaponSlot.ItemData
			? EquippedWeaponSlot.ItemData->DisplayName
			: FText::GetEmpty();
	DragOperation->DefaultDragVisual = this;
	DragOperation->Pivot = EDragPivot::MouseDown;

	OutOperation = DragOperation;
}

bool ULEquipmentWeaponSlotWidget::NativeOnDrop(
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

	if (!CanAcceptInventorySlot(DragOperation->SourceInventorySlotIndex))
	{
		return false;
	}

	const bool bEquipped =
		InventoryComponent->EquipWeaponFromSlot(
			DragOperation->SourceInventorySlotIndex
		);

	if (bEquipped)
	{
		RefreshWeaponSlot();
	}

	return bEquipped;
}

void ULEquipmentWeaponSlotWidget::RefreshWeaponSlot()
{
	ResolveDesignerWidgets();

	FLInventorySlot EquippedWeaponSlot;
	const bool bHasWeapon =
		InventoryComponent
		&& InventoryComponent->GetEquippedWeaponSlot(EquippedWeaponSlot)
		&& EquippedWeaponSlot.ItemData;

	if (IMG_WeaponIcon)
	{
		if (bHasWeapon && EquippedWeaponSlot.ItemData->IconTexture)
		{
			IMG_WeaponIcon->SetBrushFromTexture(
				EquippedWeaponSlot.ItemData->IconTexture,
				false
			);
			IMG_WeaponIcon->SetRenderOpacity(1.0f);
		}
		else
		{
			IMG_WeaponIcon->SetRenderOpacity(0.0f);
		}
	}

	if (TXT_WeaponName)
	{
		TXT_WeaponName->SetText(
			bHasWeapon
				? EquippedWeaponSlot.ItemData->DisplayName
				: FText::FromString(TEXT("Weapon"))
		);
	}

	if (TXT_EnhancementLevel)
	{
		TXT_EnhancementLevel->SetText(
			bHasWeapon && EquippedWeaponSlot.EnhancementLevel > 0
				? FText::FromString(
					FString::Printf(TEXT("+%d"), EquippedWeaponSlot.EnhancementLevel)
				)
				: FText::GetEmpty()
		);
	}

	if (TXT_AttackPower)
	{
		TXT_AttackPower->SetText(
			bHasWeapon
				? FText::FromString(
					FString::Printf(
						TEXT("ATK %d"),
						EquippedWeaponSlot.ItemData->GetAttackPowerAtEnhancementLevel(
							EquippedWeaponSlot.EnhancementLevel
						)
					)
				)
				: FText::GetEmpty()
		);
	}
}

void ULEquipmentWeaponSlotWidget::ResolveDesignerWidgets()
{
	if (!IMG_WeaponIcon)
	{
		IMG_WeaponIcon =
			Cast<UImage>(
				GetWidgetFromName(TEXT("IMG_WeaponIcon"))
			);
	}

	if (!TXT_WeaponName)
	{
		TXT_WeaponName =
			Cast<UTextBlock>(
				GetWidgetFromName(TEXT("TXT_WeaponName"))
			);
	}

	if (!TXT_EnhancementLevel)
	{
		TXT_EnhancementLevel =
			Cast<UTextBlock>(
				GetWidgetFromName(TEXT("TXT_EnhancementLevel"))
			);
	}

	if (!TXT_AttackPower)
	{
		TXT_AttackPower =
			Cast<UTextBlock>(
				GetWidgetFromName(TEXT("TXT_AttackPower"))
			);
	}
}

void ULEquipmentWeaponSlotWidget::BuildDefaultWidgetTree()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	USizeBox* RootSizeBox =
		WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	RootSizeBox->SetWidthOverride(96.0f);
	RootSizeBox->SetHeightOverride(120.0f);
	WidgetTree->RootWidget = RootSizeBox;

	UBorder* SlotBorder =
		WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	SlotBorder->SetBrushColor(FLinearColor(0.015f, 0.02f, 0.025f, 0.96f));
	SlotBorder->SetPadding(FMargin(6.0f));
	RootSizeBox->AddChild(SlotBorder);

	UOverlay* SlotOverlay =
		WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
	SlotBorder->SetContent(SlotOverlay);

	IMG_WeaponIcon =
		WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	SlotOverlay->AddChildToOverlay(IMG_WeaponIcon);

	UVerticalBox* TextBox =
		WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	UOverlaySlot* TextOverlaySlot = SlotOverlay->AddChildToOverlay(TextBox);
	TextOverlaySlot->SetVerticalAlignment(VAlign_Bottom);
	TextOverlaySlot->SetHorizontalAlignment(HAlign_Fill);

	TXT_EnhancementLevel =
		WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	TXT_EnhancementLevel->SetJustification(ETextJustify::Right);
	TXT_EnhancementLevel->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.78f, 0.28f, 1.0f)));
	TextBox->AddChildToVerticalBox(TXT_EnhancementLevel);

	TXT_WeaponName =
		WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	TXT_WeaponName->SetJustification(ETextJustify::Center);
	TXT_WeaponName->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.94f, 1.0f, 1.0f)));
	TextBox->AddChildToVerticalBox(TXT_WeaponName);

	TXT_AttackPower =
		WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	TXT_AttackPower->SetJustification(ETextJustify::Center);
	TXT_AttackPower->SetColorAndOpacity(FSlateColor(FLinearColor(0.65f, 0.75f, 0.82f, 1.0f)));
	TextBox->AddChildToVerticalBox(TXT_AttackPower);
}

bool ULEquipmentWeaponSlotWidget::CanAcceptInventorySlot(
	int32 InventorySlotIndex
) const
{
	if (!InventoryComponent || !InventoryComponent->IsValidSlotIndex(InventorySlotIndex))
	{
		return false;
	}

	FLInventorySlot InventorySlot;
	return InventoryComponent->GetSlot(InventorySlotIndex, InventorySlot)
		&& !InventorySlot.IsEmpty()
		&& InventorySlot.ItemData
		&& InventorySlot.ItemData->IsWeapon();
}
