#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LInventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class UDragDropOperation;
class ULInventoryComponent;
class ULDraggableWindowWidget;

UCLASS()
class PROJECTLAR_API ULInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
	void SetupInventorySlot(ULInventoryComponent* InInventoryComponent, int32 InSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
	void RefreshSlot();

	void SetOwningWindow(ULDraggableWindowWidget* InOwningWindow);

	UFUNCTION(BlueprintPure, Category = "Inventory Slot")
	int32 GetSlotIndex() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

	virtual FReply NativeOnMouseButtonUp(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent,
		UDragDropOperation*& OutOperation
	) override;

	virtual bool NativeOnDrop(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation
	) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_ItemIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_ItemCount;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_EnhancementLevel;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_ItemName;

private:
	UPROPERTY()
	TObjectPtr<ULInventoryComponent> InventoryComponent;

	UPROPERTY()
	int32 SlotIndex = INDEX_NONE;

	UPROPERTY()
	TObjectPtr<ULDraggableWindowWidget> OwningWindow;
};
