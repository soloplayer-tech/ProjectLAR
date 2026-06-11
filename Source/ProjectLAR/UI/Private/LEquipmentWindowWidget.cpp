#include "ProjectLAR/UI/Public/LEquipmentWindowWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"
#include "ProjectLAR/UI/Public/LEquipmentPreviewActor.h"
#include "ProjectLAR/UI/Public/LEquipmentWeaponSlotWidget.h"

void ULEquipmentWindowWidget::SetObservedPlayer(
	ALPlayerCharacter* InObservedPlayerCharacter
)
{
	ObservedPlayerCharacter = InObservedPlayerCharacter;

	if (WBP_WeaponSlot && ObservedPlayerCharacter)
	{
		WBP_WeaponSlot->SetOwningWindow(this);
		WBP_WeaponSlot->SetupWeaponSlot(
			ObservedPlayerCharacter->GetInventoryComponent()
		);
	}

	RefreshEquipmentWindow();
}

void ULEquipmentWindowWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ResolveDesignerWidgets();
	BuildDefaultWidgetTree();
	ResolveDesignerWidgets();

	if (TXT_Title)
	{
		TXT_Title->SetText(FText::FromString(TEXT("장비")));
	}
}

void ULEquipmentWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResolveDesignerWidgets();
	BuildDefaultWidgetTree();
	ResolveDesignerWidgets();
	SetupPreviewBrush();

	if (TXT_Title)
	{
		TXT_Title->SetText(FText::FromString(TEXT("장비")));
	}

	RefreshEquipmentWindow();
}

void ULEquipmentWindowWidget::NativeDestruct()
{
	if (!IsChangingWindowZOrder())
	{
		DestroyPreviewActor();
	}

	Super::NativeDestruct();
}

void ULEquipmentWindowWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GetVisibility() != ESlateVisibility::Collapsed
		&& GetVisibility() != ESlateVisibility::Hidden)
	{
		RefreshEquipmentWindow();
	}
}

void ULEquipmentWindowWidget::RefreshEquipmentWindow()
{
	ResolveDesignerWidgets();

	if (WBP_WeaponSlot && ObservedPlayerCharacter)
	{
		WBP_WeaponSlot->SetOwningWindow(this);
		WBP_WeaponSlot->SetupWeaponSlot(
			ObservedPlayerCharacter->GetInventoryComponent()
		);
	}

	EnsurePreviewActor();

	if (PreviewActor && ObservedPlayerCharacter)
	{
		PreviewActor->SetupFromPlayer(ObservedPlayerCharacter);
	}
}

void ULEquipmentWindowWidget::ResolveDesignerWidgets()
{
	if (!WBP_WeaponSlot)
	{
		WBP_WeaponSlot =
			Cast<ULEquipmentWeaponSlotWidget>(
				GetWidgetFromName(TEXT("WBP_WeaponSlot"))
			);
	}

	if (!IMG_CharacterPreview)
	{
		IMG_CharacterPreview =
			Cast<UImage>(
				GetWidgetFromName(TEXT("IMG_CharacterPreview"))
			);
	}

	if (!TXT_Title)
	{
		TXT_Title =
			Cast<UTextBlock>(
				GetWidgetFromName(TEXT("TXT_Title"))
			);
	}
}

