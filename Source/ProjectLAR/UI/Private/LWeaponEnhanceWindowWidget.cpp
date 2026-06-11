#include "ProjectLAR/UI/Public/LWeaponEnhanceWindowWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectLAR/Item/Public/LItemDataAsset.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"
#include "ProjectLAR/UI/Public/LUIDragDropOperation.h"

TWeakObjectPtr<ULWeaponEnhanceWindowWidget>
	ULWeaponEnhanceWindowWidget::ActiveWeaponEnhanceWindow;

void ULWeaponEnhanceWindowWidget::SetObservedPlayer(
	ALPlayerCharacter* InObservedPlayerCharacter
)
{
	ObservedPlayerCharacter = InObservedPlayerCharacter;
	RefreshEnhanceWindow();
}

bool ULWeaponEnhanceWindowWidget::TrySetTargetFromInventorySlot(
	int32 SlotIndex
)
{
	ULInventoryComponent* InventoryComponent =
		ResolveInventoryComponent();

	if (!InventoryComponent)
	{
		return false;
	}

	FLInventorySlot InventorySlot;

	if (!InventoryComponent->GetSlot(SlotIndex, InventorySlot)
		|| InventorySlot.IsEmpty()
		|| !InventorySlot.ItemData
		|| !InventorySlot.ItemData->IsWeapon())
	{
		return false;
	}

	TargetInventorySlotIndex = SlotIndex;
	bTargetEquippedWeapon = false;
	LastResultText = FText::GetEmpty();
	RefreshEnhanceWindow();
	BringToFront();
	return true;
}

bool ULWeaponEnhanceWindowWidget::TrySetTargetEquippedWeapon()
{
	ULInventoryComponent* InventoryComponent =
		ResolveInventoryComponent();

	if (!InventoryComponent)
	{
		return false;
	}

	FLInventorySlot EquippedWeaponSlot;

	if (!InventoryComponent->GetEquippedWeaponSlot(EquippedWeaponSlot))
	{
		return false;
	}

	TargetInventorySlotIndex = INDEX_NONE;
	bTargetEquippedWeapon = true;
	LastResultText = FText::GetEmpty();
	RefreshEnhanceWindow();
	BringToFront();
	return true;
}

void ULWeaponEnhanceWindowWidget::ClearEnhanceTarget()
{
	TargetInventorySlotIndex = INDEX_NONE;
	bTargetEquippedWeapon = false;
	LastResultText = FText::GetEmpty();
	RefreshEnhanceWindow();
}

void ULWeaponEnhanceWindowWidget::CloseWindow()
{
	if (ActiveWeaponEnhanceWindow.Get() == this)
	{
		ActiveWeaponEnhanceWindow.Reset();
	}

	SetVisibility(ESlateVisibility::Collapsed);

	TArray<UUserWidget*> EnhanceWindows;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
		this,
		EnhanceWindows,
		ULWeaponEnhanceWindowWidget::StaticClass(),
		false
	);

	for (UUserWidget* EnhanceWindow : EnhanceWindows)
	{
		if (!EnhanceWindow)
		{
			continue;
		}

		EnhanceWindow->SetVisibility(ESlateVisibility::Collapsed);
		EnhanceWindow->RemoveFromParent();
	}

	RemoveFromParent();
	RestoreGameplayInputMode();
}

