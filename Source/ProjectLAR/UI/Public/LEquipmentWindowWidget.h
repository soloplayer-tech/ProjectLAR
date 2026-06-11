#pragma once

#include "CoreMinimal.h"
#include "ProjectLAR/UI/Public/LDraggableWindowWidget.h"
#include "LEquipmentWindowWidget.generated.h"

class ALEquipmentPreviewActor;
class ALPlayerCharacter;
class UImage;
class ULEquipmentWeaponSlotWidget;
class UTextBlock;
class UTextureRenderTarget2D;

UCLASS()
class PROJECTLAR_API ULEquipmentWindowWidget : public ULDraggableWindowWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetObservedPlayer(ALPlayerCharacter* InObservedPlayerCharacter);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void RefreshEquipmentWindow();

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment|Layout")
	bool bUseCodeGeneratedLayout = true;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<ULEquipmentWeaponSlotWidget> WBP_WeaponSlot;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_CharacterPreview;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_Title;

private:
	void ResolveDesignerWidgets();
	void BuildDefaultWidgetTree();
	void EnsurePreviewActor();
	void DestroyPreviewActor();
	void SetupPreviewBrush();

	UPROPERTY()
	TObjectPtr<ALPlayerCharacter> ObservedPlayerCharacter;

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> PreviewRenderTarget;

	UPROPERTY()
	TObjectPtr<ALEquipmentPreviewActor> PreviewActor;
};
