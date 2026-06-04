#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LPlayerUIWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UMaterialInstanceDynamic;
class ALPlayerCharacter;
class ALPlayerCharacterBase;

UCLASS()
class PROJECTLAR_API ULPlayerUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetObservedCharacter(ALPlayerCharacterBase* InCharacter);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime
	) override;

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
	// Cast Bar UI
	// =========================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_CastProgress;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_CastName;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_CastRemaining;

	// =========================
	// Identity Screen Overlay
	// =========================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_IdentityFlameOverlay;

private:
	UPROPERTY()
	TObjectPtr<ALPlayerCharacterBase> ObservedCharacter;

private:
	UMaterialInstanceDynamic* InitCooldownImage(UImage* CooldownImage);
	void InitCooldownText(UTextBlock* CooldownText);

	void UpdateCooldownUI(
		UImage* CooldownImage,
		UMaterialInstanceDynamic* CooldownMaterial,
		UTextBlock* CooldownText,
		bool bOnCooldown,
		float Remaining,
		float Ratio
	);

	void UpdateIdentityActiveUI(ALPlayerCharacter* PlayerCharacter);
	void UpdateCastBarUI();
};