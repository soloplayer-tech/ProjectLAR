#pragma once

#include "CoreMinimal.h"
#include "LPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "ProjectLAR/Player/Public/LPlayerSkillSlot.h"
#include "LPlayerUIWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UMaterialInstanceDynamic;
class ALPlayerCharacterBase;

UCLASS()
class PROJECTLAR_API ULPlayerUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetObservedCharacter(ALPlayerCharacterBase* InCharacter);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_PlayerHP;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_PlayerMP;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_IdentityGauge;

	// =========================
	// Dash Cooldown UI
	// =========================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_DashIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_DashCooldownRadial;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_DashCooldown;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DashCooldownMaterial;

	// =========================
	// Q Skill Cooldown UI
	// =========================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_QCooldownRadial;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_QCooldown;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> QCooldownMaterial;

	// =========================
	// W Skill Cooldown UI
	// =========================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_WCooldownRadial;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_WCooldown;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> WCooldownMaterial;

	// =========================
	// E Skill Cooldown UI
	// =========================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_ECooldownRadial;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_ECooldown;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ECooldownMaterial;

	// =========================
	// R Skill Cooldown UI
	// =========================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_RCooldownRadial;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_RCooldown;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> RCooldownMaterial;
	
	// =========================
	// Cast Bar UI
	// =========================

	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_CastProgress;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_CastName;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_CastRemaining;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_IdentityFlameOverlay;

private:
	UPROPERTY()
	TObjectPtr<ALPlayerCharacterBase> ObservedCharacter;
	
	UMaterialInstanceDynamic* InitCooldownImage(UImage* CooldownImage);

	void UpdateIdentityActiveUI(ALPlayerCharacter* PlayerCharacter);
	
	void InitCooldownText(UTextBlock* CooldownText);

	void UpdateCooldownUI(
		UImage* CooldownImage,
		UMaterialInstanceDynamic* CooldownMaterial,
		UTextBlock* CooldownText,
		bool bOnCooldown,
		float Remaining,
		float Ratio
	);

	void UpdateSkillSlotCooldownUI(
		ELPlayerSkillSlot SkillSlot,
		UImage* CooldownImage,
		UMaterialInstanceDynamic* CooldownMaterial,
		UTextBlock* CooldownText
	);
	
	void UpdateCastBarUI();
};