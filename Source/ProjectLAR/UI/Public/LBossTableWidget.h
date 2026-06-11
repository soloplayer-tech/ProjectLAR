#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LBossTableWidget.generated.h"

class UButton;

UCLASS()
class PROJECTLAR_API ULBossTableWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Boss Table")
	void SetTargetLevelName(FName InTargetLevelName);

	UFUNCTION(BlueprintCallable, Category = "Boss Table")
	void OpenTargetLevel();

	UFUNCTION(BlueprintCallable, Category = "Boss Table")
	void CloseBossTable();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss Table")
	FName TargetLevelName = TEXT("OB_TestMap");

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_EnterBoss;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_Close;

private:
	UFUNCTION()
	void HandleEnterBossClicked();

	UFUNCTION()
	void HandleCloseClicked();

	void RestoreGameplayInputMode() const;
};
