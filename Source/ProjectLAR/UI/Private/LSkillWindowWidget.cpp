#include "ProjectLAR/UI/Public/LSkillWindowWidget.h"

#include "ProjectLAR/UI/Public/LSkillIconWidget.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"

void ULSkillWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeSkillIcons();
}

void ULSkillWindowWidget::SetOwningPlayerCharacter(ALPlayerCharacter* InPlayerCharacter)
{
	OwningPlayerCharacter = InPlayerCharacter;
}

void ULSkillWindowWidget::InitializeSkillIcons()
{
	if (WBP_MeteorSkillIcon)
	{
		WBP_MeteorSkillIcon->SetSkillData(
			ELPlayerSkillID::Meteor,
			FText::FromString(TEXT("Meteor")),
			MeteorIconTexture
		);
	}

	if (WBP_IceLanceSkillIcon)
	{
		WBP_IceLanceSkillIcon->SetSkillData(
			ELPlayerSkillID::IceLance,
			FText::FromString(TEXT("Ice Lance")),
			IceLanceIconTexture
		);
	}

	if (WBP_ThunderSkillIcon)
	{
		WBP_ThunderSkillIcon->SetSkillData(
			ELPlayerSkillID::Thunder,
			FText::FromString(TEXT("Thunder")),
			ThunderIconTexture
		);
	}

	if (WBP_WindSkillIcon)
	{
		WBP_WindSkillIcon->SetSkillData(
			ELPlayerSkillID::Wind,
			FText::FromString(TEXT("Wind")),
			WindIconTexture
		);
	}
}