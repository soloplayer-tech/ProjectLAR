#include "ProjectLAR/UI/Public/LPlayerUIWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
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
	ALPlayerCharacter* PlayerCharacter = Cast<ALPlayerCharacter>(ObservedCharacter);

	const bool bDashOnCooldown = ObservedCharacter->IsDashOnCooldown();
	const float DashRemaining = ObservedCharacter->GetDashCooldownRemaining();
	const float DashRatio = ObservedCharacter->GetDashCooldownRatio();

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
		PB_IdentityGauge->SetPercent(PlayerCharacter->GetIdentityRatio());
	}
	
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


	if (!PlayerCharacter)
	{
		return;
	}

	// Q Slot
	UpdateSkillSlotCooldownUI(
		ELPlayerSkillSlot::Q,
		IMG_QCooldownRadial.Get(),
		QCooldownMaterial.Get(),
		TXT_QCooldown.Get()
	);

	// W Slot
	UpdateSkillSlotCooldownUI(
		ELPlayerSkillSlot::W,
		IMG_WCooldownRadial.Get(),
		WCooldownMaterial.Get(),
		TXT_WCooldown.Get()
	);

	// E Slot
	UpdateSkillSlotCooldownUI(
		ELPlayerSkillSlot::E,
		IMG_ECooldownRadial.Get(),
		ECooldownMaterial.Get(),
		TXT_ECooldown.Get()
	);

	// R Slot
	UpdateSkillSlotCooldownUI(
		ELPlayerSkillSlot::R,
		IMG_RCooldownRadial.Get(),
		RCooldownMaterial.Get(),
		TXT_RCooldown.Get()
	);
	
	UpdateCastBarUI();
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

void ULPlayerUIWidget::UpdateSkillSlotCooldownUI(
	ELPlayerSkillSlot SkillSlot,
	UImage* CooldownImage,
	UMaterialInstanceDynamic* CooldownMaterial,
	UTextBlock* CooldownText
)
{
	ALPlayerCharacter* PlayerCharacter =
		Cast<ALPlayerCharacter>(ObservedCharacter);

	if (!PlayerCharacter)
	{
		return;
	}

	const ELPlayerSkillID SkillID =
		PlayerCharacter->GetEquippedSkillID(SkillSlot);

	const bool bOnCooldown =
		PlayerCharacter->IsSkillIDOnCooldown(SkillID);

	const float Remaining =
		PlayerCharacter->GetSkillIDCooldownRemaining(SkillID);

	const float Ratio =
		PlayerCharacter->GetSkillIDCooldownRatio(SkillID);

	UpdateCooldownUI(
		CooldownImage,
		CooldownMaterial,
		CooldownText,
		bOnCooldown,
		Remaining,
		Ratio
	);
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
