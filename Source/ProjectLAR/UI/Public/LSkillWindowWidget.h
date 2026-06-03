#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LPlayerSkillID.h"
#include "LSkillWindowWidget.generated.h"

class ALPlayerCharacter;
class ULSkillIconWidget;
class UTexture2D;

UCLASS()
class PROJECTLAR_API ULSkillWindowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetOwningPlayerCharacter(ALPlayerCharacter* InPlayerCharacter);

protected:
	virtual void NativeConstruct() override;

private:
	void InitializeSkillIcons();

private:
	UPROPERTY()
	TObjectPtr<ALPlayerCharacter> OwningPlayerCharacter;

	// =========================
	// Skill Icons
	// WBP_SkillWindow 안에 배치할 아이콘 위젯들
	// =========================

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<ULSkillIconWidget> WBP_MeteorSkillIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<ULSkillIconWidget> WBP_IceLanceSkillIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<ULSkillIconWidget> WBP_ThunderSkillIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<ULSkillIconWidget> WBP_WindSkillIcon;

	// =========================
	// Icon Textures
	// BP에서 직접 꽂아줄 이미지
	// =========================

	UPROPERTY(EditDefaultsOnly, Category = "Skill Window|Icon")
	TObjectPtr<UTexture2D> MeteorIconTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Skill Window|Icon")
	TObjectPtr<UTexture2D> IceLanceIconTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Skill Window|Icon")
	TObjectPtr<UTexture2D> ThunderIconTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Skill Window|Icon")
	TObjectPtr<UTexture2D> WindIconTexture;
};