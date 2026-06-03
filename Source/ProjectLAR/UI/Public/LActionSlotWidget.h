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

	virtual bool NativeOnDrop(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation
	) override;

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
	
private:
	bool CanAcceptPayload(const ULUIDragDropOperation* DragOperation) const;

	bool TryConvertToPlayerSkillSlot(
		ELPlayerSkillSlot& OutSkillSlot
	) const;

	void UpdateSlotLabel();


	ALPlayerCharacter* GetPlayerCharacter() const;

	void RefreshSlotFromPlayer();

	void SetSlotIcon(UTexture2D* IconTexture);

	UTexture2D* GetSkillIconTexture(ELPlayerSkillID SkillID) const;

	void UpdateCooldownUI(ALPlayerCharacter* PlayerCharacter, ELPlayerSkillID SkillID);

protected:
	// 이 슬롯이 Q/W/E/R/A/S/D/F/V/1/2/3/4 중 무엇인지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Slot")
	ELActionSlotKey SlotKey = ELActionSlotKey::None;

	// 이 슬롯이 Skill / Ultimate / Item 중 무엇을 받을 수 있는지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Slot")
	ELActionSlotType SlotType = ELActionSlotType::Skill;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_SlotIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_SlotKey;

	// =========================
	// Cooldown UI
	// =========================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_CooldownRadial;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_Cooldown;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CooldownMaterial;

	// =========================
	// Skill Icon Textures
	// WBP_ActionSlot 클래스 기본값에서 넣어두면 됨
	// =========================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Slot|Skill Icon")
	TObjectPtr<UTexture2D> MeteorIconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Slot|Skill Icon")
	TObjectPtr<UTexture2D> IceLanceIconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Slot|Skill Icon")
	TObjectPtr<UTexture2D> ThunderIconTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action Slot|Skill Icon")
	TObjectPtr<UTexture2D> WindIconTexture;

private:
	ELPlayerSkillID CachedSkillID = ELPlayerSkillID::None;
};