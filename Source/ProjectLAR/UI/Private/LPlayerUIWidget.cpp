// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectLAR/UI/Public/LPlayerUIWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ProjectLAR/Player/Public/LPlayerCharacterBase.h"

void ULPlayerUIWidget::SetObservedCharacter(ALPlayerCharacterBase* InCharacter)
{
	ObservedCharacter = InCharacter;
}

void ULPlayerUIWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!ObservedCharacter)
	{
		return;
	}

	// 대쉬 쿨타임 UI
	const bool bDashOnCooldown = ObservedCharacter->IsDashOnCooldown();
	const float DashRemaining = ObservedCharacter->GetDashCooldownRemaining();
	const float DashRatio = ObservedCharacter->GetDashCooldownRatio();

	if (PB_DashCooldown)
	{
		PB_DashCooldown->SetPercent(DashRatio);
	}

	if (TXT_DashCooldown)
	{
		if (bDashOnCooldown)
		{
			TXT_DashCooldown->SetText(
				FText::FromString(
					FString::Printf(TEXT("%.1f"), DashRemaining)
				)
			);
		}
		else
		{
			TXT_DashCooldown->SetText(FText::GetEmpty());
		}
	}
}