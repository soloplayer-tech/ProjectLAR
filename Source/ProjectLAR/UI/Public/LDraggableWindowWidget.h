#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LDraggableWindowWidget.generated.h"

class UWidget;

UCLASS()
class PROJECTLAR_API ULDraggableWindowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Window")
	void BringToFront();

	virtual void SetVisibility(ESlateVisibility InVisibility) override;

	bool IsChangingWindowZOrder() const;

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnPreviewMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

	virtual FReply NativeOnMouseButtonUp(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

	virtual FReply NativeOnMouseMove(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

	virtual void NativeOnMouseCaptureLost(
		const FCaptureLostEvent& CaptureLostEvent
	) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Window")
	bool bBringToFrontOnMouseDown = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Window")
	bool bDragWindowOnMouseDown = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Window")
	bool bRequireDragHandleToMove = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Window")
	bool bRootSelfHitTestInvisible = true;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> WBP_DragHandle;

private:
	static int32 AllocateWindowZOrder();
	void ApplyWindowHitTestSettings();
	bool IsMouseOverDragHandle(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) const;

private:
	bool bIsDraggingWindow = false;
	bool bIsChangingWindowZOrder = false;
	FVector2D DragStartScreenPosition = FVector2D::ZeroVector;
	FVector2D DragStartRenderTranslation = FVector2D::ZeroVector;
};
