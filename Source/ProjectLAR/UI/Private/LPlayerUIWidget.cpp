#include "ProjectLAR/UI/Public/LPlayerUIWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"
#include "ProjectLAR/Player/Public/LPlayerCharacterBase.h"

void ULPlayerUIWidget::SetObservedCharacter(
	ALPlayerCharacterBase* InCharacter
)
{
	ObservedCharacter = InCharacter;
}

void ULPlayerUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IMG_DashIcon)
	{
		IMG_DashIcon->SetVisibility(ESlateVisibility::Collapsed);
	}

	DashCooldownMaterial =
		InitCooldownImage(IMG_DashCooldownRadial.Get());

	InitCooldownText(TXT_DashCooldown.Get());

	if (PB_CastProgress)
	{
		PB_CastProgress->SetVisibility(ESlateVisibility::Collapsed);
		PB_CastProgress->SetPercent(0.0f);
	}

	if (TXT_CastName)
	{
		TXT_CastName->SetVisibility(ESlateVisibility::Collapsed);
		TXT_CastName->SetText(FText::GetEmpty());
	}

	if (TXT_CastRemaining)
	{
		TXT_CastRemaining->SetVisibility(ESlateVisibility::Collapsed);
		TXT_CastRemaining->SetText(FText::GetEmpty());
	}

	if (IMG_IdentityFlameOverlay)
	{
		IMG_IdentityFlameOverlay->SetVisibility(
			ESlateVisibility::Collapsed
		);

		IMG_IdentityFlameOverlay->SetRenderOpacity(0.0f);
	}
}

void ULPlayerUIWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!ObservedCharacter)
	{
		return;
	}

	ALPlayerCharacter* PlayerCharacter =
		Cast<ALPlayerCharacter>(ObservedCharacter);

	if (!PlayerCharacter)
	{
		return;
	}

	if (PB_PlayerHP)
	{
		PB_PlayerHP->SetPercent(PlayerCharacter->GetHPRatio());
	}

	if (PB_PlayerMP)
	{
		PB_PlayerMP->SetPercent(PlayerCharacter->GetManaRatio());
	}

	if (PB_IdentityGauge)
	{
		PB_IdentityGauge->SetPercent(
			PlayerCharacter->GetIdentityRatio()
		);
	}

	UpdateIdentityActiveUI(PlayerCharacter);

	const bool bDashOnCooldown =
		ObservedCharacter->IsDashOnCooldown();

	const float DashRemaining =
		ObservedCharacter->GetDashCooldownRemaining();

	const float DashRatio =
		ObservedCharacter->GetDashCooldownRatio();

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

	UpdateCastBarUI();
}

UMaterialInstanceDynamic* ULPlayerUIWidget::InitCooldownImage(
	UImage* CooldownImage
)
{
	if (!CooldownImage)
	{
		return nullptr;
	}

	UMaterialInstanceDynamic* DynamicMaterial =
		CooldownImage->GetDynamicMaterial();

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

void ULPlayerUIWidget::UpdateIdentityActiveUI(
	ALPlayerCharacter* PlayerCharacter
)
{
	if (!PlayerCharacter)
	{
		return;
	}

	const bool bIdentityActive =
		PlayerCharacter->IsIdentityActive();

	if (!IMG_IdentityFlameOverlay)
	{
		return;
	}

	if (bIdentityActive)
	{
		IMG_IdentityFlameOverlay->SetVisibility(
			ESlateVisibility::HitTestInvisible
		);

		IMG_IdentityFlameOverlay->SetRenderOpacity(1.0f);
	}
	else
	{
		IMG_IdentityFlameOverlay->SetVisibility(
			ESlateVisibility::Collapsed
		);

		IMG_IdentityFlameOverlay->SetRenderOpacity(0.0f);
	}
}

void ULPlayerUIWidget::UpdateCastBarUI()
{
	ALPlayerCharacter* PlayerCharacter =
		Cast<ALPlayerCharacter>(ObservedCharacter);

	if (!PlayerCharacter)
	{
		return;
	}

	const bool bCasting = PlayerCharacter->IsCasting();

	if (PB_CastProgress)
	{
		PB_CastProgress->SetVisibility(
			bCasting
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed
		);

		PB_CastProgress->SetPercent(
			bCasting ? PlayerCharacter->GetCastRatio() : 0.0f
		);
	}

	if (TXT_CastName)
	{
		TXT_CastName->SetVisibility(
			bCasting
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed
		);

		if (bCasting)
		{
			const ELPlayerSkillID CastingSkillID =
				PlayerCharacter->GetCastingSkillID();

			FString SkillName = TEXT("");

			switch (CastingSkillID)
			{
			case ELPlayerSkillID::Meteor:
				SkillName = TEXT("Meteor");
				break;

			case ELPlayerSkillID::Thunder:
				SkillName = TEXT("Thunder");
				break;

			default:
				SkillName = TEXT("Casting");
				break;
			}

			TXT_CastName->SetText(FText::FromString(SkillName));
		}
		else
		{
			TXT_CastName->SetText(FText::GetEmpty());
		}
	}

	if (TXT_CastRemaining)
	{
		TXT_CastRemaining->SetVisibility(
			bCasting
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed
		);

		if (bCasting)
		{
			TXT_CastRemaining->SetText(
				FText::FromString(
					FString::Printf(
						TEXT("%.1f"),
						PlayerCharacter->GetCastRemaining()
					)
				)
			);
		}
		else
		{
			TXT_CastRemaining->SetText(FText::GetEmpty());
		}
	}
}