#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LEquipmentWeaponSlotWidget.generated.h"

class UBorder;
class UDragDropOperation;
class UImage;
class UTextBlock;
class ULInventoryComponent;
class ULDraggableWindowWidget;

UCLASS()
class PROJECTLAR_API ULEquipmentWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetupWeaponSlot(ULInventoryComponent* InInventoryComponent);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void RefreshWeaponSlot();

	void SetOwningWindow(ULDraggableWindowWidget* InOwningWindow);

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
	TObjectPtr<UImage> IMG_WeaponIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_WeaponName;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_EnhancementLevel;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_AttackPower;

private:
	void ResolveDesignerWidgets();
	void BuildDefaultWidgetTree();
	bool CanAcceptInventorySlot(int32 InventorySlotIndex) const;

	UPROPERTY()
	TObjectPtr<ULInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<ULDraggableWindowWidget> OwningWindow;
};
