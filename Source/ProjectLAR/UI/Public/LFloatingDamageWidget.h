#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LFloatingDamageWidget.generated.h"

class UTextBlock;

UCLASS()
class PROJECTLAR_API ULFloatingDamageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetDamageValue(float Damage);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_Damage;
};