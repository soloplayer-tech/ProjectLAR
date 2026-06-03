#include "ProjectLAR/UI/Public/LSkillIconWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "InputCoreTypes.h"
#include "ProjectLAR/UI/Public/LUIDragDropOperation.h"

void ULSkillIconWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULSkillIconWidget::SetSkillData(
	ELPlayerSkillID InSkillID,
	const FText& InSkillName,
	UTexture2D* InSkillIconTexture
)
{
	SkillID = InSkillID;
	SkillName = InSkillName;
	SkillIconTexture = InSkillIconTexture;

	if (TXT_SkillName)
	{
		TXT_SkillName->SetText(SkillName);
	}

	if (IMG_SkillIcon && SkillIconTexture)
	{
		IMG_SkillIcon->SetBrushFromTexture(SkillIconTexture, true);
	}
}

ELPlayerSkillID ULSkillIconWidget::GetSkillID() const
{
	return SkillID;
}

FReply ULSkillIconWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Skill Icon Mouse Down: %s"),
			*UEnum::GetValueAsString(SkillID)
		);

		return UWidgetBlueprintLibrary::DetectDragIfPressed(
			InMouseEvent,
			this,
			EKeys::LeftMouseButton
		).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void ULSkillIconWidget::NativeOnDragDetected(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation
)
{
	Super::NativeOnDragDetected(
		InGeometry,
		InMouseEvent,
		OutOperation
	);

	ULUIDragDropOperation* DragOperation =
		NewObject<ULUIDragDropOperation>();

	if (!DragOperation)
	{
		return;
	}

	DragOperation->PayloadType = ELDragPayloadType::Skill;
	DragOperation->SkillID = SkillID;
	DragOperation->IconTexture = SkillIconTexture;
	DragOperation->DisplayName = SkillName;

	DragOperation->DefaultDragVisual = this;
	DragOperation->Pivot = EDragPivot::MouseDown;

	OutOperation = DragOperation;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Skill Drag Started: PayloadType=Skill / Skill=%s"),
		*UEnum::GetValueAsString(SkillID)
	);
}