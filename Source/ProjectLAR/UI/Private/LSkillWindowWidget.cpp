#include "ProjectLAR/UI/Public/LSkillWindowWidget.h"

#include "ProjectLAR/UI/Public/LSkillIconWidget.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/PanelSlot.h"
#include "Components/PanelWidget.h"

void ULSkillWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeSkillIcons();
}

void ULSkillWindowWidget::SetOwningPlayerCharacter(ALPlayerCharacter* InPlayerCharacter)
{
	OwningPlayerCharacter = InPlayerCharacter;
	InitializeSkillIcons();
}

void ULSkillWindowWidget::InitializeSkillIcons()
{
	const auto ResolveSkillName =
		[this](ELPlayerSkillID SkillID, const FText& FallbackName)
		{
			if (OwningPlayerCharacter)
			{
				const FText DataAssetName =
					OwningPlayerCharacter->GetSkillDisplayName(SkillID);

				if (!DataAssetName.IsEmpty())
				{
					return DataAssetName;
				}
			}

			return FallbackName;
		};

	const auto ResolveSkillIcon =
		[this](ELPlayerSkillID SkillID, UTexture2D* FallbackIcon)
		{
			if (OwningPlayerCharacter)
			{
				if (UTexture2D* DataAssetIcon =
					OwningPlayerCharacter->GetSkillIconTexture(SkillID))
				{
					return DataAssetIcon;
				}
			}

			return FallbackIcon;
		};

	if (WBP_MeteorSkillIcon)
	{
		WBP_MeteorSkillIcon->SetOwningWindow(this);
		WBP_MeteorSkillIcon->SetSkillData(
			ELPlayerSkillID::Meteor,
			ResolveSkillName(ELPlayerSkillID::Meteor, FText::FromString(TEXT("Meteor"))),
			ResolveSkillIcon(ELPlayerSkillID::Meteor, MeteorIconTexture.Get())
		);
	}

	if (WBP_IceLanceSkillIcon)
	{
		WBP_IceLanceSkillIcon->SetOwningWindow(this);
		WBP_IceLanceSkillIcon->SetSkillData(
			ELPlayerSkillID::IceLance,
			ResolveSkillName(ELPlayerSkillID::IceLance, FText::FromString(TEXT("Ice Lance"))),
			ResolveSkillIcon(ELPlayerSkillID::IceLance, IceLanceIconTexture.Get())
		);
	}

	if (WBP_ThunderSkillIcon)
	{
		WBP_ThunderSkillIcon->SetOwningWindow(this);
		WBP_ThunderSkillIcon->SetSkillData(
			ELPlayerSkillID::Thunder,
			ResolveSkillName(ELPlayerSkillID::Thunder, FText::FromString(TEXT("Thunder"))),
			ResolveSkillIcon(ELPlayerSkillID::Thunder, ThunderIconTexture.Get())
		);
	}

	if (WBP_WindSkillIcon)
	{
		WBP_WindSkillIcon->SetOwningWindow(this);
		WBP_WindSkillIcon->SetSkillData(
			ELPlayerSkillID::Wind,
			ResolveSkillName(ELPlayerSkillID::Wind, FText::FromString(TEXT("Wind"))),
			ResolveSkillIcon(ELPlayerSkillID::Wind, WindIconTexture.Get())
		);
	}

	if (ULSkillIconWidget* FrostFieldSkillIcon = GetOrCreateFrostFieldSkillIcon())
	{
		FrostFieldSkillIcon->SetOwningWindow(this);
		UTexture2D* FrostFieldFallbackIcon =
			FrostFieldIconTexture ? FrostFieldIconTexture.Get() : WindIconTexture.Get();

		FrostFieldSkillIcon->SetSkillData(
			ELPlayerSkillID::FrostField,
			ResolveSkillName(ELPlayerSkillID::FrostField, FText::FromString(TEXT("Frost Field"))),
			ResolveSkillIcon(ELPlayerSkillID::FrostField, FrostFieldFallbackIcon)
		);
	}
}

ULSkillIconWidget* ULSkillWindowWidget::GetOrCreateFrostFieldSkillIcon()
{
	if (WBP_FrostFieldSkillIcon)
	{
		return WBP_FrostFieldSkillIcon;
	}

	if (!WBP_WindSkillIcon || !WBP_WindSkillIcon->Slot)
	{
		return nullptr;
	}

	UPanelWidget* ParentPanel = WBP_WindSkillIcon->Slot->Parent;

	if (!ParentPanel)
	{
		return nullptr;
	}

	WBP_FrostFieldSkillIcon = CreateWidget<ULSkillIconWidget>(
		GetWorld(),
		WBP_WindSkillIcon->GetClass()
	);

	if (!WBP_FrostFieldSkillIcon)
	{
		return nullptr;
	}

	ParentPanel->AddChild(WBP_FrostFieldSkillIcon);
	return WBP_FrostFieldSkillIcon;
}
