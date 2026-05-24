#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LPlayerUIWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class ALPlayerCharacterBase;

UCLASS()
class PROJECTLAR_API ULPlayerUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetObservedCharacter(ALPlayerCharacterBase* InCharacter);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_PlayerHP;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_PlayerMP;

	// 대쉬 쿨타임 표시용. WBP에 직접 추가할 예정.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> PB_DashCooldown;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_DashCooldown;

private:
	UPROPERTY()
	TObjectPtr<ALPlayerCharacterBase> ObservedCharacter;
};