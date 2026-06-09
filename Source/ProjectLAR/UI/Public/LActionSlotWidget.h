#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectLAR/UI/Public/LUIDragDropTypes.h"
#include "LPlayerSkillID.h"
#include "ProjectLAR/Player/Public/LPlayerSkillSlot.h"
#include "LActionSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;
class UDragDropOperation;
class UMaterialInstanceDynamic;
class ULUIDragDropOperation;
class ALPlayerCharacter;

UCLASS()
class PROJECTLAR_API ULActionSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	ELActionSlotKey GetSlotKey() const;
	ELActionSlotType GetSlotType() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent,
		UDragDropOperation*& OutOperation
	) override;

	virtual void NativeOnDragCancelled(
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation
	) override;

	virtual bool NativeOnDrop(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation
	) override;

private:
	bool CanAcceptPayload(const ULUIDragDropOperation* DragOperation) const;

	bool TryConvertToPlayerSkillSlot(
		ELPlayerSkillSlot& OutSkillSlot
	) const;

	ALPlayerCharacter* GetPlayerCharacter() const;

	void RefreshSlotFromPlayer();
	void UpdateSlotLabel();

	void SetSlotIcon(UTexture2D* IconTexture);
	UTexture2D* GetSkillIconTexture(ELPlayerSkillID SkillID) const;

	void InitCooldownUI();
	void UpdateCooldownUI(
		ALPlayerCharacter* PlayerCharacter,
		ELPlayerSkillID SkillID
	);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Slot", meta = (ExposeOnSpawn = true))
	ELActionSlotKey SlotKey = ELActionSlotKey::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Slot", meta = (ExposeOnSpawn = true))
	ELActionSlotType SlotType = ELActionSlotType::Skill;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_SlotIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_SlotKey;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_CooldownRadial;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_Cooldown;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CooldownMaterial;

	// 임시 방식.
	// 나중에 DataTable/DataAsset으로 옮기기 전까지 사용.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Slot|Skill Icon")
	TObjectPtr<UTexture2D> MeteorIconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Slot|Skill Icon")
	TObjectPtr<UTexture2D> IceLanceIconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Slot|Skill Icon")
	TObjectPtr<UTexture2D> ThunderIconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Slot|Skill Icon")
	TObjectPtr<UTexture2D> WindIconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Slot|Skill Icon")
	TObjectPtr<UTexture2D> FrostFieldIconTexture;

private:
	ELPlayerSkillID CachedSkillID = ELPlayerSkillID::None;
};
