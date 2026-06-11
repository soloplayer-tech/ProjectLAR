#pragma once

#include "CoreMinimal.h"
#include "ProjectLAR/Item/Public/LInventoryComponent.h"
#include "ProjectLAR/UI/Public/LDraggableWindowWidget.h"
#include "LWeaponEnhanceWindowWidget.generated.h"

class ALPlayerCharacter;
class UButton;
class UImage;
class UTextBlock;
class UDragDropOperation;
class ULItemDataAsset;

UCLASS()
class PROJECTLAR_API ULWeaponEnhanceWindowWidget : public ULDraggableWindowWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon Enhance")
	void SetObservedPlayer(ALPlayerCharacter* InObservedPlayerCharacter);

	UFUNCTION(BlueprintCallable, Category = "Weapon Enhance")
	bool TrySetTargetFromInventorySlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Weapon Enhance")
	bool TrySetTargetEquippedWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon Enhance")
	void ClearEnhanceTarget();

	UFUNCTION(BlueprintCallable, Category = "Weapon Enhance")
	void RefreshEnhanceWindow();

	UFUNCTION(BlueprintCallable, Category = "Weapon Enhance")
	void CloseWindow();

	static ULWeaponEnhanceWindowWidget* GetActiveWeaponEnhanceWindow();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual FReply NativeOnKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;

	virtual FReply NativeOnPreviewKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;

	virtual bool NativeOnDrop(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation
	) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_TargetIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_Title;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_TargetName;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_CurrentLevel;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_AttackPower;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_GoldCost;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_MaterialCost;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_SuccessChance;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_Status;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_Result;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_EnhanceButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_Enhance;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_Clear;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_Close;

private:
	UFUNCTION()
	void HandleEnhanceClicked();

	UFUNCTION()
	void HandleClearClicked();

	UFUNCTION()
	void HandleCloseClicked();

	void ResolveDesignerWidgets();
	void BindButtons();
	void RestoreGameplayInputMode() const;
	ULInventoryComponent* ResolveInventoryComponent() const;
	bool GetCurrentTargetSlot(FLInventorySlot& OutSlot) const;
	FText GetMaterialDisplayText(FName MaterialItemID) const;
	FText BuildResultText(const FLWeaponEnhanceResult& Result) const;
	void SetTargetIcon(UTexture2D* IconTexture);
	void SetEmptyText();

private:
	static TWeakObjectPtr<ULWeaponEnhanceWindowWidget> ActiveWeaponEnhanceWindow;

	UPROPERTY()
	TObjectPtr<ALPlayerCharacter> ObservedPlayerCharacter;

	int32 TargetInventorySlotIndex = INDEX_NONE;
	bool bTargetEquippedWeapon = false;
	FText LastResultText;
};
