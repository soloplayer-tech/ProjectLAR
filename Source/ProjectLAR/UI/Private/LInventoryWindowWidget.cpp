#include "ProjectLAR/UI/Public/LInventoryWindowWidget.h"

#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectLAR/Item/Public/LInventoryComponent.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"
#include "ProjectLAR/UI/Public/LUIDragDropOperation.h"
#include "ProjectLAR/UI/Public/LInventorySlotWidget.h"
#include "Styling/SlateBrush.h"

void ULInventoryWindowWidget::SetObservedPlayer(
	ALPlayerCharacter* InPlayerCharacter
)
{
	ObservedPlayerCharacter = InPlayerCharacter;
	RebuildInventoryGrid();
}

void ULInventoryWindowWidget::RebuildInventoryGrid()
{
	if (!GRID_InventorySlots || !InventorySlotWidgetClass)
	{
		return;
	}

	ULInventoryComponent* InventoryComponent =
		ResolveInventoryComponent();

	if (!InventoryComponent)
	{
		return;
	}

	InventoryComponent->InitializeInventorySlots();

	GRID_InventorySlots->ClearChildren();
	CreatedSlotWidgets.Empty();

	const TArray<FLInventorySlot> Slots =
		InventoryComponent->GetSlots();

	const int32 SafeColumns = GetSafeColumnCount();
	const int32 SafeRows = FMath::Max(
		1,
		FMath::CeilToInt(
			static_cast<float>(Slots.Num()) / static_cast<float>(SafeColumns)
		)
	);
	const float SafeSlotSize = GetSafeSlotSize();
	const float SafeSlotContentSize = GetSafeSlotContentSize();

	GRID_InventorySlots->SetMinDesiredSlotWidth(SafeSlotSize);
	GRID_InventorySlots->SetMinDesiredSlotHeight(SafeSlotSize);

	if (bAutoSizeGridToSlots)
	{
		if (UCanvasPanelSlot* CanvasSlot =
			Cast<UCanvasPanelSlot>(GRID_InventorySlots->Slot))
		{
			CanvasSlot->SetAutoSize(true);
			CanvasSlot->SetSize(
				FVector2D(
					SafeSlotSize * static_cast<float>(SafeColumns),
					SafeSlotSize * static_cast<float>(SafeRows)
				)
			);
		}
	}

	for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); ++SlotIndex)
	{
		ULInventorySlotWidget* SlotWidget =
			CreateWidget<ULInventorySlotWidget>(
				GetOwningPlayer(),
				InventorySlotWidgetClass
			);

		if (!SlotWidget)
		{
			continue;
		}

		SlotWidget->SetupInventorySlot(
			InventoryComponent,
			SlotIndex
		);
		SlotWidget->SetOwningWindow(this);

		USizeBox* SlotSizeBox = NewObject<USizeBox>(this);

		if (!SlotSizeBox)
		{
			continue;
		}

		SlotSizeBox->SetWidthOverride(SafeSlotContentSize);
		SlotSizeBox->SetHeightOverride(SafeSlotContentSize);

		UOverlay* CellOverlay = NewObject<UOverlay>(this);

		if (!CellOverlay)
		{
			continue;
		}

		if (bCreateRuntimeSlotBackground)
		{
			UBorder* CellBackground = NewObject<UBorder>(this);

			if (CellBackground)
			{
				FSlateBrush BackgroundBrush;
				BackgroundBrush.DrawAs = ESlateBrushDrawType::Box;
				BackgroundBrush.TintColor = FSlateColor(RuntimeSlotBackgroundColor);
				BackgroundBrush.ImageSize = FVector2D(
					SafeSlotContentSize,
					SafeSlotContentSize
				);

				CellBackground->SetBrush(BackgroundBrush);
				CellBackground->SetBrushColor(RuntimeSlotBackgroundColor);

				if (UOverlaySlot* BackgroundSlot =
					CellOverlay->AddChildToOverlay(CellBackground))
				{
					BackgroundSlot->SetHorizontalAlignment(HAlign_Fill);
					BackgroundSlot->SetVerticalAlignment(VAlign_Fill);
				}
			}
		}

		if (UOverlaySlot* ContentSlot =
			CellOverlay->AddChildToOverlay(SlotWidget))
		{
			ContentSlot->SetHorizontalAlignment(HAlign_Fill);
			ContentSlot->SetVerticalAlignment(VAlign_Fill);
		}

		SlotSizeBox->AddChild(CellOverlay);

		const int32 Row = SlotIndex / SafeColumns;
		const int32 Column = SlotIndex % SafeColumns;

		UUniformGridSlot* GridSlot = GRID_InventorySlots->AddChildToUniformGrid(
			SlotSizeBox,
			Row,
			Column
		);

		if (GridSlot)
		{
			GridSlot->SetHorizontalAlignment(HAlign_Left);
			GridSlot->SetVerticalAlignment(VAlign_Top);
		}

		CreatedSlotWidgets.Add(SlotWidget);
	}

	RefreshInventory();
}