void ULEquipmentWindowWidget::BuildDefaultWidgetTree()
{
	if (!WidgetTree || !bUseCodeGeneratedLayout)
	{
		return;
	}

	if (WidgetTree->RootWidget && WBP_WeaponSlot && IMG_CharacterPreview)
	{
		return;
	}

	UCanvasPanel* RootCanvas =
		WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = RootCanvas;

	UBorder* WindowBorder =
		WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	WindowBorder->SetBrushColor(FLinearColor(0.01f, 0.015f, 0.02f, 0.97f));
	WindowBorder->SetPadding(FMargin(14.0f));

	UCanvasPanelSlot* WindowSlot = RootCanvas->AddChildToCanvas(WindowBorder);
	WindowSlot->SetAnchors(FAnchors(0.5f, 0.5f));
	WindowSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	WindowSlot->SetSize(FVector2D(1180.0f, 760.0f));
	WindowSlot->SetPosition(FVector2D::ZeroVector);

	UVerticalBox* RootVertical =
		WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	WindowBorder->SetContent(RootVertical);

	TXT_Title =
		WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	TXT_Title->SetJustification(ETextJustify::Center);
	TXT_Title->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.86f, 0.45f, 1.0f)));
	TXT_Title->SetText(FText::FromString(TEXT("장비")));
	UVerticalBoxSlot* TitleSlot = RootVertical->AddChildToVerticalBox(TXT_Title);
	TitleSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));

	UHorizontalBox* ContentBox =
		WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	UVerticalBoxSlot* ContentSlot = RootVertical->AddChildToVerticalBox(ContentBox);
	ContentSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	UBorder* LeftPanel =
		WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	LeftPanel->SetBrushColor(FLinearColor(0.015f, 0.02f, 0.025f, 0.96f));
	LeftPanel->SetPadding(FMargin(12.0f));
	UHorizontalBoxSlot* LeftSlot = ContentBox->AddChildToHorizontalBox(LeftPanel);
	LeftSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	LeftSlot->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));

	UVerticalBox* LeftContent =
		WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	LeftPanel->SetContent(LeftContent);

	UTextBlock* WeaponLabel =
		WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	WeaponLabel->SetText(FText::FromString(TEXT("무기")));
	WeaponLabel->SetJustification(ETextJustify::Center);
	WeaponLabel->SetColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.9f, 0.95f, 1.0f)));
	LeftContent->AddChildToVerticalBox(WeaponLabel);

	WBP_WeaponSlot =
		WidgetTree->ConstructWidget<ULEquipmentWeaponSlotWidget>(
			ULEquipmentWeaponSlotWidget::StaticClass()
		);
	UVerticalBoxSlot* WeaponSlot = LeftContent->AddChildToVerticalBox(WBP_WeaponSlot);
	WeaponSlot->SetPadding(FMargin(0.0f, 12.0f, 0.0f, 0.0f));

	UBorder* PreviewPanel =
		WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	PreviewPanel->SetBrushColor(FLinearColor(0.008f, 0.013f, 0.018f, 1.0f));
	PreviewPanel->SetPadding(FMargin(6.0f));
	UHorizontalBoxSlot* PreviewSlot = ContentBox->AddChildToHorizontalBox(PreviewPanel);
	PreviewSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	USizeBox* PreviewSizeBox =
		WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	PreviewSizeBox->SetWidthOverride(640.0f);
	PreviewSizeBox->SetHeightOverride(660.0f);
	PreviewPanel->SetContent(PreviewSizeBox);

	IMG_CharacterPreview =
		WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	PreviewSizeBox->AddChild(IMG_CharacterPreview);

	UBorder* RightPanel =
		WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	RightPanel->SetBrushColor(FLinearColor(0.015f, 0.02f, 0.025f, 0.96f));
	RightPanel->SetPadding(FMargin(12.0f));
	UHorizontalBoxSlot* RightSlot = ContentBox->AddChildToHorizontalBox(RightPanel);
	RightSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	RightSlot->SetPadding(FMargin(12.0f, 0.0f, 0.0f, 0.0f));

	USizeBox* RightSizeBox =
		WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	RightSizeBox->SetWidthOverride(220.0f);
	RightPanel->SetContent(RightSizeBox);
}

void ULEquipmentWindowWidget::EnsurePreviewActor()
{
	if (PreviewActor || !GetWorld())
	{
		return;
	}

	if (!PreviewRenderTarget)
	{
		PreviewRenderTarget =
			NewObject<UTextureRenderTarget2D>(this, TEXT("EquipmentPreviewRT"));
		PreviewRenderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
		PreviewRenderTarget->ClearColor = FLinearColor(0.005f, 0.008f, 0.01f, 1.0f);
		PreviewRenderTarget->InitAutoFormat(768, 768);
		PreviewRenderTarget->UpdateResourceImmediate(true);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.ObjectFlags |= RF_Transient;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PreviewActor = GetWorld()->SpawnActor<ALEquipmentPreviewActor>(
		ALEquipmentPreviewActor::StaticClass(),
		FVector(0.0f, 0.0f, -50000.0f),
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (PreviewActor)
	{
		PreviewActor->SetRenderTarget(PreviewRenderTarget);
	}
}

void ULEquipmentWindowWidget::DestroyPreviewActor()
{
	if (PreviewActor)
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}
}

void ULEquipmentWindowWidget::SetupPreviewBrush()
{
	if (!IMG_CharacterPreview)
	{
		return;
	}

	if (!PreviewRenderTarget)
	{
		PreviewRenderTarget =
			NewObject<UTextureRenderTarget2D>(this, TEXT("EquipmentPreviewRT"));
		PreviewRenderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
		PreviewRenderTarget->ClearColor = FLinearColor(0.005f, 0.008f, 0.01f, 1.0f);
		PreviewRenderTarget->InitAutoFormat(768, 768);
		PreviewRenderTarget->UpdateResourceImmediate(true);
	}

	FSlateBrush PreviewBrush;
	PreviewBrush.SetResourceObject(PreviewRenderTarget);
	PreviewBrush.ImageSize = FVector2D(640.0f, 660.0f);
	PreviewBrush.TintColor = FSlateColor(FLinearColor::White);
	IMG_CharacterPreview->SetBrush(PreviewBrush);
	IMG_CharacterPreview->SetColorAndOpacity(FLinearColor::White);
}
