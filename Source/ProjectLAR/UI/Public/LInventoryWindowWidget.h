#pragma once

#include "CoreMinimal.h"
#include "ProjectLAR/UI/Public/LDraggableWindowWidget.h"
#include "LInventoryWindowWidget.generated.h"

class UTextBlock;
class UUniformGridPanel;
class UDragDropOperation;
class ULInventoryComponent;
class ULInventorySlotWidget;
class ALPlayerCharacter;

UCLASS()
class PROJECTLAR_API ULInventoryWindowWidget : public ULDraggableWindowWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetObservedPlayer(ALPlayerCharacter* InPlayerCharacter);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RebuildInventoryGrid();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshInventory();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual bool NativeOnDrop(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation
	) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<ULInventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Layout", meta = (ClampMin = "1", ClampMax = "20", UIMin = "1", UIMax = "20"))
	int32 Columns = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Layout", meta = (ClampMin = "24.0", ClampMax = "80.0", UIMin = "24.0", UIMax = "80.0"))
	float SlotSize = 44.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Layout", meta = (ClampMin = "0.0", ClampMax = "8.0", UIMin = "0.0", UIMax = "8.0"))
	float SlotSpacing = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Layout")
	bool bAutoSizeGridToSlots = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Slot Visual")
	bool bCreateRuntimeSlotBackground = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Slot Visual", meta = (EditCondition = "bCreateRuntimeSlotBackground"))
	FLinearColor RuntimeSlotBackgroundColor = FLinearColor(0.035f, 0.04f, 0.04f, 0.95f);

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UUniformGridPanel> GRID_InventorySlots;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_Gold;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_Title;

private:
	ULInventoryComponent* ResolveInventoryComponent() const;
	int32 GetSafeColumnCount() const;
	float GetSafeSlotSize() const;
	float GetSafeSlotContentSize() const;
	bool GetSlotIndexAtScreenPosition(
		const FVector2D& ScreenPosition,
		int32& OutSlotIndex
	) const;

private:
	UPROPERTY()
	TObjectPtr<ALPlayerCharacter> ObservedPlayerCharacter;

	UPROPERTY()
	TArray<TObjectPtr<ULInventorySlotWidget>> CreatedSlotWidgets;
};