void ULWeaponEnhanceWindowWidget::RefreshEnhanceWindow()
{
	ResolveDesignerWidgets();

	ULInventoryComponent* InventoryComponent =
		ResolveInventoryComponent();

	if (!InventoryComponent)
	{
		SetEmptyText();
		if (TXT_Status)
		{
			TXT_Status->SetText(FText::FromString(TEXT("No inventory.")));
		}
		return;
	}

	FLInventorySlot TargetSlot;

	if (!GetCurrentTargetSlot(TargetSlot))
	{
		SetEmptyText();
		return;
	}

	ULItemDataAsset* WeaponData = TargetSlot.ItemData;

	if (!WeaponData)
	{
		SetEmptyText();
		return;
	}

	SetTargetIcon(WeaponData->IconTexture);

	const int32 CurrentLevel = TargetSlot.EnhancementLevel;
	const int32 NextLevel = FMath::Min(
		CurrentLevel + 1,
		WeaponData->MaxEnhancementLevel
	);
	const int32 CurrentAttackPower =
		WeaponData->GetAttackPowerAtEnhancementLevel(CurrentLevel);
	const int32 NextAttackPower =
		WeaponData->GetAttackPowerAtEnhancementLevel(NextLevel);
	const int32 GoldCost =
		WeaponData->GetEnhanceGoldCost(CurrentLevel);
	const int32 MaterialCost =
		WeaponData->GetEnhanceMaterialCost(CurrentLevel);
	const float SuccessChance =
		WeaponData->GetEnhanceSuccessChance(CurrentLevel);

	if (TXT_Title)
	{
		TXT_Title->SetText(FText::FromString(TEXT("무기 강화")));
	}

	if (TXT_TargetName)
	{
		TXT_TargetName->SetText(WeaponData->DisplayName);
	}

	if (TXT_CurrentLevel)
	{
		TXT_CurrentLevel->SetText(
			FText::FromString(
				FString::Printf(TEXT("Enhancement  +%d -> +%d"), CurrentLevel, NextLevel)
			)
		);
	}

	if (TXT_AttackPower)
	{
		TXT_AttackPower->SetText(
			FText::FromString(
				FString::Printf(TEXT("Attack Power  %d -> %d"), CurrentAttackPower, NextAttackPower)
			)
		);
	}

	if (TXT_GoldCost)
	{
		TXT_GoldCost->SetText(
			FText::FromString(
				FString::Printf(TEXT("Gold  %d / %d"), InventoryComponent->GetGold(), GoldCost)
			)
		);
	}

	if (TXT_MaterialCost)
	{
		const int32 OwnedMaterialCount =
			WeaponData->EnhanceMaterialItemID.IsNone()
				? 0
				: InventoryComponent->CountItemByID(WeaponData->EnhanceMaterialItemID);

		TXT_MaterialCost->SetText(
			FText::FromString(
				FString::Printf(
					TEXT("%s  %d / %d"),
					*GetMaterialDisplayText(WeaponData->EnhanceMaterialItemID).ToString(),
					OwnedMaterialCount,
					MaterialCost
				)
			)
		);
	}

	if (TXT_SuccessChance)
	{
		TXT_SuccessChance->SetText(
			FText::FromString(
				FString::Printf(TEXT("Success Chance  %.0f%%"), SuccessChance * 100.0f)
			)
		);
	}

	FText FailureReason;
	const bool bCanEnhance =
		bTargetEquippedWeapon
			? InventoryComponent->CanEnhanceEquippedWeapon(FailureReason)
			: InventoryComponent->CanEnhanceWeaponAtSlot(
				TargetInventorySlotIndex,
				FailureReason
			);

	if (TXT_Status)
	{
		TXT_Status->SetText(
			bCanEnhance
				? FText::FromString(TEXT("Ready to enhance."))
				: FailureReason
		);
	}

	if (TXT_Result)
	{
		TXT_Result->SetText(LastResultText);
	}

	if (BTN_Enhance)
	{
		BTN_Enhance->SetIsEnabled(true);
	}
}

ULWeaponEnhanceWindowWidget*
ULWeaponEnhanceWindowWidget::GetActiveWeaponEnhanceWindow()
{
	ULWeaponEnhanceWindowWidget* ActiveWindow =
		ActiveWeaponEnhanceWindow.Get();

	if (!ActiveWindow)
	{
		return nullptr;
	}

	if (!ActiveWindow->IsInViewport())
	{
		return nullptr;
	}

	const ESlateVisibility Visibility =
		ActiveWindow->GetVisibility();

	if (Visibility == ESlateVisibility::Collapsed
		|| Visibility == ESlateVisibility::Hidden)
	{
		return nullptr;
	}

	return ActiveWindow;
}

