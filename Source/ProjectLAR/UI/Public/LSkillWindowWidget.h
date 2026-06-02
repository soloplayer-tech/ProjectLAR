#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LSkillWindowWidget.generated.h"

class ALPlayerCharacter;

UCLASS()
class PROJECTLAR_API ULSkillWindowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetOwningPlayerCharacter(ALPlayerCharacter* InPlayerCharacter);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	TObjectPtr<ALPlayerCharacter> OwningPlayerCharacter;
};