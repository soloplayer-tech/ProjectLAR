#include "ProjectLAR/UI/Public/LBossTableWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void ULBossTableWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BTN_EnterBoss)
	{
		BTN_EnterBoss->OnClicked.AddDynamic(
			this,
			&ULBossTableWidget::HandleEnterBossClicked
		);
	}

	if (BTN_Close)
	{
		BTN_Close->OnClicked.AddDynamic(
			this,
			&ULBossTableWidget::HandleCloseClicked
		);
	}
}

void ULBossTableWidget::NativeDestruct()
{
	RestoreGameplayInputMode();

	Super::NativeDestruct();
}

void ULBossTableWidget::SetTargetLevelName(FName InTargetLevelName)
{
	TargetLevelName = InTargetLevelName;
}

void ULBossTableWidget::OpenTargetLevel()
{
	if (TargetLevelName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenTargetLevel Failed: TargetLevelName is None"));
		return;
	}

	UGameplayStatics::OpenLevel(this, TargetLevelName);
}

void ULBossTableWidget::CloseBossTable()
{
	RemoveFromParent();
}

void ULBossTableWidget::HandleEnterBossClicked()
{
	OpenTargetLevel();
}

void ULBossTableWidget::HandleCloseClicked()
{
	CloseBossTable();
}

void ULBossTableWidget::RestoreGameplayInputMode() const
{
	APlayerController* OwningPlayerController = GetOwningPlayer();
	if (!OwningPlayerController)
	{
		return;
	}

	OwningPlayerController->StopMovement();
	OwningPlayerController->bShowMouseCursor = true;

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(
		OwningPlayerController,
		nullptr,
		EMouseLockMode::DoNotLock,
		false,
		true
	);
}
