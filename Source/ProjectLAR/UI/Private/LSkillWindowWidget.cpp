#include "ProjectLAR/UI/Public/LSkillWindowWidget.h"
#include "ProjectLAR/Player/Public/LPlayerCharacter.h"

void ULSkillWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULSkillWindowWidget::SetOwningPlayerCharacter(ALPlayerCharacter* InPlayerCharacter)
{
	OwningPlayerCharacter = InPlayerCharacter;
}