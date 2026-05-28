#include "LFloatingDamageWidget.h"

#include "Components/TextBlock.h"

void ULFloatingDamageWidget::SetDamageValue(float Damage)
{
	if (!TXT_Damage)
	{
		return;
	}

	TXT_Damage->SetText(
		FText::FromString(
			FString::Printf(TEXT("%.0f"), Damage)
		)
	);
}