void ULInventoryWindowWidget::RefreshInventory()
{
	ULInventoryComponent* InventoryComponent =
		ResolveInventoryComponent();

	if (!InventoryComponent)
	{
		return;
	}

	InventoryComponent->InitializeInventorySlots();

	if (CreatedSlotWidgets.Num() != InventoryComponent->GetSlots().Num())
	{
		RebuildInventoryGrid();
		return;
	}

	if (TXT_Title)
	{
		TXT_Title->SetText(FText::FromString(TEXT("소지품")));
	}

	if (TXT_Gold)
	{
		TXT_Gold->SetText(FText::AsNumber(InventoryComponent->GetGold()));
	}

	for (ULInventorySlotWidget* SlotWidget : CreatedSlotWidgets)
	{
		if (SlotWidget)
		{
			SlotWidget->RefreshSlot();
		}
	}
}

void ULInventoryWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!ObservedPlayerCharacter)
	{
		ObservedPlayerCharacter = Cast<ALPlayerCharacter>(
			UGameplayStatics::GetPlayerCharacter(this, 0)
		);
	}

	RebuildInventoryGrid();
}

void ULInventoryWindowWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshInventory();
}

bool ULInventoryWindowWidget::NativeOnDrop(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation
)
{
	ULUIDragDropOperation* DragOperation =
		Cast<ULUIDragDropOperation>(InOperation);

	ULInventoryComponent* InventoryComponent =
		ResolveInventoryComponent();

	if (!DragOperation || !InventoryComponent)
	{
		return false;
	}

	if (DragOperation->PayloadType != ELDragPayloadType::Item)
	{
		return false;
	}

	int32 TargetSlotIndex = INDEX_NONE;

	if (!GetSlotIndexAtScreenPosition(
		InDragDropEvent.GetScreenSpacePosition(),
		TargetSlotIndex
	))
	{
		return false;
	}

	const bool bMoved = DragOperation->bFromEquipmentSlot
		? InventoryComponent->UnequipWeaponToSlot(TargetSlotIndex)
		: InventoryComponent->MoveItem(
			DragOperation->SourceInventorySlotIndex,
			TargetSlotIndex
		);

	if (bMoved)
	{
		RefreshInventory();
	}

	return bMoved;
}

ULInventoryComponent* ULInventoryWindowWidget::ResolveInventoryComponent() const
{
	if (!ObservedPlayerCharacter)
	{
		return nullptr;
	}

	return ObservedPlayerCharacter->GetInventoryComponent();
}

int32 ULInventoryWindowWidget::GetSafeColumnCount() const
{
	return FMath::Clamp(Columns, 1, 20);
}

float ULInventoryWindowWidget::GetSafeSlotSize() const
{
	return FMath::Clamp(SlotSize, 24.0f, 80.0f);
}

float ULInventoryWindowWidget::GetSafeSlotContentSize() const
{
	const float SafeSlotSize = GetSafeSlotSize();
	const float SafeSlotSpacing = FMath::Clamp(
		SlotSpacing,
		0.0f,
		FMath::Min(8.0f, SafeSlotSize - 1.0f)
	);

	return FMath::Max(1.0f, SafeSlotSize - SafeSlotSpacing);
}

bool ULInventoryWindowWidget::GetSlotIndexAtScreenPosition(
	const FVector2D& ScreenPosition,
	int32& OutSlotIndex
) const
{
	OutSlotIndex = INDEX_NONE;

	if (!GRID_InventorySlots)
	{
		return false;
	}

	ULInventoryComponent* InventoryComponent =
		ResolveInventoryComponent();

	if (!InventoryComponent)
	{
		return false;
	}

	InventoryComponent->InitializeInventorySlots();

	const TArray<FLInventorySlot> Slots =
		InventoryComponent->GetSlots();

	if (Slots.IsEmpty())
	{
		return false;
	}

	const int32 SafeColumns = GetSafeColumnCount();
	const int32 Rows = FMath::CeilToInt(
		static_cast<float>(Slots.Num()) / static_cast<float>(SafeColumns)
	);

	if (Rows <= 0)
	{
		return false;
	}

	const FGeometry GridGeometry =
		GRID_InventorySlots->GetCachedGeometry();
	const FVector2D LocalPosition =
		GridGeometry.AbsoluteToLocal(ScreenPosition);
	const float CellWidth = GetSafeSlotSize();
	const float CellHeight = GetSafeSlotSize();
	const FVector2D ExpectedGridSize(
		CellWidth * static_cast<float>(SafeColumns),
		CellHeight * static_cast<float>(Rows)
	);

	if (CellWidth <= 0.0f || CellHeight <= 0.0f)
	{
		return false;
	}

	if (LocalPosition.X < 0.0f
		|| LocalPosition.Y < 0.0f
		|| LocalPosition.X >= ExpectedGridSize.X
		|| LocalPosition.Y >= ExpectedGridSize.Y)
	{
		return false;
	}

	const int32 Column = FMath::Clamp(
		FMath::FloorToInt(LocalPosition.X / CellWidth),
		0,
		SafeColumns - 1
	);
	const int32 Row = FMath::Clamp(
		FMath::FloorToInt(LocalPosition.Y / CellHeight),
		0,
		Rows - 1
	);
	const int32 SlotIndex = Row * SafeColumns + Column;

	if (!Slots.IsValidIndex(SlotIndex))
	{
		return false;
	}

	OutSlotIndex = SlotIndex;
	return true;
}
