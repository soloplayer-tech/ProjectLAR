#include "ProjectLAR/UI/Public/LPlayerUIWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"
#include "ProjectLAR/Player/Public/LPlayerCharacterBase.h"

void ULPlayerUIWidget::SetObservedCharacter(ALPlayerCharacterBase* InCharacter)
{
	ObservedCharacter = InCharacter;
}

void ULPlayerUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 대쉬 아이콘은 네가 원한 대로 쿨타임 중에만 보이게 한다.
	if (IMG_DashIcon)
	{
		IMG_DashIcon->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Dash
	DashCooldownMaterial = InitCooldownImage(IMG_DashCooldownRadial.Get());
	InitCooldownText(TXT_DashCooldown.Get());

	// Q
	QCooldownMaterial = InitCooldownImage(IMG_QCooldownRadial.Get());
	InitCooldownText(TXT_QCooldown.Get());

	// W
	WCooldownMaterial = InitCooldownImage(IMG_WCooldownRadial.Get());
	InitCooldownText(TXT_WCooldown.Get());

	// E
	ECooldownMaterial = InitCooldownImage(IMG_ECooldownRadial.Get());
	InitCooldownText(TXT_ECooldown.Get());

	// R
	RCooldownMaterial = InitCooldownImage(IMG_RCooldownRadial.Get());
	InitCooldownText(TXT_RCooldown.Get());
}

void ULPlayerUIWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!ObservedCharacter)
	{
		return;
	}

	// =========================
	// Dash Cooldown
	// =========================

	const bool bDashOnCooldown = ObservedCharacter->IsDashOnCooldown();
	const float DashRemaining = ObservedCharacter->GetDashCooldownRemaining();
	const float DashRatio = ObservedCharacter->GetDashCooldownRatio();

	if (IMG_DashIcon)
	{
		IMG_DashIcon->SetVisibility(
			bDashOnCooldown
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed
		);
	}

	UpdateCooldownUI(
		IMG_DashCooldownRadial.Get(),
		DashCooldownMaterial.Get(),
		TXT_DashCooldown.Get(),
		bDashOnCooldown,
		DashRemaining,
		DashRatio
	);

	// =========================
	// Skill Cooldown
	// =========================

	ALPlayerCharacter* PlayerCharacter = Cast<ALPlayerCharacter>(ObservedCharacter);

	if (!PlayerCharacter)
	{
		return;
	}

	// Q
	UpdateCooldownUI(
		IMG_QCooldownRadial.Get(),
		QCooldownMaterial.Get(),
		TXT_QCooldown.Get(),
		PlayerCharacter->IsSkillOnCooldown(ELPlayerSkillSlot::Q),
		PlayerCharacter->GetSkillCooldownRemaining(ELPlayerSkillSlot::Q),
		PlayerCharacter->GetSkillCooldownRatio(ELPlayerSkillSlot::Q)
	);

	// W
	UpdateCooldownUI(
		IMG_WCooldownRadial.Get(),
		WCooldownMaterial.Get(),
		TXT_WCooldown.Get(),
		PlayerCharacter->IsSkillOnCooldown(ELPlayerSkillSlot::W),
		PlayerCharacter->GetSkillCooldownRemaining(ELPlayerSkillSlot::W),
		PlayerCharacter->GetSkillCooldownRatio(ELPlayerSkillSlot::W)
	);

	// E
	UpdateCooldownUI(
		IMG_ECooldownRadial.Get(),
		ECooldownMaterial.Get(),
		TXT_ECooldown.Get(),
		PlayerCharacter->IsSkillOnCooldown(ELPlayerSkillSlot::E),
		PlayerCharacter->GetSkillCooldownRemaining(ELPlayerSkillSlot::E),
		PlayerCharacter->GetSkillCooldownRatio(ELPlayerSkillSlot::E)
	);

	// R
	UpdateCooldownUI(
		IMG_RCooldownRadial.Get(),
		RCooldownMaterial.Get(),
		TXT_RCooldown.Get(),
		PlayerCharacter->IsSkillOnCooldown(ELPlayerSkillSlot::R),
		PlayerCharacter->GetSkillCooldownRemaining(ELPlayerSkillSlot::R),
		PlayerCharacter->GetSkillCooldownRatio(ELPlayerSkillSlot::R)
	);
}

UMaterialInstanceDynamic* ULPlayerUIWidget::InitCooldownImage(UImage* CooldownImage)
{
	if (!CooldownImage)
	{
		return nullptr;
	}

	UMaterialInstanceDynamic* DynamicMaterial = CooldownImage->GetDynamicMaterial();

	CooldownImage->SetVisibility(ESlateVisibility::Collapsed);

	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(
			TEXT("CooldownPercent"),
			0.0f
		);
	}

	return DynamicMaterial;
}

void ULPlayerUIWidget::InitCooldownText(UTextBlock* CooldownText)
{
	if (!CooldownText)
	{
		return;
	}

	CooldownText->SetVisibility(ESlateVisibility::Collapsed);
	CooldownText->SetText(FText::GetEmpty());
}

void ULPlayerUIWidget::UpdateCooldownUI(
	UImage* CooldownImage,
	UMaterialInstanceDynamic* CooldownMaterial,
	UTextBlock* CooldownText,
	bool bOnCooldown,
	float Remaining,
	float Ratio
)
{
	if (CooldownImage)
	{
		CooldownImage->SetVisibility(
			bOnCooldown
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed
		);
	}

	if (CooldownMaterial)
	{
		CooldownMaterial->SetScalarParameterValue(
			TEXT("CooldownPercent"),
			bOnCooldown ? Ratio : 0.0f
		);
	}

	if (CooldownText)
	{
		if (bOnCooldown)
		{
			CooldownText->SetVisibility(ESlateVisibility::Visible);

			CooldownText->SetText(
				FText::FromString(
					FString::Printf(TEXT("%.1f"), Remaining)
				)
			);
		}
		else
		{
			CooldownText->SetVisibility(ESlateVisibility::Collapsed);
			CooldownText->SetText(FText::GetEmpty());
		}
	}
}