#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LPlayerSkillID.h"
#include "LSkillIconWidget.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;

UCLASS()
class PROJECTLAR_API ULSkillIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSkillData(
		ELPlayerSkillID InSkillID,
		const FText& InSkillName,
		UTexture2D* InSkillIconTexture
	);

	ELPlayerSkillID GetSkillID() const;

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent,
		UDragDropOperation*& OutOperation
	) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_SkillIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_SkillName;

private:
	UPROPERTY()
	ELPlayerSkillID SkillID = ELPlayerSkillID::None;

	UPROPERTY()
	TObjectPtr<UTexture2D> SkillIconTexture;

	FText SkillName;
};