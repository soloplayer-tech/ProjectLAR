#include "ProjectLAR/UI/Public/LDraggableWindowWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "InputCoreTypes.h"

namespace
{
	int32 GNextWindowZOrder = 200;
}

void ULDraggableWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ApplyWindowHitTestSettings();
}

void ULDraggableWindowWidget::SetVisibility(
	ESlateVisibility InVisibility
)
{
	if (bRootSelfHitTestInvisible
		&& InVisibility == ESlateVisibility::Visible)
	{
		Super::SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ApplyWindowHitTestSettings();
		return;
	}

	Super::SetVisibility(InVisibility);
}

void ULDraggableWindowWidget::ApplyWindowHitTestSettings()
{
	if (!bRootSelfHitTestInvisible || !WidgetTree)
	{
		return;
	}

	Super::SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	WidgetTree->ForEachWidget(
		[this](UWidget* Widget)
		{
			if (!Widget || Widget == WBP_DragHandle)
			{
				return;
			}

			if (Cast<UUserWidget>(Widget))
			{
				return;
			}

			if (Cast<UButton>(Widget))
			{
				return;
			}

			const bool bShouldIgnoreSelfHitTest =
				Cast<UPanelWidget>(Widget)
				|| Cast<UImage>(Widget)
				|| Cast<UBorder>(Widget)
				|| Cast<USizeBox>(Widget)
				|| Cast<UTextBlock>(Widget);

			if (bShouldIgnoreSelfHitTest)
			{
				Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
	);

	// Keep the designer's drag-handle hit-test setting intact. Some windows
	// intentionally make the handle non-hit-testable so it will not cover
	// buttons or item slots placed on top of the same visual layer.
}

void ULDraggableWindowWidget::BringToFront()
{
	const ESlateVisibility PreviousVisibility = GetVisibility();
	const int32 NewZOrder = AllocateWindowZOrder();

	bIsChangingWindowZOrder = true;
	RemoveFromParent();
	AddToViewport(NewZOrder);
	bIsChangingWindowZOrder = false;

	SetVisibility(PreviousVisibility);
	ApplyWindowHitTestSettings();
}

bool ULDraggableWindowWidget::IsChangingWindowZOrder() const
{
	return bIsChangingWindowZOrder;
}

FReply ULDraggableWindowWidget::NativeOnPreviewMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

FReply ULDraggableWindowWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	if (!bDragWindowOnMouseDown
		|| InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	if (!IsMouseOverDragHandle(InGeometry, InMouseEvent))
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	bIsDraggingWindow = true;
	DragStartScreenPosition = InMouseEvent.GetScreenSpacePosition();
	DragStartRenderTranslation = GetRenderTransform().Translation;

	return FReply::Handled().CaptureMouse(TakeWidget());
}

FReply ULDraggableWindowWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	if (bIsDraggingWindow
		&& InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDraggingWindow = false;

		const FReply Reply =
			FReply::Handled().ReleaseMouseCapture();

		if (bBringToFrontOnMouseDown)
		{
			BringToFront();
		}

		return Reply;
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply ULDraggableWindowWidget::NativeOnMouseMove(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	if (!bIsDraggingWindow)
	{
		return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	}

	if (!InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		bIsDraggingWindow = false;
		return FReply::Handled().ReleaseMouseCapture();
	}

	const FVector2D DragDelta =
		InMouseEvent.GetScreenSpacePosition() - DragStartScreenPosition;

	SetRenderTranslation(DragStartRenderTranslation + DragDelta);

	return FReply::Handled();
}

void ULDraggableWindowWidget::NativeOnMouseCaptureLost(
	const FCaptureLostEvent& CaptureLostEvent
)
{
	bIsDraggingWindow = false;

	Super::NativeOnMouseCaptureLost(CaptureLostEvent);
}

int32 ULDraggableWindowWidget::AllocateWindowZOrder()
{
	return ++GNextWindowZOrder;
}

bool ULDraggableWindowWidget::IsMouseOverDragHandle(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
) const
{
	if (!WBP_DragHandle)
	{
		return !bRequireDragHandleToMove;
	}

	const FGeometry HandleGeometry =
		WBP_DragHandle->GetCachedGeometry();
	const FVector2D ScreenPosition =
		InMouseEvent.GetScreenSpacePosition();

	return HandleGeometry.IsUnderLocation(ScreenPosition);
}
