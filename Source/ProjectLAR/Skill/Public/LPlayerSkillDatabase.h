#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LPlayerSkillDataAsset.h"
#include "LPlayerSkillDatabase.generated.h"

UCLASS(BlueprintType)
class PROJECTLAR_API ULPlayerSkillDatabase : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Skill|Database")
	ULPlayerSkillDataAsset* FindSkillDataAsset(ELPlayerSkillID SkillID) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Database")
	TArray<TObjectPtr<ULPlayerSkillDataAsset>> SkillDataAssets;
};