void ULWeaponEnhanceWindowWidget::NativeConstruct()
{
	bRootSelfHitTestInvisible = false;

	Super::NativeConstruct();

	SetIsFocusable(true);
	ResolveDesignerWidgets();
	BindButtons();

	if (!ObservedPlayerCharacter)
	{
		ObservedPlayerCharacter = Cast<ALPlayerCharacter>(
			UGameplayStatics::GetPlayerCharacter(this, 0)
		);
	}

	ActiveWeaponEnhanceWindow = this;
	RefreshEnhanceWindow();

	if (!IsChangingWindowZOrder())
	{
		BringToFront();
	}

	SetKeyboardFocus();
}

void ULWeaponEnhanceWindowWidget::NativeDestruct()
{
	if (!IsChangingWindowZOrder())
	{
		if (ActiveWeaponEnhanceWindow.Get() == this)
		{
			ActiveWeaponEnhanceWindow.Reset();
		}

		RestoreGameplayInputMode();
	}

	Super::NativeDestruct();
}

FReply ULWeaponEnhanceWindowWidget::NativeOnKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent
)
{
	const FKey PressedKey = InKeyEvent.GetKey();

	if (PressedKey == EKeys::Escape
		|| PressedKey == EKeys::G
		|| PressedKey == EKeys::X)
	{
		CloseWindow();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply ULWeaponEnhanceWindowWidget::NativeOnPreviewKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent
)
{
	const FKey PressedKey = InKeyEvent.GetKey();

	if (PressedKey == EKeys::Escape
		|| PressedKey == EKeys::G
		|| PressedKey == EKeys::X)
	{
		CloseWindow();
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

bool ULWeaponEnhanceWindowWidget::NativeOnDrop(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation
)
{
	ULUIDragDropOperation* DragOperation =
		Cast<ULUIDragDropOperation>(InOperation);

	if (!DragOperation || DragOperation->PayloadType != ELDragPayloadType::Item)
	{
		return false;
	}

	if (DragOperation->bFromEquipmentSlot)
	{
		return TrySetTargetEquippedWeapon();
	}

	if (DragOperation->SourceInventorySlotIndex == INDEX_NONE)
	{
		return false;
	}

	return TrySetTargetFromInventorySlot(
		DragOperation->SourceInventorySlotIndex
	);
}

void ULWeaponEnhanceWindowWidget::HandleEnhanceClicked()
{
	ULInventoryComponent* InventoryComponent =
		ResolveInventoryComponent();

	if (!InventoryComponent)
	{
		return;
	}

	FLWeaponEnhanceResult Result;
	const bool bAttempted =
		bTargetEquippedWeapon
			? InventoryComponent->EnhanceEquippedWeapon(Result)
			: InventoryComponent->EnhanceWeaponAtSlot(
				TargetInventorySlotIndex,
				Result
			);

	LastResultText =
		bAttempted
			? BuildResultText(Result)
			: Result.FailureReason;

	RefreshEnhanceWindow();
}

void ULWeaponEnhanceWindowWidget::HandleClearClicked()
{
	ClearEnhanceTarget();
}

void ULWeaponEnhanceWindowWidget::HandleCloseClicked()
{
	CloseWindow();
}

void ULWeaponEnhanceWindowWidget::ResolveDesignerWidgets()
{
	if (!IMG_TargetIcon)
	{
		IMG_TargetIcon = Cast<UImage>(GetWidgetFromName(TEXT("IMG_TargetIcon")));
	}

	if (!TXT_Title)
	{
		TXT_Title = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_Title")));
	}

	if (!TXT_TargetName)
	{
		TXT_TargetName = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_TargetName")));
	}

	if (!TXT_CurrentLevel)
	{
		TXT_CurrentLevel = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_CurrentLevel")));
	}

	if (!TXT_AttackPower)
	{
		TXT_AttackPower = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_AttackPower")));
	}

	if (!TXT_GoldCost)
	{
		TXT_GoldCost = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_GoldCost")));
	}

	if (!TXT_MaterialCost)
	{
		TXT_MaterialCost = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_MaterialCost")));
	}

	if (!TXT_SuccessChance)
	{
		TXT_SuccessChance = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_SuccessChance")));
	}

	if (!TXT_Status)
	{
		TXT_Status = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_Status")));
	}

	if (!TXT_Result)
	{
		TXT_Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_Result")));
	}

	if (!TXT_EnhanceButton)
	{
		TXT_EnhanceButton = Cast<UTextBlock>(GetWidgetFromName(TEXT("TXT_EnhanceButton")));
	}

	if (!BTN_Enhance)
	{
		BTN_Enhance = Cast<UButton>(GetWidgetFromName(TEXT("BTN_Enhance")));
	}

	if (!BTN_Clear)
	{
		BTN_Clear = Cast<UButton>(GetWidgetFromName(TEXT("BTN_Clear")));
	}

	if (!BTN_Close)
	{
		BTN_Close = Cast<UButton>(GetWidgetFromName(TEXT("BTN_Close")));
	}

	if (!WBP_DragHandle)
	{
		WBP_DragHandle = GetWidgetFromName(TEXT("WBP_DragHandle"));
	}
}

void ULWeaponEnhanceWindowWidget::BindButtons()
{
	if (BTN_Enhance)
	{
		BTN_Enhance->OnClicked.RemoveDynamic(
			this,
			&ULWeaponEnhanceWindowWidget::HandleEnhanceClicked
		);
		BTN_Enhance->OnClicked.AddUniqueDynamic(
			this,
			&ULWeaponEnhanceWindowWidget::HandleEnhanceClicked
		);
	}

	if (BTN_Clear)
	{
		BTN_Clear->OnClicked.RemoveDynamic(
			this,
			&ULWeaponEnhanceWindowWidget::HandleClearClicked
		);
		BTN_Clear->OnClicked.AddUniqueDynamic(
			this,
			&ULWeaponEnhanceWindowWidget::HandleClearClicked
		);
	}

	if (BTN_Close)
	{
		BTN_Close->OnPressed.RemoveDynamic(
			this,
			&ULWeaponEnhanceWindowWidget::HandleCloseClicked
		);
		BTN_Close->OnPressed.AddUniqueDynamic(
			this,
			&ULWeaponEnhanceWindowWidget::HandleCloseClicked
		);

		BTN_Close->OnClicked.RemoveDynamic(
			this,
			&ULWeaponEnhanceWindowWidget::HandleCloseClicked
		);
		BTN_Close->OnClicked.AddUniqueDynamic(
			this,
			&ULWeaponEnhanceWindowWidget::HandleCloseClicked
		);
	}
}

void ULWeaponEnhanceWindowWidget::RestoreGameplayInputMode() const
{
	APlayerController* OwningPlayerController =
		GetOwningPlayer();

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

ULInventoryComponent*
ULWeaponEnhanceWindowWidget::ResolveInventoryComponent() const
{
	ALPlayerCharacter* PlayerCharacter =
		ObservedPlayerCharacter;

	if (!PlayerCharacter)
	{
		PlayerCharacter = Cast<ALPlayerCharacter>(
			UGameplayStatics::GetPlayerCharacter(this, 0)
		);
	}

	return PlayerCharacter
		? PlayerCharacter->GetInventoryComponent()
		: nullptr;
}

bool ULWeaponEnhanceWindowWidget::GetCurrentTargetSlot(
	FLInventorySlot& OutSlot
) const
{
	OutSlot.Clear();

	ULInventoryComponent* InventoryComponent =
		ResolveInventoryComponent();

	if (!InventoryComponent)
	{
		return false;
	}

	if (bTargetEquippedWeapon)
	{
		return InventoryComponent->GetEquippedWeaponSlot(OutSlot);
	}

	if (TargetInventorySlotIndex == INDEX_NONE)
	{
		return false;
	}

	return InventoryComponent->GetSlot(
		TargetInventorySlotIndex,
		OutSlot
	)
		&& !OutSlot.IsEmpty()
		&& OutSlot.ItemData
		&& OutSlot.ItemData->IsWeapon();
}

FText ULWeaponEnhanceWindowWidget::GetMaterialDisplayText(
	FName MaterialItemID
) const
{
	if (MaterialItemID.IsNone())
	{
		return FText::FromString(TEXT("Material"));
	}

	ULInventoryComponent* InventoryComponent =
		ResolveInventoryComponent();

	if (!InventoryComponent)
	{
		return FText::FromName(MaterialItemID);
	}

	const FString ExpectedID = MaterialItemID.ToString();
	const FString DataAssetName = FString(TEXT("DA_")) + ExpectedID;

	for (const FLInventorySlot& InventorySlot : InventoryComponent->GetSlots())
	{
		if (!InventorySlot.IsEmpty()
			&& InventorySlot.ItemData
			&& (InventorySlot.ItemData->ItemID == MaterialItemID
				|| InventorySlot.ItemData->GetName().Equals(
					ExpectedID,
					ESearchCase::IgnoreCase
				)
				|| InventorySlot.ItemData->GetName().Equals(
					DataAssetName,
					ESearchCase::IgnoreCase
				)
				|| InventorySlot.ItemData->DisplayName.ToString().Equals(
					ExpectedID,
					ESearchCase::IgnoreCase
				)))
		{
			return InventorySlot.ItemData->DisplayName;
		}
	}

	return FText::FromName(MaterialItemID);
}

FText ULWeaponEnhanceWindowWidget::BuildResultText(
	const FLWeaponEnhanceResult& Result
) const
{
	if (!Result.bAttempted)
	{
		return Result.FailureReason;
	}

	if (Result.bSuccess)
	{
		return FText::FromString(
			FString::Printf(
				TEXT("Success  +%d -> +%d"),
				Result.PreviousLevel,
				Result.NewLevel
			)
		);
	}

	return FText::FromString(
		FString::Printf(
			TEXT("Failed  +%d remains"),
			Result.PreviousLevel
		)
	);
}

void ULWeaponEnhanceWindowWidget::SetTargetIcon(
	UTexture2D* IconTexture
)
{
	if (!IMG_TargetIcon)
	{
		return;
	}

	if (!IconTexture)
	{
		IMG_TargetIcon->SetRenderOpacity(0.0f);
		return;
	}

	IMG_TargetIcon->SetBrushFromTexture(IconTexture, false);
	IMG_TargetIcon->SetDesiredSizeOverride(FVector2D(112.0f, 112.0f));
	IMG_TargetIcon->SetRenderOpacity(1.0f);
}

void ULWeaponEnhanceWindowWidget::SetEmptyText()
{
	SetTargetIcon(nullptr);

	if (TXT_Title)
	{
		TXT_Title->SetText(FText::FromString(TEXT("무기 강화")));
	}

	if (TXT_TargetName)
	{
		TXT_TargetName->SetText(FText::FromString(TEXT("Drop weapon here")));
	}

	if (TXT_CurrentLevel)
	{
		TXT_CurrentLevel->SetText(FText::FromString(TEXT("Enhancement  -")));
	}

	if (TXT_AttackPower)
	{
		TXT_AttackPower->SetText(FText::FromString(TEXT("Attack Power  -")));
	}

	if (TXT_GoldCost)
	{
		TXT_GoldCost->SetText(FText::FromString(TEXT("Gold  -")));
	}

	if (TXT_MaterialCost)
	{
		TXT_MaterialCost->SetText(FText::FromString(TEXT("Material  -")));
	}

	if (TXT_SuccessChance)
	{
		TXT_SuccessChance->SetText(FText::FromString(TEXT("Success Chance  -")));
	}

	if (TXT_Status)
	{
		TXT_Status->SetText(
			FText::FromString(TEXT("Drag a weapon here or right-click a weapon in inventory."))
		);
	}

	if (TXT_Result)
	{
		TXT_Result->SetText(LastResultText);
	}

	if (BTN_Enhance)
	{
		BTN_Enhance->SetIsEnabled(true);
	}
}
